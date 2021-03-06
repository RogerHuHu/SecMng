/**
 * \file SecMng.cpp
 *
 * \brief Implements the functions for secret management system.
 *
 * \author Roger(neverchangehuhu@gmail.com)
 *
 * \date 2017-01-05
 */

#include <iostream>
#include <functional>
#include <inttypes.h>
#include <stdlib.h>
#include "SecMng.hpp"
#include "frozen.h"

namespace secmng {
    void *instance;
    //Ctor
    SecMng::SecMng(const std::string &httpPort, const std::string &webRootDir) {
        m_httpPort = httpPort;
        m_webRootDir = webRootDir;

        m_loginPrefix = MG_MK_STR("/SecMng/login");
        m_getSecsPrefix = MG_MK_STR("/SecMng/GetSecrets");
        m_saveSecsPrefix = MG_MK_STR("/SecMng/SaveSecrets");
        m_delSecsPrefix = MG_MK_STR("/SecMng/DelSecrets");

        login = new Login("username", "password", 10, 300.0);
        acntMng = new AccountMng("type", "target", "username", "password");
        ptMatch = new PatternMatch();
    }

    //Dtor
    SecMng::~SecMng() {
        mg_mgr_free(&m_mgr);

        delete login;
        delete acntMng;
        delete ptMatch;
    }

    /**
     * Init SecMng HTTP server
     */
    void SecMng::Init() {
        cs_stat_t st;
        mg_mgr_init(&m_mgr, NULL);
        m_nc = mg_bind(&m_mgr, m_httpPort.c_str(), SecMng::EvHandler);
        if(m_nc == NULL) {
            std::cerr << "Cannot bind to " << m_httpPort << std::endl; 
            exit(1);
        }

        //Set up HTTP server parameters
        mg_set_protocol_http_websocket(m_nc);
        m_httpServerOpts.document_root = m_webRootDir.c_str();
        m_httpServerOpts.index_files = "html/login.html";
        mg_set_timer(m_nc, mg_time() + login->GetSessionChkIntv());

        if(mg_stat(m_httpServerOpts.document_root, &st) != 0) {
            std::cerr << "Cannot find web_root directory, exiting" << std::endl;
            exit(1);
        }
    }

    /**
     * Run SecMng HTTP server
     */
    void SecMng::Run(int interval) {
        std::cout << "Starting SecMng web server on port " 
            << m_httpPort << std::endl;
        m_lastTime = 0;
        while(true) {
            time_t now = time(NULL);
            mg_mgr_poll(&m_mgr, interval);
            if(now - m_lastTime > 0) {
                BroadcastData(&m_mgr);
                m_lastTime = now;
            }
        }
    }

    /**
     * Handler HTTP events
     */
    void SecMng::EvHandler(struct mg_connection *nc, int ev, void *evData) {
        struct http_message *hm = (struct http_message *)evData;
        SecMng *mng = (SecMng *)instance;

        switch(ev) {
            case MG_EV_HTTP_REQUEST: {
                if(mng->HasPrefix(&hm->uri, &(mng->m_loginPrefix))) {
                    //Login request.
                    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
                    struct Session s;
                    if((mng->login)->HandleLogin(hm, s)) {
                        std::string ckName = (mng->login)->GetCookieName();
                        mg_printf_http_chunk(nc, "{\"result\":1, "
                                "\"cookiename\":\"%s\", \"id\":\"%llx\"}\n",
                                ckName.c_str(), s.id);
                    } else {
                        mg_printf_http_chunk(nc, "{\"result\":0}");
                    }
                    mg_send_http_chunk(nc, "", 0);
                } else if(mng->HasPrefix(&hm->uri, &(mng->m_getSecsPrefix))) {
                    //Get secret information request.
                    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
                    struct Session s;
                    if(!(mng->login)->GetSession(hm, s)) {
                        mg_printf_http_chunk(nc, "{\"result\":-1}");
                    } else {
                        string req = hm->uri.p;
                        (mng->ptMatch)->CaculateFail("\r\n\r\n");
                        int idx = (mng->ptMatch)->Match(req, "\r\n\r\n");
                        req = req.substr(idx, req.size() - idx - 1);
                        char dst[req.size()];
                        mg_url_decode(req.c_str(), req.size(), dst, req.size(), 1);
                        req = dst;
                        string draw = mng->ExtractParam(req, "sEcho=");
                        struct Condition cond;
                        string orderClm = mng->ExtractParam(req, "iSortCol_0=");
                        cond.orderClm = atoi(orderClm.c_str());
                        cond.orderDir = mng->ExtractParam(req, "sSortDir_0=");
                        cond.startIdx = mng->ExtractParam(req, "iDisplayStart=");
                        cond.length = mng->ExtractParam(req, "iDisplayLength=");
                        cond.searchVal = mng->ExtractParam(req, "sSearch=");
                        string ret = "{\"result\":1, ";
                        ret += "\"draw\":" + draw + ", ";

                        int recordsTotal = 0;
                        int recordsFiltered = 0;
                        std::list<struct Account> acnts;
                        if((mng->acntMng)->GetAccounts(acnts, recordsTotal,
                                    recordsFiltered, cond, s.flag)) {
                            ret += "\"recordsTotal\":" +
                                    std::to_string(recordsTotal) + ", " +
                                    "\"recordsFiltered\":" +
                                    std::to_string(recordsFiltered) + ", " +
                                    "\"data\": [";
                            for(std::list<struct Account>::iterator iter = acnts.begin();
                                    iter != acnts.end(); ++iter) {
                                struct Account acnt = *iter; 
                                char chId[10];
                                sprintf(chId, "%d", acnt.id);
                                ret += "{\"id\":" + (std::string)chId +
                                    ", \"target\":\"" + acnt.target + 
                                    "\", \"username\":\"" + acnt.username +
                                    "\", \"password\":\"" + acnt.password + "\"},";
                            }
                            ret = ret.substr(0, ret.size() - 1);
                            ret += "]}";
                        } else {
                            ret += "\"recordsTotal\":0, \"recordsFiltered\":0, \"data\":[]}";
                        }
                        mg_printf_http_chunk(nc, "%s", ret.c_str());
                    }
                    mg_send_http_chunk(nc, "", 0);
                } else if(mng->HasPrefix(&hm->uri, &(mng->m_saveSecsPrefix))) {
                    //Save secret information request.
                    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
                    struct Session s;
                    if(!(mng->login)->GetSession(hm, s)) {
                        mg_printf_http_chunk(nc, "{\"result\":-1}");
                    } else {
                        if((mng->acntMng)->SaveAccount(hm, s.flag)) {
                            mg_printf_http_chunk(nc, "{\"result\":1}");
                        } else {
                            mg_printf_http_chunk(nc, "{\"result\":0}");
                        }
                    }
                    mg_send_http_chunk(nc, "", 0);
                } else if(mng->HasPrefix(&hm->uri, &(mng->m_delSecsPrefix))) {
                    //Delete secret information request.
                    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
                    struct Session s;
                    if(!(mng->login)->GetSession(hm, s)) {
                        mg_printf_http_chunk(nc, "{\"result\":-1}");
                    } else {
                        if((mng->acntMng)->DelAccount(hm, s.flag)) {
                            std::cout << "Delete Success" << std::endl;
                            mg_printf_http_chunk(nc, "{\"result\":1}");
                        } else {
                            mg_printf_http_chunk(nc, "{\"result\":0}");
                        }
                    }
                    mg_send_http_chunk(nc, "", 0);
                } else {
                    mg_serve_http(nc, hm, mng->m_httpServerOpts);
                }
            }
            break;
            case MG_EV_SSI_CALL:
                 mng->HandleSSICall(nc, (const char *)evData);
            break;
            case MG_EV_TIMER: {
                //Perform session maintenance.
                (mng->login)->CheckSession();
                mg_set_timer(nc, mg_time() + (mng->login)->GetSessionChkIntv());
            break;
            }
            default:
            break;
        }
    }

    /**
     * Handle SSI call
     */
    void SecMng::HandleSSICall(struct mg_connection *nc, const char *param) {
        if(strcmp(param, "setting1") == 0)
            mg_printf_html_escape(nc, "%s", "value1");
        else if(strcmp(param, "setting2") == 0)
            mg_printf_html_escape(nc, "%s", "value2");
    }

    /**
     * Push data to all websocket connections
     */
    void SecMng::BroadcastData(struct mg_mgr *mgr) {
        struct mg_connection *nc;
        int a = 0;

        for(nc = mg_next(mgr, NULL); nc != NULL; nc = mg_next(mgr, nc)) {
            if(nc->flags & MG_F_IS_WEBSOCKET) {
                mg_printf_websocket_frame(nc, WEBSOCKET_OP_TEXT, "%d", a);
            }
        }
    }

    /**
     * Set instance of class object
     */
    void SecMng::SetInstance(SecMng *inst) {
        instance = (void *)inst;
    }

    /**
     * To find whether there is a prefix in uri
     */
    bool SecMng::HasPrefix(const struct mg_str *uri, 
            const struct mg_str *prefix) {
        if(uri->len >= prefix->len && memcmp(uri->p, prefix->p, prefix->len) == 0)
            return true;
        return false;
    }

    /**
     * Extracts parameters from specific strings.
     */
    std::string SecMng::ExtractParam(const std::string &str,
            const std::string &model) {
        ptMatch->CaculateFail(model);         
        int index1 = ptMatch->Match(str, model);
        if(index1 <= 0)
            return "";
        ptMatch->CaculateFail("&");
        int index2 = ptMatch->Match(str.substr(index1, str.size() - index1), "&");

        if(index2 <= 0)
            return "";
        else
            return str.substr(index1, index2 - 1);
    }
}

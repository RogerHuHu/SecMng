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
#include "SecMng.hpp"

namespace secmng {
    void *instance;
    //Ctor
    SecMng::SecMng(const std::string &httpPort, const std::string &webRootDir) {
        m_httpPort = httpPort;
        m_webRootDir = webRootDir;

        loginPrefix = MG_MK_STR("/SecMng/login");

        login = new Login("username=", "password=");
    }

    //Dtor
    SecMng::~SecMng() {
        mg_mgr_free(&m_mgr);

        delete login;
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
                if(mng->HasPrefix(&hm->uri, &(mng->loginPrefix))) {
                    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
                    if((mng->login)->HandleLogin(hm))
                        mg_printf_http_chunk(nc, "{\"result\": %d}", 1);
                    else
                        mg_printf_http_chunk(nc, "{\"result\": %d}", 0);
                    mg_send_http_chunk(nc, "", 0);
                }
                    
                if(mg_vcmp(&hm->uri, "SecMng/login") == 0) {
                } else if(mg_vcmp(&hm->uri, "/get_cpu_usage") == 0) {
                }
                else {
                    mg_serve_http(nc, hm, mng->m_httpServerOpts);
                }
                }
                break;
            case MG_EV_SSI_CALL:
                mng->HandleSSICall(nc, (const char *)evData);
                break;
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
}

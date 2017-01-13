/**
 * \file Login.cpp
 *
 * \brief Implements the functions for login secret management system.
 *
 * \author Roger(neverchangehuhu@gmail.com)
 *
 * \date 2017-01-05
 */

#include <iostream>
#include "Login.hpp"
#include "frozen.h"

namespace secmng {
    //Ctor
    Login::Login(const std::string &usernameFlag, 
            const std::string &passwordFlag, int numSessions,
            double sessionTTL) : m_usernameFlag(usernameFlag),
        m_passwordFlag(passwordFlag), m_numSessions(numSessions),
        m_sessionTTL(sessionTTL) {
            m_sessionCookieName = "SecMngCookie";
            m_sessionChkIntv = m_sessionTTL / 5.0;

        ptMatch = new PatternMatch();
        db = new Database("../db/secmng.db");
    }

    //Dtor
    Login::~Login() {
        delete ptMatch;
        delete db;
    }

    /**
     * Handle login requests.
     */
    bool Login::HandleLogin(const struct http_message *hm, struct Session &s) {
        struct UserInfo usrInfo;
        if(CheckPassword(hm, &usrInfo)) {
            s = CreateSession(hm, usrInfo.username, usrInfo.flag);
            return true;
        }
        return false;
    }

    /**
     * Extract username and password from request HTTP message.
     */
    bool Login::ExtractUserInfo(const struct http_message *hm, std::string &username,
            std::string &password) {
        std::string httpMsg(hm->body.p);

        char *_username, *_password;
        std::string fmt = "{" + m_usernameFlag + ":%Q," +
            m_passwordFlag + ":%Q}";
        json_scanf(httpMsg.c_str(), httpMsg.size(), fmt.c_str(),
                &_username, &_password);

        username = _username; 
        password = _password;

        free(_username);
        free(_password);

        return true;
    }

    /**
     * Check password.
     */
    bool Login::CheckPassword(const struct http_message *hm, 
            struct UserInfo *usrInfo) {
        std::string username, password;
        if(ExtractUserInfo(hm, username, password)) {
            if(db->SqliteOpen()) {
                usrInfo->username = username;
                if(db->GetUserInfo(usrInfo)) {
                    if(usrInfo->password == password) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    /**
     * Creates a new session for user.
     */
    struct Session Login::CreateSession(const struct http_message *hm,
            const std::string &username, int flag) {
        //Find first available slot or use the oldest one.
        struct Session newS;
        std::list<struct Session>::iterator oldestIter;
        if(sessions.size() > m_numSessions) {
            struct Session oldestS = sessions.front();
            for(std::list<struct Session>::iterator iter = sessions.begin();
                    iter != sessions.end(); ++iter) {
                if(iter->lastUsed < oldestS.lastUsed) {
                    oldestS = *iter;
                    oldestIter = iter;
                }
            }
            DestroySession(oldestIter);
        }
        //Initialize new session.
        newS.created = mg_time();
        newS.username = username;
        newS.luckyNumber = rand();
        newS.flag = flag;
        //Create an ID by putting various volatiles into a pot and stirring.
        cs_sha1_ctx ctx;
        cs_sha1_init(&ctx);
        cs_sha1_update(&ctx, (const unsigned char *)hm->message.p,
                hm->message.len);
        cs_sha1_update(&ctx, (const unsigned char *)&newS, sizeof(newS));
        unsigned char digest[20];
        cs_sha1_final(digest, &ctx);
        newS.id = *((uint64_t *)digest);
        sessions.push_back(newS);
        return newS;
    }

    /**
     * Destroy the session state.
     */
    void Login::DestroySession(std::list<struct Session>::iterator &iter) {
        if(sessions.size() > 0) {
            iter = sessions.erase(iter);
        }
    }

    /**
     * Cleans up sessions that have been idle for too long.
     */
    void Login::CheckSession() {
        double threshold = mg_time() - m_sessionTTL;
        for(std::list<struct Session>::iterator iter = sessions.begin();
                iter != sessions.end();) {
            if(iter->id != 0 && iter->lastUsed < threshold) {
                std::cout << "Session " << iter->id << " " << iter->username << 
                    " closed due to idleness." << std::endl;
                DestroySession(iter);
            } else {
                ++iter;
            }
        }
    }

    /**
     * Parses the session cookie and returns the session information
     */
    bool Login::GetSession(struct http_message *hm, struct Session &s) {
        struct mg_str *cookieHeader = mg_get_http_header(hm, "Cookie");
        if(cookieHeader == NULL) return false;
        char ssid[21];
        if(!mg_http_parse_header(cookieHeader, m_sessionCookieName,
                    ssid, sizeof(ssid))) {
            return false;
        }

        uint64_t sid = strtoull(ssid, NULL, 16);
        for(std::list<struct Session>::iterator iter = sessions.begin();
                iter != sessions.end(); ++iter) {
            if(iter->id == sid) {
                iter->lastUsed = mg_time();
                std::cout << "update lastUsed" << std::endl;
                s = *iter;
                return true;
            }
        }
        return false;
    }
}

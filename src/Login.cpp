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

namespace secmng {
    //Ctor
    Login::Login(const std::string &usernameFlag, 
            const std::string &passwordFlag, int numSessions,
            double sessionTTL) : m_usernameFlag(usernameFlag),
        m_passwordFlag(passwordFlag), m_numSessions(numSessions),
        m_sessionTTL(sessionTTL) {
            m_sessionCookieName = "SecMngCookie"

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
    bool Login::HandleLogin(const struct http_message *hm) {
        std::string username;
        if(CheckPassword(hm, username)) {
            struct Session s = CreateSession(hm, username);
            //char sheas[100];
            //snprintf(shead, sizeof(shead),
            //        "Set-Cookie: %s=%" INT64_X_FMT "; path=/",
            //        m_sessionCookieName.c_str(), s.id):
            return true;
        }
        return false;
    }

    /**
     * Extract username and password from request HTTP message.
     */
    bool Login::ExtractUserInfo(const struct http_message *hm, std::string &username,
            std::string &password) {
        int idx1, idx2;
        std::string httpMsg((hm->uri).p);
        ptMatch->CaculateFail(m_usernameFlag);
        if((idx1 = ptMatch->Match(httpMsg, m_usernameFlag)) < 0)
            return false;
        ptMatch->CaculateFail(m_passwordFlag);
        if((idx2 = ptMatch->Match(httpMsg, m_passwordFlag)) < 0)
            return false;
        username = httpMsg.substr(idx1, idx2 - idx1 - 1 - m_passwordFlag.size()); 

        std::string spStr = "HTTP/1.1";
        ptMatch->CaculateFail(spStr); 
        if((idx1 = ptMatch->Match(httpMsg, spStr)) < 0)
            return false;
        password = httpMsg.substr(idx2, idx1 - idx2 - 1 - spStr.size());
            
        return true;
    }

    /**
     * Check password.
     */
    bool Login::CheckPassword(const struct http_message *hm, 
            std::string &username) {
        std::string password;
        if(ExtractUserInfo(hm, username, password)) {
            struct UserInfo usrInfo;
            if(db->SqliteOpen()) {
                usrInfo.username = username;
                if(db->GetUserInfo(&usrInfo)) {
                    if(usrInfo.passwor == password) {
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
            const std::string &username) {
        //Find first available slot or use the oldest one.
        struct Session newS = NULL;
        struct Session oldestS = sessions.front();
        std::list<struct Session>::iterator oldestIter;
        if(sessions.size() > m_numSessions) {
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
        //Create an ID by putting various volatiles into a pot and stirring.
        cs_sha1_ctx ctx;
        cs_sha1_init(&ctx);
        cs_sha1_update(&ctx, (const unsigned char *)hm->messege.p,
                hm->messege.len);
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
    void DestroySession(std::list<struct Session>::iterator *iter) {
        if(sessions.size() > 0)
            sessions.erase(iter);
    }

    /**
     * Cleans up sessions that have been idle for too long.
     */
    void Login::CheckSession() {
        double threshold = mg_time() - m_sessionTTL;
        for(std::list<struct Session>::iterator iter = sessions.begin();
                iter != sessions.end(); ++iter) {
            if(iter->id != 0 && iter->lastUsed < threshold) {
                std::cout << "Session " + iter->id + " " + iter->username + 
                    " closed due to idleness." << std::endl;
                DestroySession(iter);
            }
        }
    }

    /**
     * Parses the session cookie and returns the session information
     */
    struct Session Login::GetSession(struct http_message *hm) {
        struct mg_str *cookieHeader = mg_get_http_header(hm, "cookie");
        if(cookieHeader == NULL) return NULL;
        char ssid[21];
        if(!mg_http_parse_header(cookieHeader, m_sessionCookieName,
                    ssid, sizeof(ssid)))
            return NULL;

        uint64_t sid = strtoull(ssid, NULL, 16);
        for(std::list<struct Session>::iterator iter = sessions.begin();
                iter != sessions.end(); ++iter) {
            if(iter->id == sid) {
                iter->lastUsed = mg_time();
                return *iter;
            }
        }
        return NULL;
    }
}

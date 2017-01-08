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
            const std::string &passwordFlag) {
        m_usernameFlag = usernameFlag;
        m_passwordFlag = passwordFlag;

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
        std::string username, password;
        if(ExtractAccount(hm, username, password)) {
            struct AccountInfo acntInfo;
            if(db->SqliteOpen()) {
                acntInfo.username = username;
                if(db->GetAccount(&acntInfo)) {
                    if(acntInfo.password == password)
                        return true;
                }
            }
        }
        return false;
    }

    /**
     * Extract username and password from request HTTP message.
     */
    bool Login::ExtractAccount(const struct http_message *hm, std::string &username,
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
}

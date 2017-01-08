/**
 * \file AccountMng.cpp
 *
 * \brief Implements the functions for accounts management.
 *
 * \author Roger(neverchangehuhu@gmail.com)
 *
 * \date 2017-01-08
 */

#include <iostream>
#include "AccountMng.hpp"

namespace secmng {
    //Ctor
    AccountMng::AccountMng() {
        ptMatch = new PatternMatch();
        db = new Database("../db/secmng.db");
    }

    //Dtor
    AccountMng::~AccountMng() {
        delete ptMatch;
        delete db;
    }

    /**
     * Get accounts from database.
     */
    bool AccountMng::GetAccounts(std::list<struct Account> &acnts) {
        if(db->GetAccount(acnts)) {
            return true;
        }
        return false;
    }

    /**
     * Extract username and password from request HTTP message.
     */
    bool AccountMng::ExtractAccount(const struct http_message *hm, std::string &username,
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

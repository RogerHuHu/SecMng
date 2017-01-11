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
    AccountMng::AccountMng(const std::string targetFlag,
            const std::string usernameFlag,
            const std::string passwordFlag) : 
        m_targetFlag(targetFlag), m_usernameFlag(usernameFlag),
        m_passwordFlag(passwordFlag) {
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
        if(db->SqliteOpen()) {
            if(db->GetAccount(acnts)) {
                return true;
            }
            db->SqliteClose();
        }
        return false;
    }

    /**
     * Save account to database.
     */
    bool AccountMng::SaveAccount(const struct http_message *hm) {
        struct Account acnt;
        ExtractAccount(hm, acnt);
        std::cout << acnt.target << std::endl;
        std::cout << acnt.username << std::endl;
        std::cout << acnt.password << std::endl;
        if(db->SqliteOpen()) {
            if(db->InsertAccount(acnt))
                return true;
            db->SqliteClose();
        }
        return false;
    }

    /**
     * Extract username and password from request HTTP message.
     */
    bool AccountMng::ExtractAccount(const struct http_message *hm, struct Account &acnt) {
        int idx1, idx2, idx3;
        std::string httpMsg((hm->uri).p);

        std::cout << "============SaveAccount============" << std::endl;
        std::cout << httpMsg << std::endl;

        ptMatch->CaculateFail(m_targetFlag);
        if((idx1 = ptMatch->Match(httpMsg, m_targetFlag)) < 0)
            return false;
        ptMatch->CaculateFail(m_usernameFlag);
        if((idx2 = ptMatch->Match(httpMsg, m_usernameFlag)) < 0)
            return false;
        acnt.target = httpMsg.substr(idx1, idx2 - idx1 - 1 - m_usernameFlag.size());

        ptMatch->CaculateFail(m_passwordFlag);
        if((idx3 = ptMatch->Match(httpMsg, m_passwordFlag)) < 0)
            return false;
        acnt.username = httpMsg.substr(idx2, idx3 - idx2 - 1 - m_passwordFlag.size()); 

        std::string spStr = "HTTP/1.1";
        ptMatch->CaculateFail(spStr); 
        if((idx1 = ptMatch->Match(httpMsg, spStr)) < 0)
            return false;
        acnt.password = httpMsg.substr(idx3, idx1 - idx3 - 1 - spStr.size());

        return true;
    }
}

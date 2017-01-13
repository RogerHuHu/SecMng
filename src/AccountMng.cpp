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
#include "frozen.h"

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
    bool AccountMng::GetAccounts(std::list<struct Account> &acnts, int flag) {
        bool retVal = false;
        if(db->SqliteOpen()) {
            if(db->GetAccount(acnts, flag))
                retVal = true;
            db->SqliteClose();
        }
        return retVal;
    }

    /**
     * Save account to database.
     */
    bool AccountMng::SaveAccount(const struct http_message *hm, int flag) {
        struct Account acnt;
        bool retVal = false;
        ExtractAccount(hm, acnt);
        std::cout << acnt.target << std::endl;
        std::cout << acnt.username << std::endl;
        std::cout << acnt.password << std::endl;
        if(db->SqliteOpen()) {
            if(db->InsertAccount(acnt, flag))
                retVal = true;
            db->SqliteClose();
        }
        return retVal;
    }

    /**
     * Delete account to database.
     */
    bool AccountMng::DelAccount(const struct http_message *hm, int flag) {
        struct Account acnt;
        bool retVal = false;
        ExtractAccount(hm, acnt);
        if(db->SqliteOpen()) {
            if(db->DelAccount(acnt, flag))
                retVal = true;
            db->SqliteClose();
        }
        return retVal;
    }

    /**
     * Extract username and password from request HTTP message.
     */
    bool AccountMng::ExtractAccount(const struct http_message *hm,
            struct Account &acnt) {
        std::string httpMsg((hm->body).p);
        std::cout << httpMsg << std::endl;

        char *target, *username, *password;
        std::string fmt = "{" + m_targetFlag + ":%Q," +
            m_usernameFlag + ":%Q," + m_passwordFlag + ":%Q}";
        std::cout << fmt << std::endl;
        json_scanf(httpMsg.c_str(), httpMsg.size(), fmt.c_str(),
                &target, &username, &password);
        acnt.target = target;
        acnt.username = username;
        acnt.password = password;

        free(target);
        free(username);
        free(password);

        return true;
    }

    /**
     * Extract account's id from request http message.
     */
    bool AccountMng::ExtractId(const struct http_message *hm,
            struct Account &acnt) {
        std::string httpMsg(hm->body.p);
        std::cout << httpMsg << std::endl;

        int id;
        json_scanf(httpMsg.c_str(), httpMsg.size(), "{id:%d}",
                &id);
        acnt.id = id;

        return true;
    }
}

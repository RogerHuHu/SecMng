/**
 * \file AccountMng.hpp
 *
 * \brief Defines the functions for accounts management.
 *
 * \author Roger(neverchangehuhu@gmail.com)
 *
 * \date 2017-01-08
 */

#ifndef ACCOUNT_MNG_HPP_H
#define ACCOUNT_MNG_HPP_H

#include <string>
#include <list>
#include "mongoose.h"
#include "PatternMatch.hpp"
#include "Database.hpp"

using namespace match;
using namespace database;

namespace secmng {
    /**
     * \brief This class is used to manage accounts.
     */
    class AccountMng {
    public:
        //Ctor
        AccountMng(const std::string targetFlag,
                const std::string usernameFlag, 
                const std::string passwordFlag);

        //Dtor
        ~AccountMng();

        /**
         * \brief Get accounts from database. 
         *
         * \param acnts  Account information structure list.
         *
         * \return Get accounts result true/false.
         */
        bool GetAccounts(std::list<struct Account> &acnts);

        /**
         * \brief Save account to database. 
         *
         * \param hm  Http message.
         *
         * \return Save accounts result true/false.
         */
        bool SaveAccount(const struct http_message *hm);
    private:
        /**
         * \brief Extract username and password from request HTTP message.
         *
         * \param hm    Http message.
         * \param acnt  Account information structure.
         *
         * \return Account extract result.
         */
        bool ExtractAccount(const struct http_message *hm, struct Account &acnt);

        std::string m_targetFlag;
        std::string m_usernameFlag;
        std::string m_passwordFlag;

        PatternMatch *ptMatch;
        Database *db;
    };
}

#endif //ACCOUNT_MNG_HPP_H

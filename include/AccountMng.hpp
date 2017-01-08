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
        AccountMng();

        //Dtor
        ~AccountMng();

        /**
         * \brief Get accounts from database. 
         *
         * \param .
         *
         * \return Get accounts result true/false.
         */
        bool GetAccounts(std::list<struct Account> &acnts);
    private:
        /**
         * \brief Extract username and password from request HTTP message.
         *
         * \param hm  Http message.
         * \param username  Username extract from request HTTP message.
         * \param password  Password extract from request HTTP message.
         *
         * \return Account extract result.
         */
        bool ExtractAccount(const struct http_message *hm, std::string &username,
                std::string &password);

        std::string m_usernameFlag;
        std::string m_passwordFlag;

        PatternMatch *ptMatch;
        Database *db;
    };
}

#endif //ACCOUNT_MNG_HPP_H

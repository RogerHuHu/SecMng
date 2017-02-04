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
        AccountMng(const std::string &typeFlag,const std::string &targetFlag,
                const std::string &usernameFlag, 
                const std::string &passwordFlag);

        //Dtor
        ~AccountMng();

        /**
         * \brief Get accounts from database. 
         *
         * \param acnts  Account information structure list.
         * \param recordsTotal     Total record number in database.
         * \param recordsFiltered  The record number meets the requirement.
         * \param cond             Database select conditions.
         * \param flag   User flag.
         *
         * \return Get accounts result true/false.
         */
        bool GetAccounts(std::list<struct Account> &acnts, 
                int &recordsTotal, int &recordsFiltered, struct Condition cond,
                int flag);

        /**
         * \brief Save account to database. 
         *
         * \param hm    Http message.
         * \param flag  User flag.
         *
         * \return Save accounts result true/false.
         */
        bool SaveAccount(const struct http_message *hm, int flag);

        /**
         * \brief Delete account to database. 
         *
         * \param hm    Http message.
         * \param flag  User flag.
         *
         * \return Delete accounts result true/false.
         */
        bool DelAccount(const struct http_message *hm, int flag);
    private:
        /**
         * \brief Extract username and password from request HTTP message.
         *
         * \param hm    Http message.
         * \param acnt  Account information structure.
         * \param type  Operate type insert or update.
         *
         * \return Account extract result.
         */
        bool ExtractAccount(const struct http_message *hm, struct Account &acnt,
                int &type);

        /**
         * \brief Extract account's id from request http message.
         *
         * \param hm    Http message. 
         * \param acnt  Account information structure.
         */
        bool ExtractId(const struct http_message *hm, struct Account &acnt);

        std::string m_typeFlag;
        std::string m_targetFlag;
        std::string m_usernameFlag;
        std::string m_passwordFlag;

        PatternMatch *ptMatch;
        Database *db;
    };
}

#endif //ACCOUNT_MNG_HPP_H

/**
 * \file Database.hpp
 *
 * \brief Defines the functions for database operation.
 *
 * \author Roger(neverchangehuhu@gmail.com)
 *
 * \date 2017-01-07
 */

#ifndef DATABASE_HPP_H
#define DATABASE_HPP_H

#include <string>
#include <sqlite3.h>
#include <list>
#include "AesEncryptor.hpp"
#include "AESResource.hpp"

using namespace secmng;

namespace database {
    /**
     * \brief User information struct.
     */
    struct UserInfo {
        std::string username;
        std::string password;
        int priority;
        int flag;
    };

    /**
     * \brief Account information struct.
     */
    struct Account {
        int id;
        std::string target;
        std::string username;
        std::string password;
    };

    /**
     * \brief This class is used to operate database.
     */
    class Database {
    public:
        //Ctor
        Database() {}
        Database(const std::string &dbPath);

        //Dtor
        ~Database();

        /**
         * \brief Open sqlite database.
         *
         * \return Open result true/false.
         */
        bool SqliteOpen();

        /**
         * \brief Close sqlite database.
         */
        void SqliteClose();

        /**
         * \brief Create table users.
         *
         * \return Create result true/false.
         */
        bool CreateTableUsers();

        /**
         * \brief Insert user information to sqlite database.
         *
         * \param usrInfo
         *
         * \return Add result true/false.
         */
        bool InsertUserInfo(const struct UserInfo &usrInfo);

        /**
         * \brief Get user information from database.
         *
         * \param usrInfo
         *
         * \return Database select result true/false.
         */
        bool GetUserInfo(struct UserInfo *usrInfo);

        /**
         * \brief Create table accounts.
         *
         * \return Create result true/false.
         */
        bool CreateTableAccounts();

        /**
         * \brief Insert account to sqlite database.
         *
         * \param acnt  Account information structure.
         *
         * \return Insert result true/false.
         */
        bool InsertAccount(const struct Account &acnt);

        /**
         * \brief Get account from sqlite database.
         *
         * \param acnt  Account information structure.
         *
         * \return Database select result true/false.
         */
        bool GetAccount(std::list<struct Account> &acnts);

        /**
         * \brief Delete account from sqlite database.
         *
         * \param acnt  Account information structure.
         *
         * \return Database delete result true/false.
         */
        bool DelAccount(const struct Account &acnt);
    private:
        std::string m_dbPath;
        sqlite3 *m_db;

        AesEncryptor *aes;
    };
}

#endif //DATABASE_HPP_H

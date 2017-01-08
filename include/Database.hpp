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

namespace database {
    /**
     * \brief Account information struct.
     */
    struct AccountInfo {
        std::string username;
        std::string password;
        int priority;
        int flag;
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
         * \brief Insert account to sqlite database.
         *
         * \param username
         * \param password
         * \param priority
         * \param flag
         *
         * \return Add result true/false.
         */
        bool InsertAccount(const struct AccountInfo acntInfo);

        /**
         * \brief Get account information from database.
         *
         * \param username
         * \param password
         *
         * \return Database select result true/false.
         */
        bool GetAccount(struct AccountInfo *acntInfo);

    private:
        std::string m_dbPath;
        sqlite3 *m_db;
    };
}

#endif //DATABASE_HPP_H

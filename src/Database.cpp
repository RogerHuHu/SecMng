/**
 * \file Database.cpp
 *
 * \brief Implements the functions for database operation.
 *
 * \author Roger(neverchangehuhu@gmail.com)
 *
 * \date 2017-01-07
 */

#include <iostream>
#include <sqlite3.h>
#include "Database.hpp"

namespace database {

    Database::Database(const std::string &dbPath) :
        m_dbPath(dbPath) {}

    Database::~Database() {
        SqliteClose();
    }

    /**
     * Open sqlite database.
     */
    bool Database::SqliteOpen() {
        if(sqlite3_open(m_dbPath.c_str(), &m_db) != SQLITE_OK) {
            std::cout << "Sqlite open error!" << std::endl;
            std::cout << sqlite3_errmsg(m_db) << std::endl;
            return false;
        }

        return true;
    }

    /**
     * Close sqlite database.
     */
    void Database::SqliteClose() {
        if(m_db != NULL) {
            sqlite3_close(m_db);
            m_db = NULL;
        }
    }

    /**
     * Create table users
     */
    bool Database::CreateTableUsers() {
        char *errMsg = 0;

        std::string sqlCmd = "CREATE TABLE users ("
            "Id INTEGER PRIMARY KEY AUTOINCREMENT   NOT NULL,"
            "Username   CHAR(20),"
            "Password   CHAR(40),"
            "Priority   INTEGER,"
            "Flag       INTEGER);";

        if(sqlite3_exec(m_db, sqlCmd.c_str(), NULL, NULL, &errMsg) != SQLITE_OK) {
            sqlite3_free(errMsg);
            return false;
        }
        return true;
    }

    /**
     * Insert user information to sqlite database
     */
    bool Database::InsertUserInfo(const struct UserInfo usrInfo) {
        char *errMsg = 0;
        sqlite3_stmt *stmt;

        std::string sqlCmd = "INSERT INTO users(Username,Password,"
               "Priority, Flag) VALUES(?,?,?,?);";

        if(sqlite3_prepare_v2(m_db, sqlCmd.c_str(), sqlCmd.size(), 
                    &stmt, NULL) != SQLITE_OK) {
            std::cout << sqlite3_errmsg(m_db) << std::endl;
            return false;
        }

        if(sqlite3_exec(m_db, "BEGIN;", NULL, NULL, &errMsg) != SQLITE_OK) {
            std::cout << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }

        sqlite3_bind_text(stmt, 1, usrInfo.username.c_str(), 
                usrInfo.username.size(), SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, usrInfo.password.c_str(),
                usrInfo.password.size(), SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, usrInfo.priority); 
        sqlite3_bind_int(stmt, 4, usrInfo.flag); 

        if(sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return false;
        }

        if(sqlite3_exec(m_db, "COMMIT;", NULL, NULL, &errMsg) != SQLITE_OK) {
            std::cout << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }

        return true;
    }

    /**
     * Get user information from database
     */ 
    bool Database::GetUserInfo(struct UserInfo *usrInfo) {
        sqlite3_stmt *stmt;
        std::string sqlRet;
        int cnt = 0;
        
        std::string sqlCmd = "SELECT Username,Password,Priority,Flag FROM users "
            "WHERE Username='" + usrInfo->username + "'";
        if(sqlite3_prepare_v2(m_db, sqlCmd.c_str(), sqlCmd.size(), &stmt,
                    NULL) != SQLITE_OK) {
            if(stmt)
                sqlite3_finalize(stmt);
            return false;
        }

        while(sqlite3_step(stmt) == SQLITE_ROW) {
            cnt++;
            sqlRet = (const char *)sqlite3_column_text(stmt, 0);
            usrInfo->username = sqlRet != "" ? sqlRet : "NULL";
            sqlRet = (const char *)sqlite3_column_text(stmt, 1);
            usrInfo->password = sqlRet != "" ? sqlRet : "NULL";
            usrInfo->priority = sqlite3_column_int(stmt, 2);
            usrInfo->flag = sqlite3_column_int(stmt, 2);
        }
        
        if(stmt)
            sqlite3_finalize(stmt);
        
        return cnt > 0 ? true : false;
    }

    /**
     * Create table accounts
     */
    bool Database::CreateTableAccounts() {
        char *errMsg = 0;

        std::string sqlCmd = "CREATE TABLE accounts ("
            "Id INTEGER PRIMARY KEY AUTOINCREMENT   NOT NULL,"
            "Flag       INTEGER,"
            "Target     CHAR(50),"
            "Username   CHAR(20),"
            "Password   CHAR(40));";

        if(sqlite3_exec(m_db, sqlCmd.c_str(), NULL, NULL, &errMsg) != SQLITE_OK) {
            std::cout << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }
        return true;
    }

    /**
     * Insert account to sqlite database.
     */
    bool Database::InsertAccount(const struct Account acnt) {
        char *errMsg = 0;
        sqlite3_stmt *stmt;

        std::string sqlCmd = "INSERT INTO accounts(Target,Username,Password"
               ") VALUES(?,?,?);";

        if(sqlite3_prepare_v2(m_db, sqlCmd.c_str(), sqlCmd.size(), 
                    &stmt, NULL) != SQLITE_OK) {
            std::cout << sqlite3_errmsg(m_db) << std::endl;
            return false;
        }

        if(sqlite3_exec(m_db, "BEGIN;", NULL, NULL, &errMsg) != SQLITE_OK) {
            std::cout << "BEGIN" << std::endl;
            std::cout << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }

        sqlite3_bind_text(stmt, 1, acnt.target.c_str(), 
                acnt.target.size(), SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, acnt.username.c_str(),
                acnt.username.size(), SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, acnt.password.c_str(),
                acnt.password.size(), SQLITE_STATIC);

        if(sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return false;
        }

        if(sqlite3_exec(m_db, "COMMIT;", NULL, NULL, &errMsg) != SQLITE_OK) {
            std::cout << "COMMIT" << std::endl;
            std::cout << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }

        return true;
    }

    /**
     * Get account from sqlite database.
     */
    bool Database::GetAccount(std::list<struct Account> &acnts) {
        sqlite3_stmt *stmt;
        std::string sqlRet;
        
        std::string sqlCmd = "SELECT Target,Username,Password FROM accounts;";
        if(sqlite3_prepare_v2(m_db, sqlCmd.c_str(), sqlCmd.size(), &stmt,
                    NULL) != SQLITE_OK) {
            if(stmt)
                sqlite3_finalize(stmt);
            return false;
        }

        while(sqlite3_step(stmt) == SQLITE_ROW) {
            struct Account acnt;
            sqlRet = (const char *)sqlite3_column_text(stmt, 0);
            acnt.target = sqlRet != "" ? sqlRet : "NULL";
            sqlRet = (const char *)sqlite3_column_text(stmt, 1);
            acnt.username = sqlRet != "" ? sqlRet : "NULL";
            sqlRet = (const char *)sqlite3_column_text(stmt, 2);
            acnt.password = sqlRet != "" ? sqlRet : "NULL";
            acnts.push_back(acnt);
        }
        
        if(stmt)
            sqlite3_finalize(stmt);
        /*
        for(int i = 0; i < 2; i++) {
            struct Account acnt;
            acnt.target = "Test1";
            acnt.username = "roger";
            acnt.password = "123";
            acnts.push_back(acnt);
        }*/
        
        return acnts.size() > 0 ? true : false;
    }
}

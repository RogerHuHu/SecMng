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
        m_dbPath(dbPath) {
        aes = new AesEncryptor(key);
    }

    Database::~Database() {
        SqliteClose();
        delete aes;
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
    bool Database::InsertUserInfo(const struct UserInfo &usrInfo) {
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
        std::string encPassword = aes->EncryptString(usrInfo.password);
        sqlite3_bind_text(stmt, 2, encPassword.c_str(),
                encPassword.size(), SQLITE_STATIC);
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
            usrInfo->password = aes->DecryptString(usrInfo->password);
            usrInfo->priority = sqlite3_column_int(stmt, 2);
            usrInfo->flag = sqlite3_column_int(stmt, 3);
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
    bool Database::InsertAccount(const struct Account &acnt, int flag) {
        char *errMsg = 0;
        sqlite3_stmt *stmt;

        std::string sqlCmd = "INSERT INTO accounts(Flag,Target,Username,Password"
               ") VALUES(?,?,?,?)"; 

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

        sqlite3_bind_int(stmt, 1, flag);
        sqlite3_bind_text(stmt, 2, acnt.target.c_str(), 
                acnt.target.size(), SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, acnt.username.c_str(),
                acnt.username.size(), SQLITE_STATIC);
        std::string encPassword = aes->EncryptString(acnt.password);
        sqlite3_bind_text(stmt, 4, encPassword.c_str(),
                encPassword.size(), SQLITE_STATIC);

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
     * Update account to sqlite database.
     */
    bool Database::UpdateAccount(const struct Account &acnt, int flag) {
        char *errMsg = 0;
        sqlite3_stmt *stmt;

        //std::string sqlCmd = "INSERT INTO accounts(Flag,Target,Username,Password"
        //       ") VALUES(?,?,?,?)"; 
        std::string sqlCmd = "UPDATE accounts SET Password=? WHERE Target='" +
            acnt.target + "' AND Username='" + acnt.username + "';";

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

        std::string encPassword = aes->EncryptString(acnt.password);
        sqlite3_bind_text(stmt, 1, encPassword.c_str(),
                encPassword.size(), SQLITE_STATIC);

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
    bool Database::GetAccount(std::list<struct Account> &acnts,
            int &recordsTotal, int &recordsFiltered, struct Condition cond,
            int flag) {
        recordsTotal = GetRecordNum("", flag);
        recordsFiltered = GetRecordNum(cond.searchVal, flag);
        sqlite3_stmt *stmt = NULL;
        std::string sqlRet;
        
        std::string orderSqlCmd;
        switch(cond.orderClm) {
            case 0 : 
                orderSqlCmd = " ORDER BY Target " + cond.orderDir;
                break;
            case 1 :
                orderSqlCmd = " ORDER BY Username " + cond.orderDir;
                break;
            case 2 : 
                orderSqlCmd = " ORDER BY Password " + cond.orderDir;
            default :
                orderSqlCmd = "";
                break;
        }

        std::string searchSqlCmd;
        if(cond.searchVal == "") {
            searchSqlCmd = "";
        } else {
            searchSqlCmd = " AND Target LIKE '%" + cond.searchVal + "%' ";
        }

        std::string limitSqlCmd = " LIMIT " + cond.length + " OFFSET " + cond.startIdx;

        std::string sqlCmd = "SELECT Id,Target,Username,Password "
            "FROM accounts WHERE Flag=" + std::to_string(flag) + searchSqlCmd +
            orderSqlCmd + limitSqlCmd + ";";
        if(sqlite3_prepare_v2(m_db, sqlCmd.c_str(), sqlCmd.size(), &stmt,
                    NULL) != SQLITE_OK) {
            if(stmt)
                sqlite3_finalize(stmt);
            return false;
        }

        while(sqlite3_step(stmt) == SQLITE_ROW) {
            struct Account acnt;
            acnt.id = sqlite3_column_int(stmt, 0);
            sqlRet = (const char *)sqlite3_column_text(stmt, 1);
            acnt.target = sqlRet != "" ? sqlRet : "NULL";
            sqlRet = (const char *)sqlite3_column_text(stmt, 2);
            acnt.username = sqlRet != "" ? sqlRet : "NULL";
            sqlRet = (const char *)sqlite3_column_text(stmt, 3);
            acnt.password = sqlRet != "" ? sqlRet : "";
            acnt.password = aes->DecryptString(acnt.password);
            acnts.push_back(acnt);
        }
        
        if(stmt)
            sqlite3_finalize(stmt);
        
        return acnts.size() > 0 ? true : false;
    }

    /**
     * Delete account from sqlite database.
     */
    bool Database::DelAccount(const struct Account &acnt, int flag) {
        sqlite3_stmt *stmt = NULL;

        std::string sqlCmd = "DELETE FROM accounts WHERE Target=? AND Username=? "
            "AND Flag=?;";
        if(sqlite3_prepare_v2(m_db, sqlCmd.c_str(), sqlCmd.size(), &stmt,
                    NULL) != SQLITE_OK) {
            if(stmt)
                sqlite3_finalize(stmt);
            return false;
        }
       
        sqlite3_bind_text(stmt, 1, acnt.target.c_str(), acnt.target.size(),
                SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, acnt.username.c_str(), acnt.username.size(),
                SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, flag);
        int ret = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if(ret == SQLITE_DONE || ret == SQLITE_ROW)
            return true;
        return false;
    }

    /**
     * Get the record's number in database.
     */
    int Database::GetRecordNum(const std::string &searchVal, int flag) {
        sqlite3_stmt *stmt = NULL;
        std::string sqlCmd;

        if(searchVal == "") {
            sqlCmd = "SELECT COUNT(*) FROM accounts WHERE Flag=" +
                std::to_string(flag) + ";";
        } else {
            std::string rule = "'%" + searchVal + "%'";
            sqlCmd = "SELECT COUNT(*) FROM accounts WHERE Target LIKE " +
                rule + " AND Flag=" + std::to_string(flag) + ";";
        }

        if(sqlite3_prepare_v2(m_db, sqlCmd.c_str(), sqlCmd.size(), &stmt,
                    NULL) != SQLITE_OK) {
            if(stmt)
                sqlite3_finalize(stmt);
            return -1;
        }

        int recordNum = -1;
        while(sqlite3_step(stmt) == SQLITE_ROW)
           recordNum = sqlite3_column_int(stmt, 0); 
        sqlite3_finalize(stmt);

        return recordNum;
    }
}

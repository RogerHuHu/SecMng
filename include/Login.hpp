/**
 * \file Login.hpp
 *
 * \brief Defines the functions for login secret management system.
 *
 * \author Roger(neverchangehuhu@gmail.com)
 *
 * \date 2017-01-05
 */

#ifndef LOGIN_CPP_H
#define LOGIN_CPP_H

#include <string>
#include <list>
#include "mongoose.h"
#include "PatternMatch.hpp"
#include "Database.hpp"

using namespace match;
using namespace database;

namespace secmng {
    /**
     * \brief Session information structure.
     */
    struct Session {
        uint64_t id;              //Session ID. Must be unique and hard to guess.
        double created;           //Time when the session was created and time of last
                                  //activity. Used to clean up stale sessions.
        double lastUsed;          //Time when the session was last active.
        std::string username;     //Username this session is associated with.
        std::string luckyNumber;  //Some state associated with user's session.
        int flag;                 //User flag;
    };

    /**
     * \brief This class is used to handle login requests.
     */
    class Login {
    public:
        //Ctor
        Login(const std::string &usernameFlag, const std::string &passwordFlag,
                int numSessions, double sessionTTL);

        //Dtor
        ~Login();

        /**
         * \brief Handle login requests. 
         *
         * \param hm  Http message.
         *
         * \return Login result true/false.
         */
        bool HandleLogin(const struct http_message *hm, struct Session &s);

        /**
         * \brief Cleans up sessions that have been idle for too long.
         */
        void CheckSession();

        /**
         * \brief Parses the session cookie and returns the session information
         *
         * \param hm  Http message.
         * \param s
         *
         * \return Get session result true/false.
         */
        bool GetSession(struct http_message *hm, struct Session &s);

        /**
         * \brief Get the cookie name.
         *
         * \return Name of session cookie.
         */
        std::string GetCookieName() const {return m_sessionCookieName;}

        /**
         * \brief Get the session check interval.
         *
         * \return Session check interval.
         */
        double GetSessionChkIntv() const {return m_sessionChkIntv;}
        
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
        bool ExtractUserInfo(const struct http_message *hm, std::string &username,
                std::string &password);

        /**
         * \brief Check password.
         *
         * \param hm        Http message.
         * \param username
         *
         * \return Check result true/false.
         */
        bool CheckPassword(const struct http_message *hm,
                struct UserInfo *usrInfo);

        /**
         * \brief Creates a new session for user. 
         *
         * \param hm        Http message.
         * \param username  
         * \param flag      User flag.
         *
         * \reutrn The session structure created.
         */
        struct Session CreateSession(const struct http_message *hm,
                const std::string &username, int flag);

        /**
         * \brief Destroys the session state.
         *
         * \param iter
         */
        void DestroySession(std::list<struct Session>::iterator &iter);


        std::string m_usernameFlag;
        std::string m_passwordFlag;
        int m_numSessions;
        double m_sessionTTL;
        double m_sessionChkIntv;          //Session check interval.
        const char *m_sessionCookieName;

        std::list<struct Session> sessions;
        
        PatternMatch *ptMatch;
        Database *db;
    };
}

#endif //LOGIN_CPP_H

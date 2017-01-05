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
#include "mongoose.h"
#include "PatternMatch.hpp"

using namespace match;

namespace secmng {
    /**
     * \brief This class is used to handle login requests.
     */
    class Login {
    public:
        //Ctor
        Login(const std::string &usernameFlag, const std::string &passwordFlag);

        //Dtor
        ~Login();

        /**
         * \brief Handle login requests. 
         *
         * \param hm  Http message.
         *
         * \return Login result true/false.
         */
        bool HandleLogin(const struct http_message *hm);
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
    };
}

#endif //LOGIN_CPP_H

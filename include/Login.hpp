/**
 * \file Login.hpp
 *
 * \brief Defines the functions for login secret management system.
 *
 * \author Roger(neverchangehuhu@gmail.com)
 *
 * \date 2017-01-03 
 */

#ifndef LOGIN_CPP_H
#define LOGIN_CPP_H

#include <string>
#include "mongoose.h"

using std::string;

namespace secmng {
    /**
     * \brief This class is used to handle login requests.
     */
    class Login {
    public:
        //Ctor
        Login() {}

        //Dtor
        ~Login() {}

        /*
         * \brief Handle login requests. 
         *
         * \param nc            Mongoose connection
         * \param mh            Http message
         * \param userNameBody  Username lable name in body of html file
         * \param passwdBody    Password lable name in body of html file
         */
        void HandleLogin(struct mg_connection *nc, struct http_message *hm,
                const std::string &userNameBody, const std::string &passwdBody);
    };
}

#endif //LOGIN_CPP_H

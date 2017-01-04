/**
 * \file Login.cpp
 *
 * \brief Implements the functions for login secret management system.
 *
 * \author Roger(neverchangehuhu@gmail.com)
 *
 * \date 2017-01-03 
 */

#include <iostream>
#include "Login.hpp"

using std::cout;
using std::endl;

namespace secmng {
    void Login::HandleLogin(struct mg_connection *nc, struct http_message *hm,
            const std::string &userNameBody, const std::string &passwdBody) {
        char buf[100];
        //Get username and password form variables
        mg_get_http_var(&hm->body, userNameBody.c_str(), buf, sizeof(buf));
        std::cout << "User Name: " << buf << std::endl;
        mg_get_http_var(&hm->body, passwdBody.c_str(), buf, sizeof(buf));
        std::cout << "Password: " << buf << std::endl; 
        
        //Send response
        mg_printf(nc, "%s", "HTTP/1.1 302 OK\r\nLocation: /\r\n\r\n");
    }
}

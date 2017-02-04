/**
 * \file SecMng.hpp
 *
 * \brief Defines the functions for secret management system.
 *
 * \author Roger(neverchangehuhu@gmail.com)
 *
 * \date 2017-01-05
 */

#ifndef SEC_MNG_HPP
#define SEC_MNG_HPP

#include <string>
#include "mongoose.h"
#include "Login.hpp"
#include "AccountMng.hpp"
#include "PatternMatch.hpp"

using std::string;

namespace secmng {
    extern void *obj;

    /**
     * \brief This class is used to handle login requests.
     */
    class SecMng {
    public:
        //Ctor
        SecMng() {}
        SecMng(const std::string &httpPort, const std::string &webRootDir);

        //Dtor
        ~SecMng();

        /**
         * \brief Init SecMng HTTP server
         */
        void Init();

        /**
         * \brief Run SecMng HTTP server
         *
         * \param interval  Maximum number of milliseconds to sleep.
         */
        void Run(int interval);

        /**
         * \brief Set instance of class object
         *
         * \param inst  Inst of class object
         */
        void SetInstance(SecMng *inst);
    private:
        /** \brief Handle HTTP events
         *
         * \param nc      Mongoose connection
         * \param ev      Event type
         * \param evData  Event data
         */
        static void EvHandler(struct mg_connection *nc, int ev, void *evData);
        
        /**
         * \brief Handle SSI call
         * 
         * \param nc     mongoose connection
         * \param param  
         */
        void HandleSSICall(struct mg_connection *nc, const char *param);

        /**
         * \brief Push data to all websocket connections
         *
         * \param mgr  mongoose event manager
         */
        void BroadcastData(struct mg_mgr *mgr);

        /**
         * \brief To find whether there is a prefix in uri
         *
         * \param uri     HTTP request message
         * \param prefix  
         *
         * \return true/false
         */
        bool HasPrefix(const struct mg_str *uri, const struct mg_str *prefix);

        /**
         * \brief Extracts parameters from specific strings.
         * 
         * \param str    Input string.
         * \param model  Model string.
         */
        std::string ExtractParam(const std::string &str,
                const std::string &model);

        std::string m_httpPort;
        std::string m_webRootDir;
        struct mg_mgr m_mgr;
        struct mg_connection *m_nc;
        struct mg_serve_http_opts m_httpServerOpts;
        time_t m_lastTime;

        struct mg_str m_loginPrefix;
        struct mg_str m_getSecsPrefix;
        struct mg_str m_saveSecsPrefix;
        struct mg_str m_delSecsPrefix;

        Login *login;
        AccountMng *acntMng;
        PatternMatch *ptMatch;
    };
}

#endif //SEC_MNG_HPP

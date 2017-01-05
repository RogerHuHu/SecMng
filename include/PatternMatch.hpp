/**
 * \file PatternMatch.hpp
 *
 * \brief Defines the functions for find strings.
 *
 * \author Roger(neverchangehuhu@gmail.com)
 *
 * \date 2017-01-05 
 */

#ifndef PATTERN_MATCH_CPP_H
#define PATTERN_MATCH__CPP_H

#include <string>

namespace match {
    /**
     * \brief This class is used to find strings and it's position.
     */
    class PatternMatch {
    public:
        //Ctor
        PatternMatch() {}

        //Dtor
        ~PatternMatch() {}

        /**
         * \brief Find whether the specific string is in the source string, 
         *        and it's position.
         *
         * \param sStr   Source string.
         * \param spStr  Specific string.
         *
         * \return The position of spStr.
         */
        int Match(const std::string &sStr, const std::string &spStr);

        /**
         * \brief Caculate fail function.
         *
         * \param spStr  Specific string.
         */
        void CaculateFail(const std::string &spStr);
    private:

        int failure[20];
    };
}

#endif //PATTERN_MATCH_CPP_H

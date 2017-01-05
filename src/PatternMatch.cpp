/**
 * \file PatternMatch.cpp
 *
 * \brief Implements the functions for find strings.
 *
 * \author Roger(neverchangehuhu@gmail.com)
 *
 * \date 2017-01-05
 */

#include <iostream>
#include "PatternMatch.hpp"

namespace match {
    /**
     * This class is used to find strings and it's position.
     */
    int PatternMatch::Match(const std::string &sStr, const std::string &spStr) {
        size_t i = 0, j = 0;
        
        while(i < sStr.size() && j < spStr.size()) {
            if(sStr.at(i) == spStr.at(j)) {
                ++i; ++j;
            } else if(j == 0) {
                ++i;
            } else {
                j = failure[j - 1] + 1;
            }
        }

        return (j == spStr.size()) ? i : -1;  
    }

    /**
     * Caculate fail function.
     */
    void PatternMatch::CaculateFail(const std::string &spStr) {
        int i;
        failure[0] = -1;
        for(size_t j = 1; j < spStr.size() - 1; ++j) {
            i = failure[j - 1];
            while((spStr.at(j) != spStr.at(j + 1)) && (i >= 0))
                i = failure[i];
            if(spStr.at(j) == spStr.at(i + 1))
                failure[j] = i + 1;
            else
                failure[j] = -1;
        }
    }
}

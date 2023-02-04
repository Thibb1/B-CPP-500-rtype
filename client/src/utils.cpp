/*
** EPITECH PROJECT, 2023
** rtype
** File description:
** utils.cpp by thibb1
*/

#include "utils.hpp"

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

bool isMatch(const std::string &str, const std::string &pattern) {
    std::regex reg(pattern);
    return std::regex_match(str, reg);
}

bool isNumber(const std::string &str) { return isMatch(str, "^[0-9]+$"); }

std::string getMatch(const std::string &str, const std::string &pattern, int index) {
    std::regex reg(pattern);
    std::smatch match;
    if (std::regex_search(str, match, reg)) {
        return match.str(index);
    }
    return "";
}

std::vector<std::string> getMatches(const std::string &str, const std::string &pattern) {
    std::regex reg(pattern);
    std::smatch match;
    std::vector<std::string> matches;
    if (std::regex_search(str, match, reg)) {
        for (auto &m : match) {
            matches.push_back(m.str());
        }
    }
    return matches;
}

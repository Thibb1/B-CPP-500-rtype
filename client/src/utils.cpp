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

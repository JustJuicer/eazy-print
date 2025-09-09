//
// Created by jujin on 25-9-9.
//
#include "sprint.hpp"
#include <print>
int main() {
    {
        std::pair p{1, 3.4};
        std::println("[std]: pair: {}", p);
    }
    {
        std::tuple tp {std::tuple{2, 3.5}, 2.3, std::pair{23, 4}};
        std::println("[std]: tuple: {}", tp);
    }
}
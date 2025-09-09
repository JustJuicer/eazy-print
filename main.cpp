//
// Created by jujin on 25-9-9.
//
#include "sprint.hpp"
#include <print>
int main() {
    {
        std::pair p{1, 3.4};
        std::println("[std] pair: {}", p);
    }
    {
        std::tuple tp {std::tuple{2, 3.5}, 2.3, std::pair{23, 4}};
        std::println("[std] tuple: {}", tp);
    }
    {
        std::optional<int> opt1;
        std::optional<int> opt2{3};
        std::println("[std] optional none: {}", opt1);
        std::println("[std] optional value: {}", opt2);
    }
    {
        std::filesystem::path p{"A/B/C/D"};
        std::println("[std] path: {}", p);
    }
}
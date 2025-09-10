//
// Created by jujin on 25-9-9.
//
#include "sprint.hpp"
#include <print>
#include <deque>
#include <vector>
#include <list>
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
    {
        auto p = std::make_shared<int>(3);
        auto c = p;
        std::println("[std] shared_ptr: {}", p);
    }
    {
        auto p = std::make_unique<int>(4);
        std::println("[std] unique_ptr: {}", p);
    }
    {
        std::complex<double> c{3.4, 3};
        std::println("[std] complex: {}", c);
    }
    {
        std::println("{}", std::chrono::microseconds(3));
    }
    {
        struct Tag {
            int data;
            std::string to_string() {
                return std::to_string(data);
            }
        };
        std::println("range test");
        std::vector v{1, 2, 3, 4};
        std::deque<std::vector<int>> d{{2, 3}, {5, 6}, {7}};
        Tag tag{14};
        println(tag);
        println(v);
        println(std::pair{v, std::tuple{std::make_shared<Tag>(35), tag, d}});
    }
    {
        class Tag {
            public:
            Tag(int a): data(a) {}
            int data;
        };
        println(Tag{3});
    }
}
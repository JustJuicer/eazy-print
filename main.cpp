//
// Created by jujin on 25-9-9.
//
#include "sprint.hpp"
#include <print>
#include <deque>
#include <vector>
#include <list>
#include <map>
using namespace ju;
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
        println("range test");
        std::vector v{1, 2, 3, 4};
        std::deque<std::vector<int>> d{{2, 3}, {5, 6}, {7}};
        Tag tag{14};
        println(tag);
        println(v);
        println(std::tuple{v, std::tuple{
            std::make_shared<Tag>(35), tag, d, "hello world"},
            std::optional<std::string>{"good"}
        });
        println(std::optional<std::string>{"str in optional"});
    }
    {
        class Tag {
            public:
            Tag(int a): data(a) {}
            int data;
            operator std::string() const {
                return std::to_string(data);
            }
        };
        println(Tag{3});
    }
    { // print type name test
        auto a_type_in_func = []<typename T>(T&& obj){
            print("raw: ");
            println(_inner::get_raw_name<const std::decay_t<T>>());
            print("get: ");
            println<const T>();
        };

        a_type_in_func(3);
        a_type_in_func(34.f);
        a_type_in_func(std::tuple<int, double>{});
        a_type_in_func(std::tuple<std::pair<int, std::optional<std::chrono::seconds>>, double>{});
        std::println();
    }
    {
        struct Age {
            int age;
        };
        struct Person {
            Age age;
            std::string name;
            std::vector<std::string> games;
        };
        Person tag{18, "Jujinqian", {"cs", "mc"}};
        const Person ctag{18, "Jujinqian", {"cs", "mc"}};
        println(tag);
        println(ctag);
    }
    {
        class Person {
            int age_;
        public:
            Person(int age): age_(age) {}
            int age(){ return age_;}
        };
        Person p{3};
        const Person cp{3};
        println(p);
        println(cp);
    }
    {
        std::map<int, std::string> mp{{1, "good1"}, {3, "good"}, {4, "good3"}};
        std::multimap<int, std::string> mmp{{1, "good1"}, {2, "good"}, {2, "good3"}};
        println(mp);
        println(mmp);
        println(std::multimap<int, std::string_view>{{1, "good1"}, {2, "good"}, {2, "good3"}});
    }
    {
        int a;
        println("This is ",a,"(int)");
        ju::println();
    }
}
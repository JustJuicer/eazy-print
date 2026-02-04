//
// Created by jujin on 25-9-9.
// Comprehensive test cases for eprint library (new API)
//
#include "eprint.hpp"
#include <deque>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_set>
#include <forward_list>
#include <array>
#include <numeric>
#include <sstream>
#include <chrono>
#include <thread>


void print_section(const char* title) {
    std::cout << "\n========== " << title << " ==========\n";
}

// ==================== Test: Basic std types ====================
void test_pair() {
    print_section("std::pair");
    
    std::pair p1{1, 3.14};
    jo.println("pair<int, double>: ", p1);

    std::pair p2{std::pair{1, 2}, std::pair{3.0, 4.0}};
    jo.println("nested pair: ", p2);

    std::pair p3{std::string("key"), std::string("value")};
    jo.println("pair<string, string>: ", p3);

    std::pair p4{'A', 100L};
    jo.println("pair<char, long>: ", p4);
}

void test_tuple() {
    print_section("std::tuple");

    std::tuple<> t0{};
    jo.println("empty tuple: ", t0);

    std::tuple t1{42};
    jo.println("single element tuple: ", t1);

    std::tuple t2{1, 2.5, "hello", 'c'};
    jo.println("tuple<int, double, const char*, char>: ", t2);

    std::tuple t3{std::tuple{1, 2}, std::tuple{3.0, 4.0, 5.0}};
    jo.println("nested tuple: ", t3);

    std::tuple t4{std::pair{1, 2}, 3.14, std::pair{"a", "b"}};
    jo.println("tuple with pairs: ", t4);
}

void test_optional() {
    print_section("std::optional");

    std::optional<int> opt1;
    jo.println("optional<int> (empty): ", opt1);

    std::optional<int> opt2{42};
    jo.println("optional<int> (value): ", opt2);

    std::optional<std::string> opt3{"hello world"};
    jo.println("optional<string>: ", opt3);

    std::optional<std::optional<int>> opt4{std::optional<int>{99}};
    jo.println("nested optional (value): ", opt4);

    std::optional<std::vector<int>> opt6{{1, 2, 3}};
    jo.println("optional<vector<int>>: ", opt6);
}

void test_smart_pointers() {
    print_section("Smart Pointers");

    auto sp1 = std::make_shared<int>(100);
    jo.println("shared_ptr<int>: ", sp1);

    auto sp2 = sp1;
    auto sp3 = sp1;
    jo.println("shared_ptr (3 refs): ", sp1);

    auto up1 = std::make_unique<double>(3.14);
    jo.println("unique_ptr<double>: ", up1);

    std::shared_ptr<int> sp_null;
    jo.println("shared_ptr (null): ", sp_null);

    std::unique_ptr<int> up_null;
    jo.println("unique_ptr (null): ", up_null);
}

void test_complex() {
    print_section("std::complex");

    std::complex<double> c1{3.0, 4.0};
    jo.println("complex<double>: ", c1);

    std::complex<float> c2{1.5f, -2.5f};
    jo.println("complex<float>: ", c2);
}

void test_filesystem_path() {
    print_section("std::filesystem::path");

    std::filesystem::path p1{"A/B/C/D"};
    jo.println("path: ", p1);

    std::filesystem::path p2{"C:\\Users\\test\\file.txt"};
    jo.println("Windows path: ", p2);
}

// ==================== Test: Containers ====================
void test_sequence_containers() {
    print_section("Sequence Containers");

    std::vector<int> vec{1, 2, 3, 4, 5};
    jo.println("vector<int>: ", vec);

    std::vector<int> vec_empty;
    jo.println("vector<int> (empty): ", vec_empty);

    std::vector<std::vector<int>> vec2d{{1, 2}, {3, 4, 5}, {6}};
    jo.println("vector<vector<int>>: ", vec2d);

    std::deque<double> dq{1.1, 2.2, 3.3};
    jo.println("deque<double>: ", dq);

    std::list<std::string> lst{"apple", "banana", "cherry"};
    jo.println("list<string>: ", lst);

    std::forward_list<int> fwd{10, 20, 30};
    jo.println("forward_list<int>: ", fwd);

    std::array<int, 5> arr{1, 2, 3, 4, 5};
    jo.println("array<int, 5>: ", arr);
}

void test_associative_containers() {
    print_section("Associative Containers");

    std::map<int, std::string> mp{{1, "one"}, {2, "two"}, {3, "three"}};
    jo.println("map<int, string>: ", mp);

    std::map<int, int> mp_empty;
    jo.println("map<int, int> (empty): ", mp_empty);

    std::multimap<int, std::string> mmp{{1, "a"}, {1, "b"}, {2, "c"}};
    jo.println("multimap<int, string>: ", mmp);

    std::set<int> st{5, 3, 1, 4, 2};
    jo.println("set<int>: ", st);

    std::map<std::string, std::vector<int>> nested_map{
        {"primes", {2, 3, 5, 7}},
        {"evens", {2, 4, 6, 8}}
    };
    jo.println("map<string, vector<int>>: ", nested_map);
}

// ==================== Test: Custom types ====================
void test_to_string_method() {
    print_section("Custom to_string() method");

    struct Point {
        int x, y;
        std::string to_string() const {
            return "Point(" + std::to_string(x) + ", " + std::to_string(y) + ")";
        }
    };

    Point p{10, 20};
    jo.println("Point with to_string(): ", p);

    std::vector<Point> points{{1, 2}, {3, 4}, {5, 6}};
    jo.println("vector<Point>: ", points);
}

void test_string_conversion() {
    print_section("String conversion operator");

    class Color {
    public:
        int r, g, b;
        Color(int r, int g, int b) : r(r), g(g), b(b) {}
        operator std::string() const {
            return "RGB(" + std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b) + ")";
        }
    };

    Color c{255, 128, 0};
    jo.println("Color with string conversion: ", c);
}

void test_aggregate_types() {
    print_section("Aggregate Types (Reflection)");

    struct Point2D {
        int x;
        int y;
    };
    Point2D p2d{10, 20};
    jo.println("Point2D: ", p2d);

    struct Person {
        int age;
        std::string name;
        double height;
    };
    Person person{25, "Alice", 1.65};
    jo.println("Person: ", person);

    struct Address {
        std::string city;
        int zip;
    };
    struct Employee {
        std::string name;
        Address address;
        std::vector<std::string> skills;
    };
    Employee emp{"Bob", {"New York", 10001}, {"C++", "Python", "Rust"}};
    jo.println("Employee (nested): ", emp);

    const Person const_person{30, "Charlie", 1.80};
    jo.println("const Person: ", const_person);
}

// ==================== Test: Type name printing ====================
void test_type_name() {
    print_section("Type Name Printing");

    jo.print("int type: ");
    jo.println<int>();

    jo.print("vector<int> type: ");
    jo.println<std::vector<int>>();

    jo.print("map<string, int> type: ");
    jo.println<std::map<std::string, int>>();
}

// ==================== Test: ju_dbg macro ====================
void test_dbg_macro() {
    print_section("ju_dbg Macro");

    int x = 42;
    ju_dbg(x);

    double pi = 3.14159;
    ju_dbg(pi);

    std::vector<int> nums{1, 2, 3, 4, 5};
    ju_dbg(nums);

    ju_dbg(1 + 2 + 3);
}

// ==================== Test: Output to stream ====================
void test_ostream() {
    print_section("Output to Custom Stream");

    std::stringstream ss;
    auto ssprinter = ju::make_ostream_printer(ss);

    ssprinter.println("Hello");
    ssprinter.println(std::vector<int>{1, 2, 3});
    ssprinter.println(std::pair{42, "answer"});

    jo.println("stringstream content:\n", ss.str());
}

// ==================== Test: to_string ====================
void test_to_string_func() {
    print_section("to_string Function");

    auto s1 = jo.to_string(std::vector{1, 2, 3});
    jo.println("to_string(vector): ", s1);

    auto s2 = jo.to_string(std::pair{"key", 42});
    jo.println("to_string(pair): ", s2);

    struct Data { int a; double b; };
    auto s3 = jo.to_string(Data{10, 3.14});
    jo.println("to_string(aggregate): ", s3);
}

// ==================== Test: Multiple arguments ====================
void test_multiple_args() {
    print_section("Multiple Arguments");

    jo.println("a=", 1, ", b=", 2, ", c=", 3);
    jo.println("vector: ", std::vector{1, 2}, " map: ", std::map<int, int>{{1, 2}});

    // Empty println
    jo.println();
}

// ==================== Test: String types ====================
void test_string_types() {
    print_section("String Types");

    const char* cstr = "C-string";
    jo.println("const char*: ", cstr);

    std::string str = "std::string";
    jo.println("std::string: ", str);

    std::string_view sv = "string_view";
    jo.println("string_view: ", sv);

    std::vector<std::string> strs{"hello", "world"};
    jo.println("vector<string>: ", strs);

    std::map<std::string, std::string> str_map{{"key1", "val1"}, {"key2", "val2"}};
    jo.println("map<string, string>: ", str_map);
}

// ==================== Test: Numeric types ====================
void test_numeric_types() {
    print_section("Numeric Types");

    jo.println("int: ", 42);
    jo.println("double: ", 3.14159);
    jo.println("char: ", 'A');
    jo.println("bool true: ", true);
    jo.println("bool false: ", false);
}

// ==================== Test: Edge cases ====================
void test_edge_cases() {
    print_section("Edge Cases");

    auto deep = std::tuple{
        std::vector{
            std::map<int, std::optional<std::pair<int, std::string>>>{
                {1, std::pair{10, "ten"}},
                {2, std::nullopt}
            }
        }
    };
    jo.println("deeply nested: ", deep);

    std::vector<int> large(20);
    std::iota(large.begin(), large.end(), 1);
    jo.println("large vector (1-20): ", large);
}

// ==================== Test: Custom Policy ====================
void test_custom_policy() {
    print_section("Custom Policy");
    
    // Test with wcout_policy
    auto wprinter = ju::make_printer(wcout_policy{});
    wprinter.println(L"Wide string test: ", std::vector{1, 2, 3});
    
    // Test ju_dbg_with
    std::wstring val = L"JU_DBG_WITH_STRING";
    ju_dbg_with(wprinter, val);
}
void test_empty_print() {
    jo.println();
}

// ==================== Test: std::chrono::time_point ====================
void test_chrono_time_point() {
    print_section("std::chrono::time_point");

    auto now = std::chrono::system_clock::now();
    jo.println("system_clock::now(): ", now);

    auto tp1 = std::chrono::system_clock::time_point{};
    jo.println("epoch time: ", tp1);

    // steady_clock time_point
    auto steady_now = std::chrono::steady_clock::now();
    jo.println("steady_clock::now(): ", steady_now);

    // time_point in container
    std::vector times{now, tp1};
    jo.println("vector<time_point>: ", times);
}

// ==================== Test: ju::type_name ====================
void test_type_name_api() {
    print_section("ju::type_name<T>() API");

    jo.println("type_name<int>: ", ju::type_name<int>());
    jo.println("type_name<double>: ", ju::type_name<double>());
    jo.println("type_name<std::string>: ", ju::type_name<std::string>());
    jo.println("type_name<std::vector<int>>: ", ju::type_name<std::vector<int>>());
    jo.println("type_name<std::map<std::string, int>>: ", ju::type_name<std::map<std::string, int>>());

    // constexpr test
    constexpr auto name = ju::type_name<std::optional<int>>();
    jo.println("constexpr type_name<optional<int>>: ", name);

    // with reference types (should strip reference)
    jo.println("type_name<int&>: ", ju::type_name<int&>());
    jo.println("type_name<const int&>: ", ju::type_name<const int&>());
}

void test_custom_printer() {
    print_section("Custom Printer");
    struct UIPrinter {
        using char_type = char;
        void write(std::string_view str) {
            if (str.empty()) return;
            std::cout << "[ui] " << str;
        }
    };
    static auto uio = ju::make_printer(UIPrinter{});
    uio.println("start with tag: ui");
}

int main() {
    std::cout << "==============================================\n";
    std::cout << "    eprint Library Test Suite (New API)       \n";
    std::cout << "==============================================\n";
    
    test_pair();
    test_tuple();
    test_optional();
    test_smart_pointers();
    test_complex();
    test_filesystem_path();
    
    test_sequence_containers();
    test_associative_containers();
    
    test_to_string_method();
    test_string_conversion();
    test_aggregate_types();
    
    test_type_name();
    test_dbg_macro();
    test_ostream();
    test_to_string_func();
    test_multiple_args();
    test_string_types();
    test_numeric_types();
    test_edge_cases();
    test_custom_policy();
    test_empty_print();
    test_chrono_time_point();
    test_type_name_api();
    test_custom_printer();
    std::cout << "\n==============================================\n";
    std::cout << "              All Tests Completed!            \n";
    std::cout << "==============================================\n";
    
    return 0;
}

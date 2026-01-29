//
// Created by jujin on 25-9-9.
// Comprehensive test cases for eprint library
//
#include "eprint.hpp"
#include <print>
#include <deque>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_set>
#include <forward_list>
#include <array>
#include <stack>
#include <queue>
#include <variant>
#include <any>
#include <bitset>
#include <functional>
#include <numeric>

using namespace ju;

template <class... _Types>
void println_(std::format_string<_Types...> _Fmt, _Types&&... _Args) {
    std::cout << std::format(_Fmt, std::forward<_Types>(_Args)...) << '\n';
}

void print_section(const char* title) {
    std::cout << "\n========== " << title << " ==========\n";
}

// ==================== Test: Basic std types ====================
void test_pair() {
    print_section("std::pair");
    
    // Basic pair
    std::pair p1{1, 3.14};
    println_("[std::format] pair<int, double>: {}", p1);
    println("[ju::println] pair<int, double>: ", p1);
    
    // Nested pair
    std::pair p2{std::pair{1, 2}, std::pair{3.0, 4.0}};
    println_("[std::format] nested pair: {}", p2);
    println("[ju::println] nested pair: ", p2);
    
    // Pair with string
    std::pair p3{std::string("key"), std::string("value")};
    println("[ju::println] pair<string, string>: ", p3);
    
    // Pair with different types
    std::pair p4{'A', 100L};
    println("[ju::println] pair<char, long>: ", p4);
}

void test_tuple() {
    print_section("std::tuple");
    
    // Empty tuple
    std::tuple<> t0{};
    println("[ju::println] empty tuple: ", t0);
    
    // Single element tuple
    std::tuple t1{42};
    println("[ju::println] single element tuple: ", t1);
    
    // Multiple elements
    std::tuple t2{1, 2.5, "hello", 'c'};
    println_("[std::format] tuple<int, double, const char*, char>: {}", t2);
    println("[ju::println] tuple<int, double, const char*, char>: ", t2);
    
    // Nested tuple
    std::tuple t3{std::tuple{1, 2}, std::tuple{3.0, 4.0, 5.0}};
    println("[ju::println] nested tuple: ", t3);
    
    // Tuple with pair
    std::tuple t4{std::pair{1, 2}, 3.14, std::pair{"a", "b"}};
    println("[ju::println] tuple with pairs: ", t4);
}

void test_optional() {
    print_section("std::optional");
    
    // Empty optional
    std::optional<int> opt1;
    println_("[std::format] optional<int> (empty): {}", opt1);
    println("[ju::println] optional<int> (empty): ", opt1);
    
    // Optional with value
    std::optional<int> opt2{42};
    println_("[std::format] optional<int> (value): {}", opt2);
    println("[ju::println] optional<int> (value): ", opt2);
    
    // Optional with string
    std::optional<std::string> opt3{"hello world"};
    println("[ju::println] optional<string>: ", opt3);
    
    // Nested optional
    std::optional<std::optional<int>> opt4{std::optional<int>{99}};
    println("[ju::println] nested optional (value): ", opt4);
    
    std::optional<std::optional<int>> opt5{std::nullopt};
    println("[ju::println] nested optional (outer empty): ", opt5);
    
    // Optional with complex type
    std::optional<std::vector<int>> opt6{{1, 2, 3}};
    println("[ju::println] optional<vector<int>>: ", opt6);
}

void test_smart_pointers() {
    print_section("Smart Pointers");
    
    // shared_ptr
    auto sp1 = std::make_shared<int>(100);
    println_("[std::format] shared_ptr<int>: {}", sp1);
    println("[ju::println] shared_ptr<int>: ", sp1);
    
    // shared_ptr with multiple references
    auto sp2 = sp1;
    auto sp3 = sp1;
    println("[ju::println] shared_ptr (3 refs): ", sp1);
    
    // unique_ptr
    auto up1 = std::make_unique<double>(3.14);
    println_("[std::format] unique_ptr<double>: {}", up1);
    println("[ju::println] unique_ptr<double>: ", up1);
    
    // nullptr shared_ptr
    std::shared_ptr<int> sp_null;
    println("[ju::println] shared_ptr (null): ", sp_null);
    
    // nullptr unique_ptr
    std::unique_ptr<int> up_null;
    println("[ju::println] unique_ptr (null): ", up_null);
}

void test_complex() {
    print_section("std::complex");
    
    std::complex<double> c1{3.0, 4.0};
    println_("[std::format] complex<double>: {}", c1);
    println("[ju::println] complex<double>: ", c1);
    
    std::complex<float> c2{1.5f, -2.5f};
    println("[ju::println] complex<float>: ", c2);
    
    std::complex<double> c3{0.0, 1.0};
    println("[ju::println] complex<double> (pure imaginary): ", c3);
}

void test_filesystem_path() {
    print_section("std::filesystem::path");
    
    std::filesystem::path p1{"A/B/C/D"};
    println_("[std::format] path: {}", p1);
    println("[ju::println] path: ", p1);
    
    std::filesystem::path p2{"C:\\Users\\test\\file.txt"};
    println("[ju::println] Windows path: ", p2);
    
    std::filesystem::path p3{"/home/user/documents"};
    println("[ju::println] Unix path: ", p3);
}

void test_chrono() {
    print_section("std::chrono");
    
    println_("[std::format] microseconds: {}", std::chrono::microseconds(1500));
    println_("[std::format] milliseconds: {}", std::chrono::milliseconds(2500));
    println_("[std::format] seconds: {}", std::chrono::seconds(90));
    println_("[std::format] minutes: {}", std::chrono::minutes(5));
    println_("[std::format] hours: {}", std::chrono::hours(2));
    
    // ju::println with chrono
    println("[ju::println] nanoseconds: ", std::chrono::nanoseconds(123456789));
}

// ==================== Test: Containers ====================
void test_sequence_containers() {
    print_section("Sequence Containers");
    
    // vector
    std::vector<int> vec{1, 2, 3, 4, 5};
    println("[ju::println] vector<int>: ", vec);
    
    // empty vector
    std::vector<int> vec_empty;
    println("[ju::println] vector<int> (empty): ", vec_empty);
    
    // nested vector
    std::vector<std::vector<int>> vec2d{{1, 2}, {3, 4, 5}, {6}};
    println("[ju::println] vector<vector<int>>: ", vec2d);
    
    // deque
    std::deque<double> dq{1.1, 2.2, 3.3};
    println("[ju::println] deque<double>: ", dq);
    
    // list
    std::list<std::string> lst{"apple", "banana", "cherry"};
    println("[ju::println] list<string>: ", lst);
    
    // forward_list
    std::forward_list<int> fwd{10, 20, 30};
    println("[ju::println] forward_list<int>: ", fwd);
    
    // array
    std::array<int, 5> arr{1, 2, 3, 4, 5};
    println("[ju::println] array<int, 5>: ", arr);
}

void test_associative_containers() {
    print_section("Associative Containers");
    
    // map
    std::map<int, std::string> mp{{1, "one"}, {2, "two"}, {3, "three"}};
    println("[ju::println] map<int, string>: ", mp);
    
    // empty map
    std::map<int, int> mp_empty;
    println("[ju::println] map<int, int> (empty): ", mp_empty);
    
    // multimap
    std::multimap<int, std::string> mmp{{1, "a"}, {1, "b"}, {2, "c"}};
    println("[ju::println] multimap<int, string>: ", mmp);
    
    // unordered_map
    std::unordered_map<std::string, int> ump{{"one", 1}, {"two", 2}};
    println("[ju::println] unordered_map<string, int>: ", ump);
    
    // unordered_multimap
    std::unordered_multimap<int, char> ummp{{1, 'a'}, {1, 'b'}, {2, 'c'}};
    println("[ju::println] unordered_multimap<int, char>: ", ummp);
    
    // set
    std::set<int> st{5, 3, 1, 4, 2};
    println("[ju::println] set<int>: ", st);
    
    // multiset
    std::multiset<int> mst{1, 2, 2, 3, 3, 3};
    println("[ju::println] multiset<int>: ", mst);
    
    // unordered_set
    std::unordered_set<std::string> ust{"apple", "banana"};
    println("[ju::println] unordered_set<string>: ", ust);
    
    // Nested map
    std::map<std::string, std::vector<int>> nested_map{
        {"primes", {2, 3, 5, 7}},
        {"evens", {2, 4, 6, 8}}
    };
    println("[ju::println] map<string, vector<int>>: ", nested_map);
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
    println("[ju::println] Point with to_string(): ", p);
    
    std::vector<Point> points{{1, 2}, {3, 4}, {5, 6}};
    println("[ju::println] vector<Point>: ", points);
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
    println("[ju::println] Color with string conversion: ", c);
}

void test_aggregate_types() {
    print_section("Aggregate Types (Reflection)");
    
    // Simple struct
    struct Point2D {
        int x;
        int y;
    };
    Point2D p2d{10, 20};
    println("[ju::println] Point2D: ", p2d);
    
    // Struct with different types
    struct Person {
        int age;
        std::string name;
        double height;
    };
    Person person{25, "Alice", 1.65};
    println("[ju::println] Person: ", person);
    
    // Nested struct
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
    println("[ju::println] Employee (nested): ", emp);
    
    // Const aggregate
    const Person const_person{30, "Charlie", 1.80};
    println("[ju::println] const Person: ", const_person);
    
    // Empty struct
    struct Empty {};
    Empty e;
    println("[ju::println] Empty struct: ", e);
    
    // Struct with many fields
    struct ManyFields {
        int a, b, c, d, e;
        double f, g;
        std::string h;
    };
    ManyFields mf{1, 2, 3, 4, 5, 6.0, 7.0, "eight"};
    println("[ju::println] ManyFields: ", mf);
}

// ==================== Test: Type name printing ====================
void test_type_name() {
    print_section("Type Name Printing");
    
    auto print_type = []<typename T>(T&&) {
        print("[ju::println] type of arg: ");
        println<T>();
    };
    
    print_type(42);
    print_type(3.14);
    print_type(std::string("hello"));
    print_type(std::vector<int>{1, 2, 3});
    print_type(std::map<std::string, int>{});
    print_type(std::optional<std::pair<int, double>>{});
    print_type(std::tuple<int, double, std::string>{});
    
    // Complex nested type
    print("[ju::println] complex nested type: ");
    println<std::map<std::string, std::vector<std::optional<int>>>>();
}

// ==================== Test: ju_dbg macro ====================
void test_dbg_macro() {
    print_section("ju_dbg Macro");
    
    int x = 42;
    ju_dbg(x);
    
    double pi = 3.14159;
    ju_dbg(pi);
    
    std::string msg = "hello";
    ju_dbg(msg);
    
    std::vector<int> nums{1, 2, 3, 4, 5};
    ju_dbg(nums);
    
    auto complex_expr = std::tuple{
        std::pair{1, "one"},
        std::vector{2, 3, 4},
        std::optional<double>{2.718}
    };
    ju_dbg(complex_expr);
    
    // Expression in ju_dbg
    ju_dbg(1 + 2 + 3);
    ju_dbg((std::vector<int>{10, 20, 30}));
}

// ==================== Test: Output to stream ====================
void test_ostream() {
    print_section("Output to Custom Stream");
    
    std::stringstream ss;
    
    ju::println(static_cast<std::ostream&>(ss), "Hello");
    ju::println(static_cast<std::ostream&>(ss), std::vector<int>{1, 2, 3});
    ju::println(static_cast<std::ostream&>(ss), std::pair{42, "answer"});
    
    println("[ju::println] stringstream content: ", ss.str());
}

// ==================== Test: Multiple arguments ====================
void test_multiple_args() {
    print_section("Multiple Arguments");
    
    println("a=", 1, ", b=", 2, ", c=", 3);
    println("vector: ", std::vector{1, 2}, " map: ", std::map<int, int>{{1, 2}});
    
    // Empty println
    println();
}

// ==================== Test: String types ====================
void test_string_types() {
    print_section("String Types");
    
    // C-string
    const char* cstr = "C-string";
    println("[ju::println] const char*: ", cstr);
    
    // std::string
    std::string str = "std::string";
    println("[ju::println] std::string: ", str);
    
    // std::string_view
    std::string_view sv = "string_view";
    println("[ju::println] string_view: ", sv);
    
    // String in container (should be quoted)
    std::vector<std::string> strs{"hello", "world"};
    println("[ju::println] vector<string>: ", strs);
    
    // String in tuple
    std::tuple<std::string, int> str_tuple{"test", 42};
    println("[ju::println] tuple<string, int>: ", str_tuple);
    
    // String in map
    std::map<std::string, std::string> str_map{{"key1", "val1"}, {"key2", "val2"}};
    println("[ju::println] map<string, string>: ", str_map);
}

// ==================== Test: Edge cases ====================
void test_edge_cases() {
    print_section("Edge Cases");
    
    // Deeply nested structure
    auto deep = std::tuple{
        std::vector{
            std::map<int, std::optional<std::pair<int, std::string>>>{
                {1, std::pair{10, "ten"}},
                {2, std::nullopt}
            }
        }
    };
    println("[ju::println] deeply nested: ", deep);
    
    // Large container
    std::vector<int> large(20);
    std::iota(large.begin(), large.end(), 1);
    println("[ju::println] large vector (1-20): ", large);
    
    // Unicode strings
    std::string unicode = "Hello, World!";
    println("[ju::println] ASCII string: ", unicode);
}

// ==================== Test: Numeric types ====================
void test_numeric_types() {
    print_section("Numeric Types");
    
    println("[ju::println] int: ", 42);
    println("[ju::println] unsigned: ", 42u);
    println("[ju::println] long: ", 42L);
    println("[ju::println] long long: ", 42LL);
    println("[ju::println] float: ", 3.14f);
    println("[ju::println] double: ", 3.14159265358979);
    println("[ju::println] char: ", 'A');
    println("[ju::println] bool true: ", true);
    println("[ju::println] bool false: ", false);
}

int main() {
    std::cout << "==============================================\n";
    std::cout << "    eprint Library Comprehensive Test Suite   \n";
    std::cout << "==============================================\n";
    
    // Basic std types
    test_pair();
    test_tuple();
    test_optional();
    test_smart_pointers();
    test_complex();
    test_filesystem_path();
    test_chrono();
    
    // Containers
    test_sequence_containers();
    test_associative_containers();
    
    // Custom types
    test_to_string_method();
    test_string_conversion();
    test_aggregate_types();
    
    // Type name
    test_type_name();
    
    // Utilities
    test_dbg_macro();
    test_ostream();
    test_multiple_args();
    test_string_types();
    test_numeric_types();
    
    // Edge cases
    test_edge_cases();
    
    std::cout << "\n==============================================\n";
    std::cout << "              All Tests Completed!            \n";
    std::cout << "==============================================\n";
    
    return 0;
}

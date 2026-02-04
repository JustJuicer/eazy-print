//
// Created by jujin on 25-9-9.
//

#ifndef EPRINT_HPP
#define EPRINT_HPP
#include <format>
#include <tuple>
#include <optional>
#include <filesystem>
#include <memory>
#include <complex>
#include <iostream>
#include <array>
#include <map>
#include <unordered_map>
#include <chrono>
#include <iomanip>
/////////////////////// CONCEPT ////////////////////////////////////////////
namespace ju {
namespace _concept {
    namespace stringlike{
        template <typename CharT>
        concept CharacterType = std::same_as<CharT, char> ||
                                std::same_as<CharT, wchar_t> ||
                                std::same_as<CharT, char8_t> ||
                                std::same_as<CharT, char16_t> ||
                                std::same_as<CharT, char32_t>;

        template <typename T>
        struct is_basic_string : std::false_type {};

        template <typename CharT, typename Traits, typename Alloc>
        struct is_basic_string<std::basic_string<CharT, Traits, Alloc>> : std::true_type {};

        template <typename T>
        concept IsBasicString = is_basic_string<std::decay_t<T>>::value;

        template <typename T>
        struct is_basic_string_view : std::false_type {};

        template <typename CharT, typename Traits>
        struct is_basic_string_view<std::basic_string_view<CharT, Traits>> : std::true_type {};

        template <typename T>
        concept IsBasicStringView = is_basic_string_view<std::decay_t<T>>::value;

        template <typename T>
        concept string_like =
            // 1. 指向字符类型的指针（C 字符串）
            (std::is_pointer_v<T> && CharacterType<std::remove_cvref_t<std::remove_pointer_t<T>>>) ||
            // 2. 字符数组（如 char[10], const char[5]）
            (std::is_array_v<T> && CharacterType<std::remove_extent_t<T>>) ||
            // 3. std::basic_string
            IsBasicString<T> ||
            // 4. std::basic_string_view
            IsBasicStringView<T>;
        static_assert(stringlike::string_like<std::string&>);
        static_assert(stringlike::string_like<std::string_view>);
        static_assert(stringlike::string_like<char[3]>);
        static_assert(stringlike::string_like<const char*>);
        static_assert(stringlike::string_like<const char16_t*>);
        static_assert(stringlike::string_like<const volatile char8_t*>);
        static_assert(!stringlike::string_like<const volatile std::array<int, 5>>);
    }

    template <typename T>
    concept string_like = stringlike::string_like<std::remove_reference_t<T>>;

namespace std_t {
    template<typename T, template <typename...> class Template>
    struct is_instance_of : std::false_type {};

    template<template <typename...> class Template, typename ...Args>
    struct is_instance_of<Template<Args...>, Template> : std::true_type {};

    static_assert(is_instance_of<std::vector<int>, std::vector>::value);

    template <typename T,  template<typename...> typename ...Types> // true if T is in Types
    constexpr bool any_of_container = std::disjunction_v<is_instance_of<T, Types>...>;

    template <typename T, typename ...Types>
    constexpr bool any_of = std::disjunction_v<std::is_same<T, Types>...>;

    template <typename T>
    concept is_map = any_of_container<T, std::map, std::multimap, std::unordered_map, std::unordered_multimap>;

    template <typename T>
    struct is_time_point : std::false_type {};

    template <typename Clock, typename Duration>
    struct is_time_point<std::chrono::time_point<Clock, Duration>> : std::true_type {};

    template <typename T>
    concept is_chrono_time_point = is_time_point<std::decay_t<T>>::value;
}

}

}
/////////////////////// CONCEPT ////////////////////////////////////////////


/////////////////////// POLICY /////////////////////////////////////////////
// Policy concept
template <typename P>
concept PrintPolicy = requires {
    typename P::char_type;
} && requires(P& p, std::basic_string_view<typename P::char_type> sv) {
    { p.write(sv) } -> std::same_as<void>;
};

template <typename P>
concept FlushablePolicy = PrintPolicy<P> && requires(P& p) {
    { p.flush() } -> std::same_as<void>;
};

// Character literals traits - works for any CharT
template <typename CharT>
struct literals {
    static constexpr CharT null[] = {'n', 'u', 'l', 'l', 'p', 't', 'r', 0};
    static constexpr CharT space[] = {' ', 0};
    static constexpr CharT none[] = {'N','o','n','e', 0};
    static constexpr CharT open_brace[] = {'{',' ', 0};
    static constexpr CharT close_brace[] = {' ','}', 0};
    static constexpr CharT open_bracket[] = {'[', 0};
    static constexpr CharT close_bracket[] = {']', 0};
    static constexpr CharT comma_space[] = {',',' ', 0};
    static constexpr CharT colon_space[] = {':',' ', 0};
    static constexpr CharT quote[] = {'"', 0};
    static constexpr CharT newline[] = {'\n', 0};
    static constexpr CharT open_paren[] = {'(', 0};
    static constexpr CharT close_paren[] = {')', 0};
    static constexpr CharT address_prefix[] = {'{',' ','a','d','d','r','e','s','s',':',' ', 0};
    static constexpr CharT count_prefix[] = {',',' ','c','o','u','n','t',':',' ', 0};
    static constexpr CharT at[] = {' ','a','t',' ', 0};
    static constexpr CharT lt[] = {'<', 0};
    static constexpr CharT gt[] = {'>', 0};
};

// Built-in policies
struct cout_policy {
    using char_type = char;
    void write(std::string_view sv) { std::cout << sv; }
    void flush() { std::cout.flush(); }
};

struct wcout_policy {
    using char_type = wchar_t;
    void write(std::wstring_view sv) { std::wcout << sv; }
    void flush() { std::wcout.flush(); }
};

template <typename CharT>
struct ostream_policy {
    using char_type = CharT;
    std::basic_ostream<CharT>& os_;
    
    ostream_policy(std::basic_ostream<CharT>& os) : os_(os) {}
    void write(std::basic_string_view<CharT> sv) { os_ << sv; }
    void flush() { os_.flush(); }
};

/////////////////////// POLICY /////////////////////////////////////////////

namespace ju {

namespace _inner {
/////////////////////// AGGREGATE TYPE /////////////////////////////////////
template <typename T>
constexpr std::string_view get_raw_name() {
#ifdef _MSC_VER
    return __FUNCSIG__;
#else
    return __PRETTY_FUNCTION__;
#endif
}

template <typename T>
constexpr std::string_view get_type_name() { // don't pass in type with reference
    constexpr std::string_view sample = get_raw_name<int>();
    constexpr size_t prefix_length = sample.find("int");
    constexpr std::string_view str = get_raw_name<T>();
    constexpr size_t suffix_length = sample.size() - prefix_length - 3;
    constexpr auto name =
        str.substr(prefix_length, str.size() - prefix_length - suffix_length);

    return name;
}
//---members_count---start
namespace detail {
    struct UniversalType {
        template <typename T>
        operator T();
    };

    template <typename T, typename UniversalParam, typename = void, typename ...Args>
    struct is_constructable: std::false_type {};

    template <typename T, typename UniversalParam, typename ...Args>
    struct is_constructable<
        T,
        UniversalParam,
        std::void_t<decltype(T{{Args{}}..., {UniversalParam{}}})>,
        Args...> : std::true_type {};

    template <typename T, typename UniversalParam, typename ...Args>
    constexpr bool is_constructable_v = is_constructable<T, UniversalParam, void, Args...>::value;

    template <typename T, typename ...Args>
    constexpr size_t members_count_impl() {
        if constexpr (is_constructable_v<T, UniversalType, Args...>) {
            return members_count_impl<T, Args..., UniversalType>();
        } else {
            return sizeof...(Args);
        }
    }
}

template <typename T>
requires std::is_aggregate_v<T>
constexpr size_t members_count() {
    if constexpr (std::is_empty_v<T>) {
        return 0;
    } else {
        return detail::members_count_impl<T>();
    }
}
template <typename T>
constexpr size_t members_count_v = members_count<T>();
//---members_count---end

//---members_name---start
template <auto ptr>
inline constexpr std::string_view get_member_name() {
#if defined(_MSC_VER)
    constexpr std::string_view func_name = __FUNCSIG__;
#else
    constexpr std::string_view func_name = __PRETTY_FUNCTION__;
#endif

#if defined(__clang__)
    auto split = func_name.substr(0, func_name.size() - 2);
    return split.substr(split.find_last_of(":.") + 1);
#elif defined(__GNUC__)
    auto split = func_name.substr(0, func_name.rfind(")}"));
    return split.substr(split.find_last_of(":") + 1);
#elif defined(_MSC_VER)
    auto split = func_name.substr(0, func_name.rfind("}>"));
    return split.substr(split.rfind("->") + 2);
#else
    static_assert(false,
                  "You are using an unsupported compiler. Please use GCC, Clang "
                  "or MSVC or switch to the rfl::Field-syntax.");
#endif
}

template <typename T>
struct wrapper {
    inline static std::remove_cvref_t<T> value;
};

template <class T>
inline constexpr std::remove_cvref_t<T>& get_fake_object() noexcept {
    // return wrapper<std::remove_cvref_t<T>>::value;
    return wrapper<T>::value; //Change
}

template <class T, std::size_t n>
struct object_tuple_view_helper {
    static constexpr auto tuple_view() {
        static_assert(
            sizeof(T) < 0,
            "\n\nThis error occurs for one of two reasons:\n\n"
            "1) You have created a struct with too many fields, which is "
            "unsupported. \n\n"
            "2) Your struct is not an aggregate type. You can make it aggregated, "
            "or defined a YLT_REFL macro. \n\n");
    }
};

template <class T>
struct object_tuple_view_helper<T, 0> {
    static constexpr auto tuple_view() { return std::tie(); }
    template <typename U>
    static constexpr auto tuple_view(U&&) { return std::tie(); }
};

#define SPRINT_REFLECT_TUPLE_VIEW(n, ...) \
template <typename T> \
struct object_tuple_view_helper<T, n> { \
    static constexpr auto tuple_view() { \
        auto&[__VA_ARGS__] = get_fake_object<T>(); \
        auto ref_tuple = std::tie(__VA_ARGS__); \
        auto get_ptr = [] (auto&... fake_obj_ref) { \
            return std::make_tuple(&fake_obj_ref...); \
        }; \
        return std::apply(get_ptr, ref_tuple); \
    } \
    template <typename U> \
    static constexpr auto tuple_view(U&& t) { \
        auto& [__VA_ARGS__] = t; \
        return std::tie(__VA_ARGS__); \
    } \
};

////////////////// generate macros ////////////////////

#ifndef __IDE_HELPER__a3b1f75c2e4d9a87bb5f6d0c94ef2176

SPRINT_REFLECT_TUPLE_VIEW(1, f0);
SPRINT_REFLECT_TUPLE_VIEW(2, f0, f1);
SPRINT_REFLECT_TUPLE_VIEW(3, f0, f1, f2);
SPRINT_REFLECT_TUPLE_VIEW(
    4, f0, f1, f2, f3);
SPRINT_REFLECT_TUPLE_VIEW(
    5, f0, f1, f2, f3, f4);
SPRINT_REFLECT_TUPLE_VIEW(
    6, f0, f1, f2, f3, f4, f5);
SPRINT_REFLECT_TUPLE_VIEW(
    7, f0, f1, f2, f3, f4, f5, f6);
SPRINT_REFLECT_TUPLE_VIEW(
    8, f0, f1, f2, f3, f4, f5, f6, f7);
SPRINT_REFLECT_TUPLE_VIEW(
    9, f0, f1, f2, f3, f4, f5, f6, f7, f8);
SPRINT_REFLECT_TUPLE_VIEW(
    10, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9);
SPRINT_REFLECT_TUPLE_VIEW(
    11, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10);
SPRINT_REFLECT_TUPLE_VIEW(
    12, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11);
SPRINT_REFLECT_TUPLE_VIEW(
    13, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12);
SPRINT_REFLECT_TUPLE_VIEW(
    14, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13);
SPRINT_REFLECT_TUPLE_VIEW(
    15, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14);
SPRINT_REFLECT_TUPLE_VIEW(
    16, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15);
SPRINT_REFLECT_TUPLE_VIEW(
    17, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15,
    f16);
SPRINT_REFLECT_TUPLE_VIEW(
    18, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15,
    f16, f17);
SPRINT_REFLECT_TUPLE_VIEW(
    19, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15,
    f16, f17, f18);
SPRINT_REFLECT_TUPLE_VIEW(
    20, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15,
    f16, f17, f18, f19);
SPRINT_REFLECT_TUPLE_VIEW(
    21, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15,
    f16, f17, f18, f19, f20);
SPRINT_REFLECT_TUPLE_VIEW(
    22, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15,
    f16, f17, f18, f19, f20, f21);
SPRINT_REFLECT_TUPLE_VIEW(
    23, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15,
    f16, f17, f18, f19, f20, f21, f22);
SPRINT_REFLECT_TUPLE_VIEW(
    24, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15,
    f16, f17, f18, f19, f20, f21, f22, f23);
SPRINT_REFLECT_TUPLE_VIEW(
    25, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15,
    f16, f17, f18, f19, f20, f21, f22, f23, f24);
SPRINT_REFLECT_TUPLE_VIEW(
    26, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15,
    f16, f17, f18, f19, f20, f21, f22, f23, f24, f25);
SPRINT_REFLECT_TUPLE_VIEW(
    27, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15,
    f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26);
SPRINT_REFLECT_TUPLE_VIEW(
    28, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15,
    f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27);
SPRINT_REFLECT_TUPLE_VIEW(
    29, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15,
    f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28);
SPRINT_REFLECT_TUPLE_VIEW(
    30, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15,
    f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29);
SPRINT_REFLECT_TUPLE_VIEW(
    31, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15,
    f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30);
SPRINT_REFLECT_TUPLE_VIEW(
    32, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15,
    f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30,
    f31);
SPRINT_REFLECT_TUPLE_VIEW(
    33, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15,
    f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30,
    f31, f32);

#endif

////////////////// generate macros ////////////////////
template <class T>
struct Wrapper {
    using Type = T;
    T v;
};

template <class T>
Wrapper(T) -> Wrapper<T>;

// This workaround is necessary for clang.
template <class T>
inline constexpr auto wrap(const T& arg) noexcept {
    return Wrapper{arg};
}


template <typename T>
inline constexpr auto struct_to_tuple() {
    return object_tuple_view_helper<T, members_count_v<T>>::tuple_view();
}

template <typename _Ty>
inline constexpr std::array<std::string_view, members_count_v<_Ty>>
get_member_names() {
    constexpr size_t member_count = members_count<_Ty>();
    using T = std::remove_cvref_t<_Ty>;
    std::array<std::string_view, member_count> arr;
    constexpr auto tp = struct_to_tuple<T>();
    [&]<size_t... Is>(std::index_sequence<Is...>){ //Change
        ((arr[Is] = get_member_name<wrap(std::get<Is>(tp))>()), ...);
    }(std::make_index_sequence<member_count>{});
    return arr;
}


template <typename T>
constexpr auto object_to_tuple(T&& obj) {
    using type = std::decay_t<T>;
    return object_tuple_view_helper<type, members_count_v<type>>::tuple_view(obj);
}

//---members_name---end

/////////////////////// AGGREGATE TYPE /////////////////////////////////////


// Helper to convert value to string for Policy output
template <typename CharT, typename T>
std::basic_string<CharT> to_basic_string(T&& val) {
    std::basic_ostringstream<CharT> oss;
    oss << std::forward<T>(val);
    return oss.str();
}

template <PrintPolicy Policy, typename Obj>
void _print_impl(Policy& policy, Obj&& obj, size_t depth = 0) {
    using CharT = typename Policy::char_type;
    using Lit = literals<CharT>;
    using Decay_Obj = std::decay_t<Obj>;
    using string_type = std::basic_string<CharT>;
    using string_view_type = std::basic_string_view<CharT>;

    if constexpr (_concept::std_t::is_instance_of<Decay_Obj, std::shared_ptr>::value) {
        if (!obj) {
            policy.write(Lit::null);
        } else {
            policy.write(Lit::address_prefix);
            policy.write(to_basic_string<CharT>(static_cast<void*>(obj.get())));
            policy.write(Lit::count_prefix);
            policy.write(to_basic_string<CharT>(obj.use_count()));
            policy.write(Lit::close_brace);
        }
    } else if constexpr (_concept::std_t::is_instance_of<Decay_Obj, std::unique_ptr>::value) {
        if (!obj) {
            policy.write(Lit::null);
        } else {
            policy.write(Lit::address_prefix);
            policy.write(to_basic_string<CharT>(static_cast<void*>(obj.get())));
            policy.write(Lit::close_brace);
        }
    } else if constexpr (_concept::string_like<Obj>) {
        if (depth != 0) {
            policy.write(Lit::quote);
            if constexpr (std::is_convertible_v<Obj, string_view_type>) {
                policy.write(string_view_type(std::forward<Obj>(obj)));
            } else {
                policy.write(string_type(std::forward<Obj>(obj)));
            }
            policy.write(Lit::quote);
        } else {
            if constexpr (std::is_convertible_v<Obj, string_view_type>) {
                policy.write(string_view_type(std::forward<Obj>(obj)));
            } else {
                policy.write(string_type(std::forward<Obj>(obj)));
            }
        }
    } else if constexpr (std::is_arithmetic_v<Decay_Obj> || std::is_pointer_v<Decay_Obj>) {
        policy.write(to_basic_string<CharT>(std::forward<Obj>(obj)));
    } else if constexpr (requires { std::forward<Obj>(obj).to_string(); }) {
        policy.write(string_type(std::forward<Obj>(obj).to_string()));
    } else if constexpr (std::convertible_to<Obj, string_type>) {
        policy.write(string_type(std::forward<Obj>(obj)));
    } else if constexpr (_concept::std_t::is_map<Decay_Obj>) {
        policy.write(Lit::open_brace);
        bool is_first = true;
        for (auto&& pair : std::forward<Obj>(obj)) {
            if (!is_first) {
                policy.write(Lit::comma_space);
            }
            is_first = false;
            _print_impl(policy, pair.first, depth + 1);
            policy.write(Lit::colon_space);
            _print_impl(policy, pair.second, depth + 1);
        }
        policy.write(Lit::close_brace);
    } else if constexpr (std::same_as<Decay_Obj, std::filesystem::path>) {
        // Must be before range check since path is iterable
        auto path_str = obj.string();
        if (depth != 0) {
            policy.write(Lit::quote);
            policy.write(string_type(path_str.begin(), path_str.end()));
            policy.write(Lit::quote);
        } else {
            policy.write(string_type(path_str.begin(), path_str.end()));
        }
    } else if constexpr (std::ranges::range<Obj>) {
        policy.write(Lit::open_bracket);
        bool first = true;
        for (auto&& e : std::forward<Obj>(obj)) {
            if (!first) policy.write(Lit::comma_space);
            first = false;
            _print_impl(policy, e, depth + 1);
        }
        policy.write(Lit::close_bracket);
    } else if constexpr (_concept::std_t::is_instance_of<Decay_Obj, std::pair>::value) {
        policy.write(Lit::open_paren);
        _print_impl(policy, obj.first, depth + 1);
        policy.write(Lit::comma_space);
        _print_impl(policy, obj.second, depth + 1);
        policy.write(Lit::close_paren);
    } else if constexpr (_concept::std_t::is_instance_of<Decay_Obj, std::tuple>::value) {
        auto for_each = [](auto&& fn, auto... args) {
            (fn(args), ...);
        };
        policy.write(Lit::open_paren);
        bool is_first = true;
        std::apply([&]<typename... Args_>(Args_&&... args) {
            for_each([&]<typename T>(T&& ele) {
                if (is_first) {
                    _print_impl(policy, std::forward<T>(ele), depth + 1);
                    is_first = false;
                } else {
                    policy.write(Lit::comma_space);
                    _print_impl(policy, std::forward<T>(ele), depth + 1);
                }
            }, std::forward<Args_>(args)...);
        }, obj);
        policy.write(Lit::close_paren);
    } else if constexpr (_concept::std_t::is_instance_of<Decay_Obj, std::optional>::value) {
        if (obj.has_value()) {
            _print_impl(policy, obj.value(), depth);
        } else {
            policy.write(Lit::none);
        }
    } else if constexpr (_concept::std_t::is_instance_of<Decay_Obj, std::complex>::value) {
        _print_impl(policy, std::pair{obj.real(), obj.imag()}, depth + 1);
    } else if constexpr (_concept::std_t::is_chrono_time_point<Decay_Obj>) {
        using clock_type = typename Decay_Obj::clock;
        if constexpr (std::is_same_v<clock_type, std::chrono::system_clock>) {
            // system_clock: convert to readable date/time
            auto time_t_val = std::chrono::system_clock::to_time_t(obj);
            std::tm tm_val{};
#ifdef _MSC_VER
            localtime_s(&tm_val, &time_t_val);
#else
            localtime_r(&time_t_val, &tm_val);
#endif
            std::basic_ostringstream<CharT> oss;
            oss << std::put_time(&tm_val, "%Y-%m-%d %H:%M:%S");
            policy.write(oss.str());
        } else {
            // other clocks (steady_clock, high_resolution_clock, etc.): print duration since epoch
            auto duration = obj.time_since_epoch();
            auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
            std::basic_ostringstream<CharT> oss;
            oss << ns << "ns since epoch";
            policy.write(oss.str());
        }
    } else if constexpr (std::is_aggregate_v<Decay_Obj>) {
        using type = Decay_Obj;
        auto members = object_to_tuple(obj);
        auto members_name = get_member_names<type>();
        if (depth != 0) {
            policy.write(Lit::open_brace);
        } else {
            auto type_name = get_type_name<type>();
            policy.write(string_type(type_name.begin(), type_name.end()));
            policy.write(Lit::space);
            policy.write(Lit::open_brace);
        }
        constexpr auto mc = members_count_v<type>;
        auto inner_printer = [&]<size_t Is>(std::integral_constant<size_t, Is>, auto member_name, auto&& member_value) {
            if constexpr (Is != 0) {
                policy.write(Lit::comma_space);
            }
            policy.write(string_type(member_name.begin(), member_name.end()));
            policy.write(Lit::colon_space);
            _print_impl(policy, std::forward<decltype(member_value)>(member_value), depth + 1);
        };
        [&]<size_t ...Is>(std::index_sequence<Is...>){
            (inner_printer(std::integral_constant<size_t, Is>{}, std::get<Is>(members_name), std::get<Is>(members)), ...);
        }(std::make_index_sequence<mc>{});
        policy.write(Lit::close_brace);
    } else {
        policy.write(Lit::lt);
        auto type_name = get_type_name<Decay_Obj>();
        policy.write(string_type(type_name.begin(), type_name.end()));
        policy.write(Lit::at);
        policy.write(to_basic_string<CharT>(static_cast<const void*>(&obj)));
        policy.write(Lit::gt);
    }
}

}
/////////////////////// PRINTER CLASS //////////////////////////////////////

template <PrintPolicy Policy>
class Printer {
    Policy policy_;
public:
    using char_type = typename Policy::char_type;
    using string_type = std::basic_string<char_type>;
    using string_view_type = std::basic_string_view<char_type>;
    using Lit = literals<char_type>;

    constexpr Printer() requires std::default_initializable<Policy> = default;
    constexpr Printer(Policy policy) : policy_(std::move(policy)) {}

    // Access policy
    Policy& policy() { return policy_; }
    const Policy& policy() const { return policy_; }

    // Single object print
    template <typename Obj>
    void print(Obj&& obj) {
        _inner::_print_impl(policy_, std::forward<Obj>(obj));
        if constexpr (FlushablePolicy<Policy>) {
            policy_.flush();
        }
    }

    template <typename Obj>
    void println(Obj&& obj) {
        _inner::_print_impl(policy_, std::forward<Obj>(obj));
        policy_.write(Lit::newline);
        if constexpr (FlushablePolicy<Policy>) {
            policy_.flush();
        }
    }

    // Multiple arguments print
    template <typename... Args>
    void print(Args&&... args) {
        (_inner::_print_impl(policy_, std::forward<Args>(args)), ...);
        if constexpr (FlushablePolicy<Policy>) {
            policy_.flush();
        }
    }

    template <typename... Args>
    void println(Args&&... args) {
        (_inner::_print_impl(policy_, std::forward<Args>(args)), ...);
        policy_.write(Lit::newline);
        if constexpr (FlushablePolicy<Policy>) {
            policy_.flush();
        }
    }

    // // Empty println
    // void println() {
    //     policy_.write(Lit::newline);
    //     if constexpr (FlushablePolicy<Policy>) {
    //         policy_.flush();
    //     }
    // }

    // Type name print
    template <typename TypeName>
    void print() {
        auto type_name = _inner::get_type_name<std::remove_reference_t<TypeName>>();
        policy_.write(string_type(type_name.begin(), type_name.end()));
        if constexpr (FlushablePolicy<Policy>) {
            policy_.flush();
        }
    }

    template <typename TypeName>
    void println() {
        auto type_name = _inner::get_type_name<std::remove_reference_t<TypeName>>();
        policy_.write(string_type(type_name.begin(), type_name.end()));
        policy_.write(Lit::newline);
        if constexpr (FlushablePolicy<Policy>) {
            policy_.flush();
        }
    }

    // Convert to string without output
    template <typename Obj>
    string_type to_string(Obj&& obj) {
        string_type result;
        struct string_policy {
            using char_type = Printer::char_type;
            string_type* str;
            void write(string_view_type sv) { str->append(sv); }
        };
        string_policy sp{&result};
        _inner::_print_impl(sp, std::forward<Obj>(obj));
        return result;
    }
};

// Factory functions
template <PrintPolicy Policy>
constexpr auto make_printer(Policy&& policy) {
    return Printer<std::decay_t<Policy>>(std::forward<Policy>(policy));
}

template <typename CharT>
auto make_ostream_printer(std::basic_ostream<CharT>& os) {
    return Printer<ostream_policy<CharT>>(ostream_policy<CharT>(os));
}

// Public API: get type name as string_view
template <class T>
constexpr std::string_view type_name() {
    return _inner::get_type_name<std::remove_cvref_t<T>>();
}




}
/////////////////////// GLOBAL INSTANCE ////////////////////////////////////

inline ju::Printer<cout_policy> jo;


/////////////////////// MACROS /////////////////////////////////////////////

#define ju_tostring(x) #x
#define ju_dbg(e) jo.println(ju_tostring(e), ": ", e)
#define ju_dbg_with(printer, e) (printer).println(ju_tostring(e), ": ", e)

#endif //EPRINT_HPP

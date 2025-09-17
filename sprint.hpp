//
// Created by jujin on 25-9-9.
//

#ifndef SPRINT_HPP
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

/////////////////////// CONCEPT ////////////////////////////////////////////

namespace Concept {
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
}

}


/////////////////////// CONCEPT ////////////////////////////////////////////


/////////////////////// STD TYPE SPECIALIZATION ////////////////////////////
template<typename T1, typename T2>
struct std::formatter<std::pair<T1, T2>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }
    auto format(const std::pair<T1, T2>& pair, std::format_context& ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "({}, {})", pair.first, pair.second);
        return out;
    }
};

template <typename ...Args>
struct std::formatter<std::tuple<Args...>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }
    static void for_each(auto&& fn, auto... args) {
        (fn(args), ...);
    }
    auto format(const std::tuple<Args...>& tuple, std::format_context& ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "(");
        bool is_first = true;
        std::apply([&]<typename... Args_>(Args_&&... args) {
            for_each([&]<typename T>(T&& ele) {
                if (is_first) {
                    out = std::format_to(out, "{}", std::forward<T>(ele));
                    is_first = false;
                } else {
                    out = std::format_to(out, ", {}", std::forward<T>(ele));
                }
            }, std::forward<Args_>(args)...);
        } , tuple);
        out = std::format_to(out, ")");
        return out;
    }
};


template <typename T>
struct std::formatter<std::optional<T>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }
    auto format(const std::optional<T>& t, std::format_context& ctx) const {
        auto out = ctx.out();
        if (t.has_value()) {
            out = std::format_to(out, "{}", t.value());
        } else {
            out = std::format_to(out, "None");
        }
        return out;
    }
};

template <>
struct std::formatter<std::filesystem::path> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }
    auto format(const std::filesystem::path& path, std::format_context& ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "{}", path.string());
        return out;
    }
};

template <typename T>
struct std::formatter<std::shared_ptr<T>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }
    auto format(const std::shared_ptr<T>& t, std::format_context& ctx) const {
        auto out = ctx.out();
        // out = std::format_to(out, "shared_ptr {{ address: {}, count: {} }}", static_cast<void*>(t.get()), t.use_count());
        out = std::format_to(out, "{{ address: {}, count: {} }}", static_cast<void*>(t.get()), t.use_count());
        return out;
    }
};

template <typename T>
struct std::formatter<std::unique_ptr<T>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }
    auto format(const std::unique_ptr<T>& t, std::format_context& ctx) const {
        auto out = ctx.out();
        // out = std::format_to(out, "shared_ptr {{ address: {}, count: {} }}", static_cast<void*>(t.get()), t.use_count());
        out = std::format_to(out, "{{ address: {} }}", static_cast<void*>(t.get()));
        return out;
    }
};

template <typename T>
struct std::formatter<std::complex<T>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }
    auto format(const std::complex<T>& t, std::format_context& ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "({}, {})", t.real(), t.imag());
        return out;
    }
};

// chrono has already been able to format by std formatter

/////////////////////// STD TYPE SPECIALIZATION ////////////////////////////

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


template <typename Obj>
void _print(std::ostream& os, Obj&& obj, size_t depth = 0) {
    using Decay_Obj = std::decay_t<Obj>;

    if constexpr (Concept::std_t::is_instance_of<Decay_Obj, std::shared_ptr>::value) {
        os << "{ address: " << static_cast<void*>(obj.get())  << ", count: " << obj.use_count() << " }";
    } else if constexpr (Concept::std_t::is_instance_of<Decay_Obj, std::unique_ptr>::value) {
        os << "{ address: " << static_cast<void*>(obj.get()) << " }";
    } else if constexpr (requires { os << std::forward<Obj>(obj); }) {
        if constexpr (Concept::string_like<Obj>) {
            if (depth != 0) {
                os << std::format("\"{}\"", std::forward<Obj>(obj));
                return;
            }
        }
        os << std::forward<Obj>(obj);
    } else if constexpr (requires { std::forward<Obj>(obj).to_string(); os << std::forward<Obj>(obj).to_string(); }) {
        os << std::forward<Obj>(obj).to_string();
    }
    else if constexpr (Concept::std_t::is_map<Decay_Obj>) {
        os << "{ ";
        bool is_first = true;
        for (auto&& pair : std::forward<Obj>(obj)) {
            if (!is_first) {
                os << ", ";
            }
            is_first = false;

            _print(os, pair.first, depth + 1);
            os << ": ";
            _print(os, pair.second, depth + 1);
        }
        os << " }";
    } else if constexpr (std::ranges::range<Obj>) {
        os << '[';
        bool first = true;
        for (auto&& e : std::forward<Obj>(obj)) {
            if (!first) os << ", ";
            first = false;
            _print(os,  e, depth + 1);
        }
        os << ']';
    } else if constexpr (std::convertible_to<Obj, std::string>) {
        os << static_cast<std::string>(std::forward<Obj>(obj));
    } else if constexpr (Concept::std_t::is_instance_of<Decay_Obj, std::pair>::value) {
        os << '(';
        _print(os, obj.first, depth + 1);
        os << ", ";
        _print(os, obj.second, depth + 1);
        os << ')';
    } else if constexpr (Concept::std_t::is_instance_of<Decay_Obj, std::tuple>::value) {
        auto  for_each = [](auto&& fn, auto... args) {
            (fn(args), ...);
        };
        os << '(';
        bool is_first = true;
        std::apply([&]<typename... Args_>(Args_&&... args) {
            for_each([&]<typename T>(T&& ele) {
                if (is_first) {
                    _print(os, std::forward<T>(ele), depth + 1);
                    is_first = false;
                } else {
                    os << ", ";
                    _print(os, std::forward<T>(ele), depth + 1);
                }
            }, std::forward<Args_>(args)...);
        } , obj);
        os << ')';

    } else if constexpr (Concept::std_t::is_instance_of<Decay_Obj, std::optional>::value) {
        if (obj.has_value()) {
            _print(os, obj.value(), depth); // optional<string> don't need to be wrapped by qm;
        } else {
            os << "None";
        }
    } else if constexpr (std::same_as<Decay_Obj, std::filesystem::path>) {
      os << obj.string();
    } else if constexpr (Concept::std_t::is_instance_of<Decay_Obj, std::complex>::value) {
        _print(os, std::pair{obj.real(), obj.imag()}, depth + 1);
    } // std::shared_ptr and std::unique_ptr was printed at first, chrono type was printed at the first os << ()
    else if constexpr (std::is_aggregate_v<Decay_Obj>) {
        using type = Decay_Obj;
        auto members = object_to_tuple(obj);
        auto members_name = get_member_names<type>();
        if (depth != 0) {
            os << "{ ";
        } else {    // TODO determined by Configer
            os << get_type_name<type>() << " { ";
        }
        constexpr auto members_count = members_count_v<type>;
        auto inner_printer = [&]<size_t Is>(std::integral_constant<size_t, Is>, auto member_name, auto&& member_value) {
            if constexpr (Is != 0) {
                os << ", ";
            }
            os << member_name << ": ";
            _print(os, std::forward<decltype(member_value)>(member_value), depth + 1);;
        };
        [&]<size_t ...Is>(std::index_sequence<Is...>){
            (inner_printer(std::integral_constant<size_t, Is>{}, std::get<Is>(members_name) , std::get<Is>(members)), ...);
        }(std::make_index_sequence<members_count>{});
        os << " }";
    }
    else {
        os << "<obj at " << static_cast<const volatile void*>(&obj) << '>';
    }
}

template <typename Obj>
void print(Obj&& obj) {
    _print(std::cout, std::forward<Obj>(obj));
    std::cout.flush();
}
template <typename Obj>
void println(Obj&& obj) {
    _print(std::cout, std::forward<Obj>(obj));
    std::cout << '\n';
    std::cout.flush();
}

template <typename ...Args>
void print(Args&&... args) {
    (print(std::forward<Args>(args)), ...);
}
template <typename ...Args>
void println(Args&&... args) {
    (print(std::forward<Args>(args)), ...);
    std::cout << '\n';
}


template <typename TypeName>
void print() {
    _print(std::cout, get_type_name<std::remove_reference_t<TypeName>>());
    std::cout.flush();
}

template <typename TypeName>
void println() {
    _print(std::cout, get_type_name<std::remove_reference_t<TypeName>>());
    std::cout << '\n';
    std::cout.flush();
}

inline void println() {
    std::cout << '\n';
}
#define SPRINT_HPP

#endif //SPRINT_HPP

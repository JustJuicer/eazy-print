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
    concept _support_std_t =
        (any_of_container<T, std::pair, std::tuple, std::optional, std::shared_ptr, std::unique_ptr, std::complex> ||
            any_of<T, std::filesystem::path, std::chrono::seconds, std::chrono::microseconds, std::chrono::milliseconds, std::chrono::hours, std::chrono::minutes, std::chrono::seconds> );

    template <typename T>
    concept support_std_t = _support_std_t<std::decay_t<T>>;

    static_assert(support_std_t<std::tuple<int, long>>);
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
    } else if constexpr (std::ranges::range<Obj>) {
        os << '[';
        bool first = true;
        for (auto&& e : std::forward<Obj>(obj)) {
            if (!first) os << ", ";
            first = false;
            _print(os,  e, depth + 1);
        }
        os << ']';
    } else if constexpr (std::convertible_to<Decay_Obj, std::string>) {
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
    else {
        os << "<obj at " << static_cast<void*>(&obj) << '>';
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

#define SPRINT_HPP

#endif //SPRINT_HPP

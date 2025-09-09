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
            (std::is_pointer_v<T> && CharacterType<std::remove_pointer_t<T>>) ||
            // 2. 字符数组（如 char[10], const char[5]）
            (std::is_array_v<T> && CharacterType<std::remove_extent_t<T>>) ||
            // 3. std::basic_string
            IsBasicString<T> ||
            // 4. std::basic_string_view
            IsBasicStringView<T>;
        static_assert(stringlike::string_like<std::string&>);
        static_assert(stringlike::string_like<std::string_view>);
        static_assert(stringlike::string_like<char[3]>);
        static_assert(stringlike::string_like<char*>);
        static_assert(stringlike::string_like<char16_t*>);
        static_assert(stringlike::string_like<char8_t*>);
        static_assert(!stringlike::string_like<std::array<int, 5>>);
    }

    template <typename T>
    concept string_like = stringlike::string_like<T>;




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
void print(std::ostream& os, Obj&& obj) {
    if constexpr (requires { os << std::forward<Obj>(obj); }) {
        os << std::forward<Obj>(obj);
    } else if constexpr (requires { std::forward<Obj>(obj).to_string(); os << std::forward<Obj>(obj).to_string(); }) {
        os << std::forward<Obj>(obj).to_string();
    } else if constexpr (std::ranges::range<Obj>) {
        os << '[';
        bool first = true;
        for (auto&& e : std::forward<Obj>(obj)) {
            if (!first) os << ", ";
            first = false;
            print(os,  e);
        }
        os << ']';
    } else {
        os << std::format("{}", std::forward<Obj>(obj));
    }
}
#define SPRINT_HPP

#endif //SPRINT_HPP

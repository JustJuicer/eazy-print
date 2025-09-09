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
void print() {

}
#define SPRINT_HPP

#endif //SPRINT_HPP

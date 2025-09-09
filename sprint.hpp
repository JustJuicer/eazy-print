//
// Created by jujin on 25-9-9.
//

#ifndef SPRINT_HPP
#include <format>
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

/////////////////////// STD TYPE SPECIALIZATION ////////////////////////////
void print() {

}
#define SPRINT_HPP

#endif //SPRINT_HPP

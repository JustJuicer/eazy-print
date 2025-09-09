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


/////////////////////// STD TYPE SPECIALIZATION ////////////////////////////
void print() {

}
#define SPRINT_HPP

#endif //SPRINT_HPP

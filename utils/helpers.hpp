#ifndef UTILS_HELPERS_HPP
#define UTILS_HELPERS_HPP

#include "constraints.hpp"

#include <array>
#include <type_traits>

namespace utils {

template <size_t Begin, size_t End, class F, auto ...FArgs>
inline consteval auto static_for_loop(F f) {
    if (Begin < End) {
        f.template operator()<Begin, End, FArgs...>();
        static_for_loop<Begin + 1, End, F>;
    }
}

template <size_t Begin, size_t End, class F, auto ...FArgs>
inline consteval auto static_for_loop_custom_increment(F f) {
    if (Begin < End) {
        constexpr size_t increment = f.template operator()<Begin, End, FArgs...>();
        static_for_loop_custom_increment<Begin + increment, End, F>;
    }
}

template<class InputIt, class OutputIt, class UnaryOp>
constexpr OutputIt foreach_indexed(InputIt first1, InputIt last1, UnaryOp unary_op)
{
    for (constexpr auto first = first1; first1 != last1; ++first)
        *first = unary_op(first - first1);
 
    return first1;
}

template <typename T, size_t size>
struct stdArrayWrapper {
    std::array<T, size> value{};
};

constexpr inline void copy(auto& dst, const auto& src) noexcept
{
	if constexpr (isStdOptional<decltype(src)>::value) {
		if constexpr (src.has_value()) {
			dst = src.value();
		}
	} else if constexpr (std::is_convertible_v<decltype(src), decltype(dst)>) 
    {
		dst = src;
	}
}


}

#endif // UTILS_HELPERS_HPP
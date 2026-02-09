#ifndef UTILS_CONSTRAINTS_HPP
#define UTILS_CONSTRAINTS_HPP

namespace utils
{

template <typename T>
struct isStdArray : std::false_type {};

template <typename T, size_t size>
struct isStdArray<std::array<T, size>> : std::true_type {};

template <typename T>
struct isStdOptional : std::false_type {};

template <typename T>
struct isStdOptional<std::optional<T>> : std::true_type {};

}

#endif // UTILS_CONSTRAINTS_HPP
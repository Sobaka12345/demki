#pragma once

#include <boost/smart_ptr/local_shared_ptr.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

namespace utils {

template <typename T>
using SharedPtr = boost::shared_ptr<T>;

template <typename T>
using LocalSharedPtr = boost::local_shared_ptr<T>;

}

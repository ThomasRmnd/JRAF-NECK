#ifndef JRAFNECK_UTILS_TYPETRAITS_HPP_
#define JRAFNECK_UTILS_TYPETRAITS_HPP_

#include <type_traits>

namespace jraf {

template<typename _Tp>
struct add_identify { typedef _Tp type; };

template<typename _Tp>
struct add_cref { typedef const _Tp& type; };

template<typename _Tp>
struct add_cptr { typedef const _Tp* type; };

template <typename T>
struct remove_cvref : std::remove_cv<typename std::remove_reference<T>::type> {};

} // namespace jraf

#endif // JRAFNECK_UTILS_TYPETRAITS_HPP_
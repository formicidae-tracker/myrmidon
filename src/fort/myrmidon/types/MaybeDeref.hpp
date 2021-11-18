#pragma once

#include <memory>


namespace fort {
namespace myrmidon {

template <typename T>
static inline T & MaybeDeref(T & x) { return x;}
template <typename T>
static inline T & MaybeDeref(T* x) { return *x;}
template <typename T>
static inline T & MaybeDeref(std::shared_ptr<T> & x) { return *x;}
template <typename T>
static inline T & MaybeDeref(std::unique_ptr<T> & x) { return *x;}
template <typename T>
static inline T & MaybeDeref(const std::shared_ptr<T> & x) { return *x;}
template <typename T>
static inline T & MaybeDeref(const std::unique_ptr<T> & x) { return *x;}

template <typename T>
static inline const T & MaybeDeref(const T & x) { return x;}
template <typename T>
static inline const T & MaybeDeref(T const * x) { return *x;}
template <typename T>
static inline const T & MaybeDeref(std::shared_ptr<const T> & x) { return *x;}
template <typename T>
static inline const T & MaybeDeref(std::unique_ptr<const T> & x) { return *x;}
template <typename T>
static inline const T & MaybeDeref(const std::shared_ptr<const T> & x) { return *x;}
template <typename T>
static inline const T & MaybeDeref(const std::unique_ptr<const T> & x) { return *x;}

} // namespace myrmidon
} // namespace fort

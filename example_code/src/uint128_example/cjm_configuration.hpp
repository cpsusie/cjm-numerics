#ifndef CJM_CONFIGURATION_NUMERICS_HPP_
#define CJM_CONFIGURATION_NUMERICS_HPP_
#include <iostream>
#include <iomanip>
#include <string>
#include <string_view>
#include <sstream>
#include <stdexcept>
#include <exception>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <climits>
#include <bit>
#include <span>

#ifdef __clang__
#define CJM_DETECTED_CLANG
#endif
#ifdef __SIZEOF_INT128__
#define CJM_DETECTED_INTRINSIC_U128
#endif
#ifdef _MSC_VER
#define CJM_DETECTED_WINDOWS
#endif
#ifdef _M_X64
#define CJM_DETECTED_X64
#endif
#if defined (__GNUG__) || defined(__GNUC__)
#define CJM_DETECTED_GCC
#endif
#if defined(__ICL) || defined(_ICC)
#define CJM_DETECTED_INTEL_CLASSIC
#endif
#ifdef __INTEL_LLVM_COMPILER
#define CJM_DETECTED_INTEL_LLVM
#endif


#endif

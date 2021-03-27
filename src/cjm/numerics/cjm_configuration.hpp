// Copyright © 2020-2021 CJM Screws, LLC
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// CJM Screws, LLC is a Maryland Limited Liability Company.
// No copyright claimed to unmodified original work of others.
// The original, unmodified work of others, to the extent included in this library,
// is licensed to you under the same terms under which it was licensed to CJM Screws, LLC.
// For information about copyright and licensing of the original work of others,
// see Notices file in cjm/ folder.
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
#if defined (CJM_DETECTED_WINDOWS) && defined(CJM_DETECTED_X64) && !defined(CJM_DETECTED_GCC) && !defined(CJM_DETECTED_INTEL_CLASSIC) && !defined(CJM_DETECTED_INTEL_LLVM) && !defined(CJM_DETECTED_CLANG)
#define CJM_UDIV_INTRINSIC_AVAILABLE
#endif
#if defined (CJM_DETECTED_WINDOWS) && defined(CJM_DETECTED_X64) && !defined(CJM_DETECTED_GCC)
#define CJM_BASE_INTRINSICS_AVAILABLE
#endif
#if defined (CJM_DETECTED_WINDOWS) && defined(CJM_DETECTED_X64) && defined(CJM_DETECTED_CLANG) && defined(__ADX__) && !defined(CJM_NUMERICS_UINT128_INTEL_ADX)
#define CJM_NUMERICS_UINT128_INTEL_ADX
#endif

#if !defined(CJM_NUMERICS_UINT128_INTEL_ADX) && defined(CJM_DETECTED_WINDOWS) && defined(CJM_DETECTED_X64) && !defined(CJM_DETECTED_GCC) && defined(__AVX2__)
#define CJM_NUMERICS_UINT128_INTEL_ADX
#endif

#if defined (CJM_DETECTED_WINDOWS) && defined(CJM_DETECTED_X64) && defined(CJM_DETECTED_CLANG) && defined(__BMI2__) && !defined(CJM_NUMERICS_UINT128_INTEL_BMI2)
#define CJM_NUMERICS_UINT128_INTEL_BMI2
#endif

#if !defined(CJM_NUMERICS_UINT128_INTEL_BMI2) && defined(CJM_DETECTED_WINDOWS) && defined(CJM_DETECTED_X64) && !defined(CJM_DETECTED_GCC) && defined(__AVX2__)
#define CJM_NUMERICS_UINT128_INTEL_BMI2
#endif

#endif

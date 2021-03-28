# CJM Numerics: (a fixed precision numeric library for C++20)
Copyright © 2020-2021, CJM Screws, LLC
## Summary
  This **header-only** library for C++ 20 currently provides an **unsigned 128 bit integer** type suitable for direct use or for extending to make other fixed precision types such as signed two's complement integers, larger signed or unsigned integers, rational numbers or flexible units of measure.  The unsigned 128-bit integer is currently feature-complete and extensively tested. Planned future features include a two's complement signed version as well as larger fixed precision integers (256, 512, 1024 bit), up to any arbitrary power of 2 greater than 128.  The larger values will be defined via templates.   Also being considered are rational numbers and units of measure for distance, area and volume.  *Please note that this library **requires C++ 20**.*  **No effort will be made to support older versions of the C++ standard.**  *If a user does not want to benefit specifically from C++20 features, then other extended precision libraries (such as the ones we modeled our algorithms from) may be a better fit.*
  
## Quick Start (pre-release)

If you want to use this library now (in a well-tested, but unreleased state), the easiest way is to download the contents of the [src/][1] folder to your configured library include path.  Make sure your IDE settings or build system is set to C++ 20 or later, then attempt to compile the following program:  
```cpp
#include <iostream>
#include <iomanip>
#include <cjm/numerics/uint128.hpp>

int main()
{
	using uint128_t = cjm::numerics::uint128;
	using namespace cjm::numerics::literals;
	constexpr auto newl = '\n';
	constexpr uint128_t lhs = 123'456'789'012'345'678'901'234_u128;
	constexpr uint128_t rhs = 432'109'876'543'210'987'654'321_u128;
	std::cout << "Hello, uint128_t!" << newl;
	std::cout << "lhs [" << lhs << "] * rhs [" << rhs << "] == [" << (lhs * rhs) << "]." << newl;
	std::cout << "Goodbye!" << std::endl;
	return 0;
}
```

Compile and run the program.  The output should be: 

> Hello, uint128_t!  
lhs [123456789012345678901234] * rhs [432109876543210987654321] == [226810394222294446283585782734362836562].  
Goodbye!
  
There is also a dependency-free example code project found [here](https://github.com/cpsusie/cjm-numerics/tree/main/example_code/src/uint128_example) for you to review the functionality.  

When this project is released, I plan on including a [vcpkg][2] port for it. 
  
### Library Components

1. **Present**  

* *uint128* (in <cjm/numerics/uint128>)  

    * This is the unsigned 128 bit integer provided by the libary.  To use it correctly, you will need all the contents presently under [src/][1]. The only include you should need to use directly is `<cjm/numerics/uint128.hpp>`.  
    
    * The namespaces you may need to use directly are: 
        * `cjm::numerics` (for uint128)
        * `cjm::numerics::concepts` (to use the concepts provided to extend the library with templates)
        * `cjm::numerics::literals` (to use the _u128 literal)
    * Do not directly used any namespace called "internal".  
    * Also, *until release*, do not use anything in any namespace other than the three mentioned above (cjm::numerics, cjm::numerics::concepts, cjm::numerics::literals).  
        
2. **Future**    

We plan to always segregate uint128 from future additions in our release packages so it may be used independently.  It will serve as the primary building block for other types we include later.  We strongly plan on including:

1. *int128* (a signed, two's complement version of uint128).

2. *fixed_uint*

    A template class that can take a uint128 or another fixed_uint as its template argument to serve as its limb type.  We will provide aliases for 256, 512 and 1024 bit variants thereof.

3. *fixed_int*

    A signed, two's complement version of fixed_uint.
    
There are less firm plans to provide:

* *urational<unsigned_integer TRep>* an unsigned rational type backed by an unsigned integer (64-bit or uint128 or fixed_uint).

* *rational<unsigned_integer TRep>* a signed (sign and magnitude) version of urational (but it will use an unsigned integer as its internal representation type).

* *distance*: a unit of measure type modeled off of the standard library's `std::chrono::duration` type.

## Sources
  This library does not pretend to contain any non-trivial original algorithms, but rather attempts to package well-known algorithms successfully used by others in a new format with different design goals.  This project has drawn most heavily from the unsigned and signed 128-bit provided by abseil.io at [abseil's github repository.](https://github.com/abseil/abseil-cpp/blob/master/absl/numeric/int128.h#L100)  It also has considered and adopted some of the work of other multiprecision numeric libraries such as the one provided by [Boost][4], [calccrypto][5], [libdivide][6], and even [Clang's Compiler-Rt][7] itself.  Its design goals, however, are sufficiently different to make it a more useful option in certain scenarios.
  
## Features / Design Principles
1.  **Constexpr (nearly) All the Things**

All or nearly all of the operations provided are constexpr-enabled: only floating point conversions and string parsing are runtime only.  Construction, destruction, conversions, assignment, comparisons (including the spaceship operator, <=>) , +,-,*,/,%, all the bitshifts and bitwise operations are constexpr-enabled.

2.  **Full (constexpr!) Support for hexadecimal and decimal literals, including the separator "'"**

A constexpr literal operator is provided that allows for the full range of values for an unsigned 128 bit integer whether expressed as a decimal or hexadecimal value.  Errors in the literals are always caught at compile time.  

3.  **Use of C++20's std::is_constant_evaluated to provide more efficient code where possible at runtime.**

Before C++ 20, one often had to choose between constexpr support and the use of efficiency promoting tools like compiler intrinsics. With C++ 20's [std::is_constant_evaluated](https://en.cppreference.com/w/cpp/types/is_constant_evaluated) feature, one can detect whether a function or operator is being executed at compile time or runtime and choose the constexpr-friendly or runtime efficient version appropriately.

4.  **Use of C++20 Concepts to simplify templated functions and types and to describe the intended functionality of an unsigned 128 bit integer in code**

C++20 concepts make it easier to express expected compile-time interfaces in code.  They also make using features like SFINAE to properly choose a correct template function far more readable and accessible to programmers who are not template meta-programming gurus.  Finally and perhaps most importantly they make error messages easier to read than they were before.  This library makes every effort to constrain all template functions and classes intended for external use -- this both documents the nature of the types expected by the templates and makes it easier to understand error messages.

5. **Makes reasoanble efforts to exactly like a built-in type wherever possible.**

6. **If compiled in an environment that supplies a built-in unsigned 128 bit integer, uses that type to perform calculations and is bit-castable to and from that type.**

7. **Sane approach to exceptions.**

This library's author is solidly in the *exceptions are a beautiful thing* camp.  Most of the (calculation-based) operations provided, however, are marked *noexcept* not because of opposition to use of exceptions but, in the interest of efficiency, to avoid runtime checks.  This will result, in most cases, in undefined behavior under the same circumstances that a built-in integer would exhibit undefined behavior (e.g. shifting left or right by an amount greater than or equal to std::numeric_limits<UnsignedInteger>::digits).  One exception lies with division and modulus operations: these operations are expensive enough by themselves that eschewing a runtime check seems absurd.  

Nevertheless, alternatives are provided for performing **unchecked** division/modulus as well as *checked* division/modulus using `std::optional` and returning `std::nullopt` instead of throwing an exception where executing the operation would cause undefined behavior.  Similarly, checked (both the throwing and `std::optional` variety) alternatives are provided for other operations that can exhibit undefined behavior for invalid inputs.

Text related code (e.g. stream insertion and extraction) throws exceptions.  

8. **Stream insertion and extraction operators provided for *char* and *wchar_t* assuming use of standard English characters.  Experimental support is provided for *char8_t*, *char16_t* and *char32_t*.**

9. **The uint128 is [bit-castable](https://en.cppreference.com/w/cpp/numeric/bit_cast) provided the requirements for bitcastability are met.**  

Support is currently present for compilers / standard library implementations that do not yet support bitcasting.  In those cases, however, bitcasting is not constexpr even if it would be constexpr with compiler support.  When these compilers / standard libraries add bitcasting support, it should be a drop-in replacement. (The library-provided bitcast simply delegates to the standard library version if it is available.).  Bitcasting allows easy conversion to and from any type that follows the same bit pattern enabling use with other libraries.  Bitcastability requies:
*   The source and destination have the same size,
*   The source and destination both be [trivially copyable](https://en.cppreference.com/w/cpp/named_req/TriviallyCopyable), and
*   (if compiler support is not present), the destination type must be [trivially default constructible](https://en.cppreference.com/w/cpp/types/is_default_constructible).
    
10. **Uses unsigned 128-bit arithmetic provided by the compiler rather than using its own algorithms on compilers that provide an unsigned 128-bit integer for which they have supplied a specialization for std::numeric_limits.**  

This mostly applies to compilation with GCC or Clang targeting a x64 architecture on a Linux or Linux-like system.  Rather than use compiler intrinsics or fallback arithmetic (usable on any conforming compiler and always used in constexpr context), the compiler-provided arithmetic is used.  In this way, as the compiler becomes better at providing optimizations, users of this library will be able to take advantage of it.

11. **Uses x64 compiler intrinsics when compiled for Windows when:**
*   targeting an x64 Intel / AMD architecture and
*   using one of the following compilers targetting x64 Windows:
    * The MSVC Compiler,
    * The clang compiler (currently version 11) that is supplied with Visual Studio, or
    * Intel's llvm-based compiler.  
    
12. **Can use the ADX and BMI2 intrinsic versions for add with carry and extended precision multiplication when these are available.**  

When targetting 64-bit Windows using the MSVC, Clang 11 or Intel-LLVM compiler, you can specify use of adx and bmi2 instrinsics for [add with carry](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_addcarryx_u64) and [extended precision multiplication](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mulx_u64).

13. **Do not support standards earlier than C++20.** 

Looking through the source code of other libraries, much complexity results from attempting to take advantage of features from recent revisions of the C++ standard while providing backwards-compatibility with older versions.  If this libary is more successful than I imagine it will be, it may end up that we do the same thing.  Our cutoff point, however, is far more recent: **now**.  There is enough complexity currently in the code from supporting big and little endian systems, using compiler-supplied uint128 types where available, using compiler extensions and intrinsics where possible, etc.  This complexity, however, will be limited to the uint128 and core-arithmetic functionality, which all other additions will use.  Heavy reliance on std::is_constant_evaluated and a desire to avoid *further* complexity therefore suggest **now** as a reasonable cutoff point.  

14. **No external dependencies except for the standard library and optionally-used compiler intrinsics**

This project depends only on the C++ standard library and, when it detects certain known compilers and feature sets, compiler extensions and intrinsics.  There will be no external dependencies required to use this library. 

Please note that this library's *unit test* application **does** have external dependencies.  The *unit test* application, however, is not part of the library and is not required to use the library.

15. **Add possible support for C++20 Modules** 

Support for modules will not be attempted until at least GCC, Clang and Microsoft all provide first-class support for this feature.  Currently, only Microsoft provides such full support.  Modules will be added if it can be done feasibly without breaking the current, header-only, format option.  The header-only format option will continue to be supported at least until the next full revision of the C++ standard is published (expected in 2023). 

16. **The best way to document intended functionality is in code, via Concepts**

With C++ 20 the best way to document for other programmers the intended functionality of a type (especially one that might be used in a generic context) is via Concepts.  To that end, the intended functionality of uint128 is demonstrated by the [cjm_unsigned_interger][8] concept. 

## Compiler / Architecture Requirements

Please note that all the requirements listed below are expressed in code and can be found [here][9]. 

1. [**C++ 20 compliance**][10]  

This software requires a compiler that conforms, at least partially, to the C++ 20 ISO standard.  It requires that when you use it, you specify C++ 20 or later as the standard in the arguments you pass to your compiler.  More specifically, it requires *at least* the following C++ 20 features to be supported:

* [char8_t (core language feature)][11]
* [Concepts (core language feature)][12]
* [Three way comparison "spaceship" operator <=> (core language feature)][13]
* [Classes and Structs as non-type template parameters (core language feature)][14]
* [Bit operations (standard library feature)][15]
* [char8_t (standard library feature)][16]
* [Concepts (standard library feature)][17]
* [std::endian (standard library feature)][18]
* [std::is_constant_evaluated (core language and standard library feature)][19]
* [std::is_nothrow_convertible (standard library feature)][20]
* [std::remove_cvref (standard library feature)][21] 
* [std::span<T> (standard library feature)][22]
* [starts_with, ends_with for std::string_view (standard library feature)][23] 

2. **Other Requirements**

* [CHAR_BIT][24] must be 8.  That is, a byte must consist of eight bits.
* [`std::endian::native`][25] must equal [`std::endian::big`][26] or [`std::endian::little`][27].  That is, the system must be big-endian or little endian, excluding mixed-endian or other edge-case platforms.
* All of the following integer types must be defined have the matching size and [`std::numeric_limits<T>::digits`][28]:
    1. std::uint8_t     size: 1,    digits: 8
    2. std::int8_t      size: 1,    digits: 7
    3. std::uint16_t    size: 2,    digits: 16
    4. std::int16_t     size: 2,    digits: 15
    5. std::uint32_t    size: 4,    digits: 32
    6. std::int32_t     size: 4,    digits: 31
    7. std::uint64_t    size: 8,    digits: 64
    8. std::int64_t     size: 8,    digits: 63

## Testing

This project (given its one-person authorship and the author's fulltime employment as a software engineer working on unrelated proprietary software) has been tested extensively.  A unit test application is included in its repository and this test must be run and pass on multiple compilers and operating systems and architectured before changes are merged into its main branch.  Also, an example code project is included in this repository: this project must build on even more compilers and run without error before code changes are merged into the main branch.  

### **Unit Test Application**

The [unit test application][29] is the one application in this repository that has external dependencies.  Those dependencies include:

1. [abseil-C++17][30] (vcpkg: abseil[cxx17])
2. [boost][31] version 1.73+ (vcpkg: boost)
3. [hinnant/date][32] 3.0.0+ (vcpkg: date)

The unit test application must be built and run in Visual Studio 2019 with the dependencies installed as static libraries with a static c-runtime via the following vcpkg triplets: `x86-windows-static` and `x64-windows-static`.

It also be built and run in a 64-bit Linux environment, Kubuntu 20.1, using the following tripet: `x64-linux`.  It is then built using CMAKE in the Clion IDE and linked against libstdc++.  Please note that libc++ (clang's implementation of the C++ standard library) will not currently work with this project because it does not yet provide full standard library support for Concepts.  The following compilers are used to build the unit test project and run it:   

* clang++ 
> Ubuntu clang version 11.0.0-2  
Target: x86_64-pc-linux-gnu  
Thread model: posix  
InstalledDir: /usr/bin  
Found candidate GCC installation: /usr/bin/../lib/gcc/x86_64-linux-gnu/10  
Found candidate GCC installation: /usr/lib/gcc/x86_64-linux-gnu/10  
Selected GCC installation: /usr/bin/../lib/gcc/x86_64-linux-gnu/10  
Candidate multilib: .;@m64  
Selected multilib: .;@m64  

* g++
> Using built-in specs.  
> COLLECT_GCC=g++  
> COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-linux-gnu/10/lto-wrapper  
> OFFLOAD_TARGET_NAMES=nvptx-none:amdgcn-amdhsa:hsa  
> OFFLOAD_TARGET_DEFAULT=1  
> Target: x86_64-linux-gnu  
> Configured with: ../src/configure -v --with-pkgversion='Ubuntu 10.2.0-13ubuntu1'  
> --with-bugurl=file:///usr/share/doc/gcc-10/README.Bugs  
> --enable-languages=c,ada,c++,go,brig,d,fortran,objc,obj-c++,m2   
> --prefix=/usr  
> --with-gcc-major-version-only  
> --program-suffix=-10  
> --program-prefix=x86_64-linux-gnu-   
> --enable-shared   
> --enable-linker-build-id  
> --libexecdir=/usr/lib  
> --without-included-gettext  
> --enable-threads=posix  
> --libdir=/usr/lib
> --enable-nls  
> --enable-clocale=gnu  
> --enable-libstdcxx-debug  
> --enable-libstdcxx-time=yes  
> --with-default-libstdcxx-abi=new  
> --enable-gnu-unique-object  
> --disable-vtable-verify  
> --enable-plugin  
> --enable-default-pie  
> --with-system-zlib  
> --enable-libphobos-checking=release  
> --with-target-system-zlib=auto  
> --enable-objc-gc=auto  
> --enable-multiarch  
> --disable-werror
> --with-arch-32=i686  
> --with-abi=m64   
> --with-multilib-list=m32,m64,mx32  
> --enable-multilib 
> --with-tune=generic  
> --enable-offload-targets=nvptx-none=/build/gcc-10-JvwpWM/gcc-10-10.2.0/debian/tmp-nvptx/usr,amdgcn-amdhsa=/build/gcc-10-JvwpWM/gcc-10-10.2.0/debian/tmp-gcn/usr,hsa  
> --without-cuda-driver   
> --enable-checking=release   
> --build=x86_64-linux-gnu  
> --host=x86_64-linux-gnu  
> --target=x86_64-linux-gnu  
> Thread model: posix  
> Supported LTO compression algorithms: zlib zstd
> gcc version 10.2.0 (Ubuntu 10.2.0-13ubuntu1) 


The CMakeLists.txt file used to build the unit test project (and example project) on linux with both g++ and clang can be found [here][33].


If you download the main branch, you can be assured that the unit test project was built successfully and passed in all of the above configurations, summarized:

*   Visual Studio v. 16.9, x64, static linking, static crt (debug and release)
*   Visual Studio v. 16.9, x86, static linking, static crt (debug and release)
*   Linux, x64, clang++ v. 11.0.2, libstdc++ (debug and release)
*   Linux, x64, g++ v. 10.0.2, libstdc++ (debug and release)

### **Example Code Application**

The [example code application][34] is designed to demonstrate the functionality of the library using code.  Unlike the unit test application, it does not have any external dependencies.  In addition to requiring that the unit test application build and passes under all the configurations listed above, it is also required that the example code application build and execute without errors.  In addition to the configurations used by the unit test application, the following extra compilers/configurations must be verified to build the example code application such that it runs successfully:

Microsoft Windows 10 Target, (using Microsoft's standard library):

* Clang v. 11, as supplied by Microsoft with Visual Studio (x86, x64, Debug and Release)
* Intel's LLVM-based compiler v. 2021.1.2 (x86, x64, Debug and Release)  

Currently, Intel's classic C++ compiler does not support C++ 20 at all.  If it ever supports C++ 20 sufficiently to be compatible with this library, it will be added to the list immediately above.
  
### **Note on Big Endian Systems**  

This library was designed to work equally well on big and little endian systems.  The author, however, has no reasonable way to test on a big endian system.  If you encounter issues on big endian systems, please report them.  

## Legal  
Copyright © 2020-2021 CJM Screws, LLC
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:  
  
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.  
  
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

CJM Screws, LLC is a Maryland Limited Liability Company.
No copyright claimed to unmodified original work of others. The original, unmodified work of others, to the extent included in this library, is licensed to you under the same terms under which it was licensed to CJM Screws, LLC. For information about copyright and licensing of the original work of others, see [Notices][35] file in cjm/ folder.

  [1]: https://github.com/cpsusie/cjm-numerics/tree/main/src/
  [2]: https://github.com/microsoft/vcpkg
  [3]: https://github.com/cpsusie/cjm-numerics/tree/main/src/
  [4]: https://www.boost.org/doc/libs/1_75_0/libs/multiprecision/doc/html/boost_multiprecision/tut/ints/cpp_int.html
  [5]: https://github.com/calccrypto/uint128_t
  [6]: https://github.com/ridiculousfish/libdivide
  [7]: https://github.com/llvm/llvm-project/blob/938d05b814c7fe470201d595afefc02e3371244e/compiler-rt/lib/builtins/udivmodti4.c#L84
  [8]: https://github.com/cpsusie/cjm-numerics/blob/main/src/cjm/numerics/cjm_numeric_concepts.hpp#L321
  [9]: https://github.com/cpsusie/cjm-numerics/blob/main/src/cjm/numerics/numerics_configuration.hpp
  [10]: https://en.cppreference.com/w/cpp/compiler_support
  [11]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0482r6.html
  [12]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0734r0.pdf
  [13]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0515r3.pdf
  [14]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0732r2.pdf
  [15]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0553r4.html
  [16]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0482r6.html
  [17]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0898r3.pdf
  [18]: https://wg21.link/P0463R1
  [19]: https://wg21.link/P0595R2
  [20]: https://wg21.link/P0758R1
  [21]: https://wg21.link/P0550R2
  [22]: https://wg21.link/P0122R7
  [23]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0758r1.html
  [24]: https://en.cppreference.com/w/cpp/header/climits
  [25]: https://en.cppreference.com/w/cpp/types/endian
  [26]: https://en.cppreference.com/w/cpp/types/endian
  [27]: https://en.cppreference.com/w/cpp/types/endian
  [28]: https://en.cppreference.com/w/cpp/types/numeric_limits/digits
  [29]: https://github.com/cpsusie/cjm-numerics/tree/main/uint128_test_app/src
  [30]: https://github.com/abseil/abseil-cpp/tree/master/absl
  [31]: https://github.com/boostorg/boost
  [32]: https://github.com/HowardHinnant/date
  [33]: https://github.com/cpsusie/cjm-numerics/blob/main/CMakeLists.txt
  [34]: https://github.com/cpsusie/cjm-numerics/blob/main/example_code/src/uint128_example/program.cpp
  [35]: https://github.com/cpsusie/cjm-numerics/blob/main/src/NOTICES.md

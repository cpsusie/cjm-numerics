# CJM Numerics: (a fixed precision numeric library for C++20)
## Summary
  This **header-only** library for C++ 20 currently provides an unsigned 128 bit integer type suitable for direct use or for extending to make other fixed precision types such as signed two's complement integers, larger signed or unsigned integers, rational numbers or flexible units of measure.  The unsigned 128-bit integer is currently feature-complete and extensively tested. Planned future features include a two's complement signed version as well as larger fixed precision integers (256, 512, 1024 bit), up to any arbitrary power of 2 greater than 128.  The larger values will be defined via templates.   Also being considered are rational numbers and units of measure for distance, area and volume.  *Please note that this library **requires C++ 20**.*  **No effort will be made to support older versions of the C++ standard.**
  
## Sources
  This library is derivative of the unsigned and signed 128-bit provided by abseil.io at [abseil's github repository.](https://github.com/abseil/abseil-cpp/blob/master/absl/numeric/int128.h#L100)  It also has considered and adopted some of the work of other multiprecision numeric libraries such as the one provided by Boost, calcrypto, and even LLVM itself.  Its design goals, however, are sufficiently different to make it a more useful option in certain scenarios.
  
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
*   using the MSVC compiler or
    * The clang compiler (currently version 11) that is supplied with Visual Studio or
    * Intel's llvm-based compiler.
    
12. Will use the ADX and BMI2 intrinsic versions for add with carry and multiplication when these are available.




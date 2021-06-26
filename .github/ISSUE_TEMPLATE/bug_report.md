---
name: Bug report
about: Create a report to help us improve
title: 'Bug Report for uint128'
labels: ''
assignees: ''

---

**Describe the bug**
A clear and concise description of what the bug is.

**To Reproduce**
Steps to reproduce the behavior:
1. Provide the operation that performed incorrectly.  (e.g: uint128_t y = 1_u128 + 1_u128).
2. Provide the expected result (e.g.: y == 2_u128)
3. Provide the actual result (e.g. y == 3_u128)
4. Indicate whether (if known) the incorrect behavior happened at compile time or runtime.

**Expected behavior**
A clear and concise description of what you expected to happen.

**Desktop (please complete the following information):**
 - OS: [Windows 10, Kubuntu 20.10, FreeBSD x.y ]
 - Compiler (MSVC, Intel-LLVM, MSVC-Clang, Clang++(gcc, linux), G++(linux)
 - Target Architecture (ARM64, Intel/AMD x64, etc)
 - If Windows whether any special calculation modes used (AVX2, BMI2, ADX)
 -  If available, output from running the -environ switch of WinInt128.exe such as:
 
```plaintext
ENVIRONMENT DATA:
Detected compiler: [Microsoft].
SSE3: [NOT AVAILABLE].
Intel ADX: [AVAILABLE].
Intel BMI2: [AVAILABLE].
Execution Cores for concurrency: [20].

PRINTING SIZES:
        CHAR_BIT: 8 bits per byte on this system.
        Sizeof unsigned short: [2].
        Sizeof unsigned int: [4].
        Sizeof unsigned long: [4].
        Sizeof unsigned long long: [8].
        Sizeof size_t: [8].
        Sizeof uintmax_t: [8].
        unsigned 128 bit integer unavailable in this environment.
        char is signed in this implementation.
        Sizeof wide character: [2].
        Wide character is unsigned in this implementation.
        Sizeof cjm::numerics::uint128: [16].
        Sizeof float: [4].
        Sizeof double: [8].
        Sizeof long double: [8].
        Sizeof pointer to object: [8].
        Sizeof most aligned built-in: [8].
DONE PRINTING SIZES

PRINTING ALIGNMENTS:
        Alignof unsigned short: [2].
        Alignof unsigned int: [4].
        Alignof unsigned long: [4].
        Alignof unsigned long long: [8].
        Alignof size_t: [8].
        Alignof uintmax_t: [8].
        unsigned 128 bit integer unavailable in this environment.
        Alignof char: [1].
        Alignof wide character: [2].
        Alignof cjm::numerics::uint128: [16].
        Alignof float: [4].
        Alignof double: [8].
        Alignof long double: [8].
        Alignof pointer to object: [8].
        Alignof most aligned built-in: [8].
DONE PRINTING ALIGNMENTS

Printing data for type float:
        float (size: 4, align: 4) is an iec559 type.
        float uses 23 of its 32 bits for mantissa.
        float's min exponent: -125; max exponent: 128.
        float's max value: 3.40282e+38.
Done printing data for type float.

Printing data for type double:
        double (size: 8, align: 8) is an iec559 type.
        double uses 52 of its 64 bits for mantissa.
        double's min exponent: -1021; max exponent: 1024.
        double's max value: 1.79769e+308.
Done printing data for type double.

Printing data for type long double:
        long double (size: 8, align: 8) is an iec559 type.
        long double uses 52 of its 64 bits for mantissa.
        long double's min exponent: -1021; max exponent: 1024.
        long double's max value: 1.79769e+308.
Done printing data for type long double.

Done printing floating point information.

Is constexpr bitcasting available?: [YES].
Arithmetic mode: [Microsoft x64 Compiler-Intrinsic Optimized Arithmetic].
Are C++20 bit operations available?: [YES].
No built-in  uint128 data available.
Is consteval (immediate function) available?: [true].
END ENVIRONMENT DATA

Done executing switch [environ]
```



**Additional context**
Add any other context about the problem here.

# Unit Test Application for CJM Numerics (a fixed precision a fixed precision numeric library for C++20)  
**Copyright © 2021, CJM Screws, LLC** 

## Summary  

The unit test application is a command line application that runs a suite of unit tests to demonstrate the correctness of operations on the uint128 type defined in the CJM Numerics library.  The primary focus and goal of the tests is to ensure the correctness of the numerical computations provided by the library at both compile and runtime.  Compile-time correctness is checked during the compilation of the unit test application.  The application needs to be run to apply runtime testing.  
  
A Secondary goal of the unit test application include testing the correctness of to-string conversion and from-string parsing, particularly for std::string and std::wstring (and their matching streams and string_views).  Some experimental support is included for using strings (and streams/string_views) based on char8_t, char16_t and char32_t: these seem to work on Windows, but are iffy on linux setups.  Supporting the newer string types is a tertiary goal of the test application, at best.

Another secondary goal of the unit test application is to provide a way to print out information about the target environment including compiler used, 64 or 32 bit, whether adx and bmi2 are enabled, sizes and alignments of standard numeric types, etc.  
  
## Functionality  

### **RUNNING ENTIRE BATTERY OF TESTS.**  
  
To run the entire battery of tests, simply run the application from the command line without using any options:  
    
```dos
E:\Source\Repos\cjm-numerics\uint128_test_app\src\x64\Release>WinInt128.exe
Using num threads: [12].
Beginning execution of test [unary_op_basic_test].
Test [unary_op_basic_test], PASSED!

Beginning execution of test [test_convert_to_double].
Test [test_convert_to_double], PASSED!

Beginning execution of test [failing_division_test_1].
Test [failing_division_test_1], PASSED!

Beginning execution of test [div_mod_zero_dividend_nonzero_divisor_tests].
Test [div_mod_zero_dividend_nonzero_divisor_tests], PASSED!

Beginning execution of test [literal_test].
Test [literal_test], PASSED!

Beginning execution of test [failing_division_test_2].
Test [failing_division_test_2], PASSED!

Beginning execution of test [umult_spec_tests].
Test [umult_spec_tests], PASSED!

Beginning execution of test [print_generated_filename_test].
Test [print_generated_filename_test], PASSED!

Beginning execution of test [controlled_float_rt_conversion_test].
Test [controlled_float_rt_conversion_test], PASSED!

Beginning execution of test [unary_op_bool_cast_test].
Test [unary_op_bool_cast_test], PASSED!

Beginning execution of test [unary_op_bitwise_not_test].
Test [unary_op_bitwise_not_test], PASSED!

Beginning execution of test [unary_op_logical_negation_test].
Test [unary_op_logical_negation_test], PASSED!

Beginning execution of test [unary_op_unary_minus_test].
Test [unary_op_unary_minus_test], PASSED!

Beginning execution of test [unary_op_post_dec_test].
Test [unary_op_post_dec_test], PASSED!

Beginning execution of test [unary_op_unary_plus_test].
Test [unary_op_unary_plus_test], PASSED!

Beginning execution of test [multiplication_tests].
Test [multiplication_tests], PASSED!

Beginning execution of test [builtin_u128fls_test_if_avail].
Test [builtin_u128fls_test_if_avail], PASSED!

Beginning execution of test [throwing_float_conversion_test].
Test [throwing_float_conversion_test], PASSED!

Beginning execution of test [division_modulus_tests].
Test [division_modulus_tests], PASSED!

Beginning execution of test [unary_op_post_inc_test].
Test [unary_op_post_inc_test], PASSED!

Beginning execution of test [subtraction_tests].
Test [subtraction_tests], PASSED!

Beginning execution of test [gen_comp_ops_test].
Test [gen_comp_ops_test], PASSED!

Beginning execution of test [safe_float_conversions_test].
Test [safe_float_conversions_test], PASSED!

Beginning execution of test [unary_op_pre_inc_test].
Test [unary_op_pre_inc_test], PASSED!

Beginning execution of test [uintcontainer_adc_tests].
Test [uintcontainer_adc_tests], PASSED!

Beginning execution of test [comparison_tests].
Test [comparison_tests], PASSED!

Beginning execution of test [builtin_sub_with_borrow_test].
Test [builtin_sub_with_borrow_test], PASSED!

Beginning execution of test [basic_u128_adc_test].
Test [basic_u128_adc_test], PASSED!

Beginning execution of test [div_mod_zero_tests].
Test [div_mod_zero_tests], PASSED!

Beginning execution of test [first_bin_op_test].
Test [first_bin_op_test], PASSED!

Beginning execution of test [builtin_add_with_carry_test].
Test [builtin_add_with_carry_test], PASSED!

Beginning execution of test [unary_op_pre_dec_test].
Test [unary_op_pre_dec_test], PASSED!

Beginning execution of test [basic_u128_sbb_test].
Test [basic_u128_sbb_test], PASSED!

Beginning execution of test [addition_tests].
Test [addition_tests], PASSED!

Beginning execution of test [shift_tests].
Test [shift_tests], PASSED!

Beginning execution of test [bw_tests].
Test [bw_tests], PASSED!

Beginning execution of test [ascii_char_interconversions].
Test [ascii_char_interconversions], PASSED!

Beginning execution of test [unary_operation_rt_serialization_tests].
Test [unary_operation_rt_serialization_tests], PASSED!

Beginning execution of test [failing_modulus_test_1].
Test [failing_modulus_test_1], PASSED!

Beginning execution of test [trim_tests].
Test [trim_tests], PASSED!

Beginning execution of test [basic_test_one].
Test [basic_test_one], PASSED!

Beginning execution of test [test_convert_to_long_double].
Test [test_convert_to_long_double], PASSED!

Beginning execution of test [binary_operation_rt_ser_tests].
Test [binary_operation_rt_ser_tests], PASSED!

Beginning execution of test [unary_op_post_stat_assert_test].
Test [unary_op_post_stat_assert_test], PASSED!

Beginning execution of test [issue_10_showbase_test].
Test [issue_10_showbase_test], PASSED!

Beginning execution of test [basic_multiplication_test].
Test [basic_multiplication_test], PASSED!

Beginning execution of test [test_fls].
Test [test_fls], PASSED!

Beginning execution of test [unary_op_code_rt_serialization_tests].
Test [unary_op_code_rt_serialization_tests], PASSED!

Beginning execution of test [issue_10_strm_insrt_test].
Test [issue_10_strm_insrt_test], PASSED!

Beginning execution of test [stream_insert_bin_op_test].
Test [stream_insert_bin_op_test], PASSED!

Beginning execution of test [test_convert_to_float].
Test [test_convert_to_float], PASSED!

Beginning execution of test [controlled_from_float_conversion_test].
Test [controlled_from_float_conversion_test], PASSED!

Beginning execution of test [string_parse_text].
Test [string_parse_text], PASSED!

Beginning execution of test [unary_operation_vec_rt_serialization_tests].
Test [unary_operation_vec_rt_serialization_tests], PASSED!

Beginning execution of test [hash_dx].
Test [hash_dx], PASSED!

All [55] tests PASSED.

Successfully wrote test result detail to: ["base_test_battery_results_2021_May_20_23_54_06Z.txt"].

E:\Source\Repos\cjm-numerics\uint128_test_app\src\x64\Release>
```  

```bash
csusie@ubuntu:~/CLionProjects/Int128/cmake-build-release-clang$ ./Int128
Using num threads: [6].
Beginning execution of test [unary_op_unary_minus_test].
Test [unary_op_unary_minus_test], PASSED!
.... (INTERMEDIATE TESTS OMMITED BECAUSE SAME AS WITH WINDOWS) ....
Beginning execution of test [hash_dx].
Test [hash_dx], PASSED!

All [55] tests PASSED.

Successfully wrote test result detail to: ["base_test_battery_results_2021_May_21_00_09_04Z.txt"].
```


As seen, the name and test result of each run test will be logged to the standard output.  More detailed results will be written to a text file whose name based on a root of "base_test_batter_results", a timestamp and the .txt extension.  Certain classes of failures (none shown) may be written to the standard error and/or the output file depending on their nature.  

NOTE: the test "hash_dx" takes a very long time (maybe 30 secondsish) on a *debug* build built with Microsoft's compiler.  For release builds, this test takes less than a second on my machine.  Using GCC or Clang in Linux, while a performance difference between *debug* and *release* exists, it is no where near as profound as the difference using Microsoft's compiler.  So, if testing with a debug buld made with Microsoft's compiler, expect it to take a while.  
  
### **USING SWITCHES**  

Besides its default behavior of running the entire battery of tests, the application supports switches to get help, print environmental information, list available tests, run a specific individual test from the list or to execute a unary or binary operation text file.  (Many tests, upon failure, will output a text file with the operation and values that resulted in failure.  You can also make your own fairly easily.)  To enter a switch, prefix its name with -- or - (all operating systems) or (Windows only) /.  For example, to get help, entering the switch --help (or any command line arguments not understood by the application) lists the switches as shown:    

*WINDOWS:*
```dos
E:\Source\Repos\cjm-numerics\uint128_test_app\src\x64\Release>WinInt128.exe brofklor
There was a problem, likely when processing command-line arguments:
        Argument: [brofklor] is not a switch:
        Switches begin with: "-", "--", or (on windows): "/"

Switches include:
        unary_from_file         Execute unary operations tests as specified in unary operations text file.
        binary_from_file        Execute binary operations tests as binary in unary operations text file.
        environ                 Print environmental information.
        default_tests           Run all tests in default battery, logging summary of result to standard output and detail to a log file.
        list                    List all tests in the default battery.
        specific_test           Run the specified test from the default battery.
        help                    Print this information.
Usage examples for all switches:
--unary_from_file                        my_unary_operations_file.txt
--binary_from_file                       my_binary_operations_file.txt
--environ
--default_tests
--list
--specific_test                          name_of_test_to_run
--help
DONE with usage examples.



E:\Source\Repos\cjm-numerics\uint128_test_app\src\x64\Release>
```
*LINUX:*  
```bash
csusie@ubuntu:~/CLionProjects/Int128/cmake-build-release-clang$ ./Int128 brofklor
There was a problem, likely when processing command-line arguments:
        Argument: [brofklor] is not a switch:
        Switches begin with: "-", "--", or (on windows): "/" 

Switches include: 
        unary_from_file         Execute unary operations tests as specified in unary operations text file.
        binary_from_file        Execute binary operations tests as binary in unary operations text file.
        environ                 Print environmental information.
        default_tests           Run all tests in default battery, logging summary of result to standard output and detail to a log file.
        list                    List all tests in the default battery.
        specific_test           Run the specified test from the default battery.
        help                    Print this information.
Usage examples for all switches: 
--unary_from_file                        my_unary_operations_file.txt
--binary_from_file                       my_binary_operations_file.txt
--environ 
--default_tests 
--list 
--specific_test                          name_of_test_to_run
--help 
DONE with usage examples.


csusie@ubuntu:~/CLionProjects/Int128/cmake-build-release-clang$
```
#### **unary_from_file switch**  
  
The "unary_from_file_switch" requires as an argument the path of an ASCII text file containing unary operations.  It will then execute the unary operations in the file and report on success or failure.  

An example of such a file can be found [here][1] and is shown below:

> !x;340282366920938463463374607431768211455;  
!x;340282366920938463463374607431768211454;  
~x;0;  
-x;1;  
static_cast<bool>(x);170141183460469231731687303715884105728;  
!x;9223372036854775808;  
+x;18446744073709551615;  
--x;340282366920938463463374607431768211454;  
++x;18446744073709551616;  

The symbols for each supported operation can be found in the [int128_tests.hpp][5] header in the array called [un_op_symbol_lookup][3].  A test is deemed to pass if, when a unary operation of the specified type is applied to a value of type uint128 and an equivalent value of type [`boost::multiprecision::uint128_t`][7] (from the Boost library), the resulting values are equivalent.  If the resulting values differ, the test is deemed to have failed.

**Usage Example**:  
  
*WINDOWS:*   
```dos
E:\Source\Repos\cjm-numerics\uint128_test_app\src\x64\Release>dir
 Volume in drive E is R4pt0r
 Volume Serial Number is 92C7-4424

 Directory of E:\Source\Repos\cjm-numerics\uint128_test_app\src\x64\Release

05/20/2021  07:54 PM    <DIR>          .
05/20/2021  07:54 PM    <DIR>          ..
05/12/2021  04:48 PM            34,120 base_test_battery_results_2021_May_12_20_48_37Z.txt
05/12/2021  04:48 PM            34,246 base_test_battery_results_2021_May_12_20_48_50Z.txt
05/20/2021  07:47 PM            34,244 base_test_battery_results_2021_May_20_23_47_16Z.txt
05/20/2021  07:54 PM            34,240 base_test_battery_results_2021_May_20_23_54_06Z.txt
04/23/2021  03:49 PM             8,702 random_binary_ops.txt
04/23/2021  03:49 PM               291 random_unary_ops.txt
05/20/2021  07:53 PM         1,128,448 WinInt128.exe
05/20/2021  07:53 PM        12,316,672 WinInt128.pdb
               8 File(s)     13,590,963 bytes
               2 Dir(s)  97,314,496,512 bytes free

E:\Source\Repos\cjm-numerics\uint128_test_app\src\x64\Release>WinInt128.exe /unary_from_file random_unary_ops.txt
Successfully read 9 unary operations from file ["random_unary_ops.txt"].
All 9 tests from file ["random_unary_ops.txt"] PASSED.

E:\Source\Repos\cjm-numerics\uint128_test_app\src\x64\Release>
```

*LINUX:* 
```bash
csusie@ubuntu:~/CLionProjects/Int128/cmake-build-release-clang$ ls
base_test_battery_results_2021_May_09_14_15_46Z.txt  base_test_battery_results_2021_May_09_14_31_22Z.txt  binary_ops_add_generated_2021_Jan_10_14_42_55Z.txt  CMakeFiles           ExampleCode  Makefile               test_file_1.txt
base_test_battery_results_2021_May_09_14_15_52Z.txt  base_test_battery_results_2021_May_09_15_33_01Z.txt  cjm-numerics-uint128.cbp                            cmake_install.cmake  Int128       random_binary_ops.txt  test_file_2.txt
base_test_battery_results_2021_May_09_14_21_01Z.txt  base_test_battery_results_2021_May_21_00_09_04Z.txt  CMakeCache.txt                                      EnvironTest          Int128.cbp   random_unary_ops.txt   Testing
csusie@ubuntu:~/CLionProjects/Int128/cmake-build-release-clang$ ./Int128 --unary_from_file random_unary_ops.txt 
Successfully read 9 unary operations from file ["random_unary_ops.txt"].
All 9 tests from file ["random_unary_ops.txt"] PASSED.
```  

#### **binary_from_file switch**  

The "binary_from_file_switch" requires as an argument the path of an ASCII text file containing binary operations.  It will then execute the binary operations in the file and report on success or failure.  

An example of such a file can be found here and is shown (greatly shortened) below:

> /;0;1;  
%;0;1;  
%;14627333968688430831;1611526157;  
/;3235827725;239;

The symbols for each supported operation can be found in the [int128_tests.hpp][5] header in the array called [op_symbol_lookup][6].  A test is deemed to pass if, when a binary operation of the specified type is applied to two values of type uint128 and two equivalent values of type [`boost::multiprecision::uint128_t`][7] (from the Boost library), the resulting values are equivalent.  If the resulting values differ, the test is deemed to have failed.
  
**Usage Example**:  
  
*WINDOWS:*   
```dos
E:\Source\Repos\cjm-numerics\uint128_test_app\src\x64\Release>dir
 Volume in drive E is R4pt0r
 Volume Serial Number is 92C7-4424

 Directory of E:\Source\Repos\cjm-numerics\uint128_test_app\src\x64\Release

05/20/2021  07:54 PM    <DIR>          .
05/20/2021  07:54 PM    <DIR>          ..
05/12/2021  04:48 PM            34,120 base_test_battery_results_2021_May_12_20_48_37Z.txt
05/12/2021  04:48 PM            34,246 base_test_battery_results_2021_May_12_20_48_50Z.txt
05/20/2021  07:47 PM            34,244 base_test_battery_results_2021_May_20_23_47_16Z.txt
05/20/2021  07:54 PM            34,240 base_test_battery_results_2021_May_20_23_54_06Z.txt
04/23/2021  03:49 PM             8,702 random_binary_ops.txt
04/23/2021  03:49 PM               291 random_unary_ops.txt
05/20/2021  07:53 PM         1,128,448 WinInt128.exe
05/20/2021  07:53 PM        12,316,672 WinInt128.pdb
               8 File(s)     13,590,963 bytes
               2 Dir(s)  97,314,492,416 bytes free

E:\Source\Repos\cjm-numerics\uint128_test_app\src\x64\Release>WinInt128.exe /binary_from_file random_binary_ops.txt
Successfully read 212 binary operations from file ["random_binary_ops.txt"].
All 212 tests from file ["random_binary_ops.txt"] PASSED.

E:\Source\Repos\cjm-numerics\uint128_test_app\src\x64\Release>
```

*LINUX:* 
```bash
csusie@ubuntu:~/CLionProjects/Int128/cmake-build-release-clang$ ls
base_test_battery_results_2021_May_09_14_15_46Z.txt  base_test_battery_results_2021_May_09_14_31_22Z.txt  binary_ops_add_generated_2021_Jan_10_14_42_55Z.txt  CMakeFiles           ExampleCode  Makefile               test_file_1.txt
base_test_battery_results_2021_May_09_14_15_52Z.txt  base_test_battery_results_2021_May_09_15_33_01Z.txt  cjm-numerics-uint128.cbp                            cmake_install.cmake  Int128       random_binary_ops.txt  test_file_2.txt
base_test_battery_results_2021_May_09_14_21_01Z.txt  base_test_battery_results_2021_May_21_00_09_04Z.txt  CMakeCache.txt                                      EnvironTest          Int128.cbp   random_unary_ops.txt   Testing
csusie@ubuntu:~/CLionProjects/Int128/cmake-build-release-clang$ ./Int128 --binary_from_file random_binary_ops.txt 
Successfully read 212 binary operations from file ["random_binary_ops.txt"].
All 212 tests from file ["random_binary_ops.txt"] PASSED.
csusie@ubuntu:~/CLionProjects/Int128/cmake-build-release-clang$ 
```  

#### **environ switch**

Using the environ switch prints out environmental / platform information that may affect performance.  See below from:

*WINDOWS (compiled targeting x64, with Advanced Vector Extensions 2 (/arch:AVX2): *  

```dos
E:\Source\Repos\cjm-numerics\uint128_test_app\src\x64\Release>WinInt128.exe --environ
Executing switch [environ]:
ENVIRONMENT DATA:
Detected compiler: [Microsoft].
SSE3: [NOT AVAILABLE].
Intel ADX: [AVAILABLE].
Intel BMI2: [AVAILABLE].
Execution Cores for concurrency: [12].

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
Is consteval (immediate function) available?: [false].
END ENVIRONMENT DATA

Done executing switch [environ]
```

Enabling AVX2 both allows the compiler to use AVX2 vector extensions on its own and, in certain circumstances, to use compiler intrinsics from ADX and BMI2, which are more efficient than the default intrinsics for those operations.  You are encouraged to enable AVX2 on a Linux platform as well, but the **initial note about ADX and BMI2 in the output will always read false in Linux**: Linux takes advantage of built-in uint128 arithmetic and the library does not use compiler intrinsics on Linux.  You should not take their showing up as false in Linux to mean that GCC or Clang is not using those architectural features ... it merely means that the library is not directly using those intrinsic function to perform calculations.  On Linux, it is left up to the compiler.  The initial notice about ADX and BMI2 is relevant for 64-bit Windows targets only (whether using Microsoft's compiler, the Clang compiler included in Visual Studio by Microsoft, or Intel's LLVM based compiler for Windows).  

The following are the results of executing the application with the environ switch on Linux (Clang 11.0.0-2): 

*LINUX:* 
```bash
csusie@ubuntu:~/CLionProjects/Int128/cmake-build-release-clang$ ./Int128 -environ
Executing switch [environ]: 
ENVIRONMENT DATA: 
Detected compiler: [Clang-GCC].
SSE3: [AVAILABLE].
Intel ADX: [NOT AVAILABLE].
Intel BMI2: [NOT AVAILABLE].
Execution Cores for concurrency: [6].

PRINTING SIZES:
        CHAR_BIT: 8 bits per byte on this system.
        Sizeof unsigned short: [2].
        Sizeof unsigned int: [4].
        Sizeof unsigned long: [8].
        Sizeof unsigned long long: [8].
        Sizeof size_t: [8].
        Sizeof uintmax_t: [8].
        Sizeof built-in unsigned 128 bit integer: [16].
        char is signed in this implementation.
        Sizeof wide character: [4].
        Wide character is signed in this implementation.
        Sizeof cjm::numerics::uint128: [16].
        Sizeof float: [4].
        Sizeof double: [8].
        Sizeof long double: [16].
        Sizeof pointer to object: [8].
        Sizeof most aligned built-in: [32].
DONE PRINTING SIZES

PRINTING ALIGNMENTS:
        Alignof unsigned short: [2].
        Alignof unsigned int: [4].
        Alignof unsigned long: [8].
        Alignof unsigned long long: [8].
        Alignof size_t: [8].
        Alignof uintmax_t: [8].
        Alignof built-in unsigned 128 bit integer: [16].
        Alignof char: [1].
        Alignof wide character: [4].
        Alignof cjm::numerics::uint128: [16].
        Alignof float: [4].
        Alignof double: [8].
        Alignof long double: [16].
        Alignof pointer to object: [8].
        Alignof most aligned built-in: [16].
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
        long double (size: 16, align: 16) is an iec559 type.
        long double uses 63 of its 128 bits for mantissa.
        long double's min exponent: -16381; max exponent: 16384.
        long double's max value: 1.18973e+4932.
Done printing data for type long double.

Done printing floating point information.

Is constexpr bitcasting available?: [NO].
Arithmetic mode: [Built-in unsigned __int128 Arithmetic].
Are C++20 bit operations available?: [YES].
Is unsigned __int128 an arithmetic type?: [YES].
Is unsigned __int128 an integral type?: [YES].
Is unsigned __int128 an unsigned type?: [YES].
sizeof(std::uintmax_t): [8].
Is consteval (immediate function) available?: [false].
END ENVIRONMENT DATA

Done executing switch [environ] 

csusie@ubuntu:~/CLionProjects/Int128/cmake-build-release-clang$ 
```

#### default_tests switch

Executing the application with the "default_tests" switch is the same as executing it without specifying any switches: the entire battery of tests is executed.  See above for examples.  

#### list switch 

Executing the application with the list switch provides a list of the individual tests from the battery of tests.  These can be used with the "specific_test" switch (see below) to run individual tests from the battery.  

**Usage Example**:  
  
*WINDOWS:*   

```dos
E:\Source\Repos\cjm-numerics\uint128_test_app\src\x64\Release>WinInt128.exe /list
Going to print all [55] tests:
Test#:  [1]:     "addition_tests"
Test#:  [2]:     "ascii_char_interconversions"
Test#:  [3]:     "basic_multiplication_test"
Test#:  [4]:     "basic_test_one"
Test#:  [5]:     "basic_u128_adc_test"
Test#:  [6]:     "basic_u128_sbb_test"
Test#:  [7]:     "binary_operation_rt_ser_tests"
Test#:  [8]:     "builtin_add_with_carry_test"
Test#:  [9]:     "builtin_sub_with_borrow_test"
Test#:  [10]:    "builtin_u128fls_test_if_avail"
Test#:  [11]:    "bw_tests"
Test#:  [12]:    "comparison_tests"
Test#:  [13]:    "controlled_float_rt_conversion_test"
Test#:  [14]:    "controlled_from_float_conversion_test"
Test#:  [15]:    "div_mod_zero_dividend_nonzero_divisor_tests"
Test#:  [16]:    "div_mod_zero_tests"
Test#:  [17]:    "division_modulus_tests"
Test#:  [18]:    "failing_division_test_1"
Test#:  [19]:    "failing_division_test_2"
Test#:  [20]:    "failing_modulus_test_1"
Test#:  [21]:    "first_bin_op_test"
Test#:  [22]:    "gen_comp_ops_test"
Test#:  [23]:    "hash_dx"
Test#:  [24]:    "issue_10_showbase_test"
Test#:  [25]:    "issue_10_strm_insrt_test"
Test#:  [26]:    "literal_test"
Test#:  [27]:    "multiplication_tests"
Test#:  [28]:    "print_generated_filename_test"
Test#:  [29]:    "safe_float_conversions_test"
Test#:  [30]:    "shift_tests"
Test#:  [31]:    "stream_insert_bin_op_test"
Test#:  [32]:    "string_parse_text"
Test#:  [33]:    "subtraction_tests"
Test#:  [34]:    "test_convert_to_double"
Test#:  [35]:    "test_convert_to_float"
Test#:  [36]:    "test_convert_to_long_double"
Test#:  [37]:    "test_fls"
Test#:  [38]:    "throwing_float_conversion_test"
Test#:  [39]:    "trim_tests"
Test#:  [40]:    "uintcontainer_adc_tests"
Test#:  [41]:    "umult_spec_tests"
Test#:  [42]:    "unary_op_basic_test"
Test#:  [43]:    "unary_op_bitwise_not_test"
Test#:  [44]:    "unary_op_bool_cast_test"
Test#:  [45]:    "unary_op_code_rt_serialization_tests"
Test#:  [46]:    "unary_op_logical_negation_test"
Test#:  [47]:    "unary_op_post_dec_test"
Test#:  [48]:    "unary_op_post_inc_test"
Test#:  [49]:    "unary_op_post_stat_assert_test"
Test#:  [50]:    "unary_op_pre_dec_test"
Test#:  [51]:    "unary_op_pre_inc_test"
Test#:  [52]:    "unary_op_unary_minus_test"
Test#:  [53]:    "unary_op_unary_plus_test"
Test#:  [54]:    "unary_operation_rt_serialization_tests"
Test#:  [55]:    "unary_operation_vec_rt_serialization_tests"
Done printing available tests.
```

*LINUX:*   

```bash
csusie@ubuntu:~/CLionProjects/Int128/cmake-build-release-clang$ ./Int128 --list
Going to print all [55] tests: 
Test#:  [1]:     "addition_tests"
Test#:  [2]:     "ascii_char_interconversions"
Test#:  [3]:     "basic_multiplication_test"
Test#:  [4]:     "basic_test_one"
Test#:  [5]:     "basic_u128_adc_test"
Test#:  [6]:     "basic_u128_sbb_test"
Test#:  [7]:     "binary_operation_rt_ser_tests"
Test#:  [8]:     "builtin_add_with_carry_test"
Test#:  [9]:     "builtin_sub_with_borrow_test"
Test#:  [10]:    "builtin_u128fls_test_if_avail"
Test#:  [11]:    "bw_tests"
Test#:  [12]:    "comparison_tests"
Test#:  [13]:    "controlled_float_rt_conversion_test"
Test#:  [14]:    "controlled_from_float_conversion_test"
Test#:  [15]:    "div_mod_zero_dividend_nonzero_divisor_tests"
Test#:  [16]:    "div_mod_zero_tests"
Test#:  [17]:    "division_modulus_tests"
Test#:  [18]:    "failing_division_test_1"
Test#:  [19]:    "failing_division_test_2"
Test#:  [20]:    "failing_modulus_test_1"
Test#:  [21]:    "first_bin_op_test"
Test#:  [22]:    "gen_comp_ops_test"
Test#:  [23]:    "hash_dx"
Test#:  [24]:    "issue_10_showbase_test"
Test#:  [25]:    "issue_10_strm_insrt_test"
Test#:  [26]:    "literal_test"
Test#:  [27]:    "multiplication_tests"
Test#:  [28]:    "print_generated_filename_test"
Test#:  [29]:    "safe_float_conversions_test"
Test#:  [30]:    "shift_tests"
Test#:  [31]:    "stream_insert_bin_op_test"
Test#:  [32]:    "string_parse_text"
Test#:  [33]:    "subtraction_tests"
Test#:  [34]:    "test_convert_to_double"
Test#:  [35]:    "test_convert_to_float"
Test#:  [36]:    "test_convert_to_long_double"
Test#:  [37]:    "test_fls"
Test#:  [38]:    "throwing_float_conversion_test"
Test#:  [39]:    "trim_tests"
Test#:  [40]:    "uintcontainer_adc_tests"
Test#:  [41]:    "umult_spec_tests"
Test#:  [42]:    "unary_op_basic_test"
Test#:  [43]:    "unary_op_bitwise_not_test"
Test#:  [44]:    "unary_op_bool_cast_test"
Test#:  [45]:    "unary_op_code_rt_serialization_tests"
Test#:  [46]:    "unary_op_logical_negation_test"
Test#:  [47]:    "unary_op_post_dec_test"
Test#:  [48]:    "unary_op_post_inc_test"
Test#:  [49]:    "unary_op_post_stat_assert_test"
Test#:  [50]:    "unary_op_pre_dec_test"
Test#:  [51]:    "unary_op_pre_inc_test"
Test#:  [52]:    "unary_op_unary_minus_test"
Test#:  [53]:    "unary_op_unary_plus_test"
Test#:  [54]:    "unary_operation_rt_serialization_tests"
Test#:  [55]:    "unary_operation_vec_rt_serialization_tests"
Done printing available tests.
```

#### **specific_test switch**

Using the specific_test switch with the name of a test (as obtained by the list switch, see above) executes the specified test.  When executing an individual test rather than the entire battery, all information is sent to the standard ouput (hopefully) or the standard error (in the sad case).  The following examples demonstrate the hash_dx test.  The hash_dx test generates hashes, and prints out usage information.  In 64-bit environments it is highly unlikely that there will be any equal uint128 values produced and also highly unlikely that any of the hash values obtained from uint128 will collide.  In a 32-bit environment, however, you can expect around 150 hash collisions.  

**WINDOWS (x64)**:  

```dos
E:\Source\Repos\cjm-numerics\uint128_test_app\src\x64\Release>WinInt128.exe /specific_test hash_dx
Beginning execution of test [hash_dx].
Of 1000000 uint128_t's, 1000000 unique values were generated.  Those values were reduced to 1000000 unique hashes. There were 0 colliding hash values.
Test [hash_dx], PASSED!

Will execute test: [hash_dx].
        Result of test:
                TEST:   [hash_dx]       Status:         [complete]
        Started at:     [1.38258e+10 microseconds]
        Ended at:       [1.38267e+10 microseconds]
        Elapsed time:   [829466 microseconds]
        Test PASSED.

END TEST:       [hash_dx].

Done with test: [hash_dx].
```

**WINDOWS (x86)**:  

```dos
E:\Source\Repos\cjm-numerics\uint128_test_app\src\Release>WinInt128.exe -specific_test hash_dx
Beginning execution of test [hash_dx].
Of 1000000 uint128_t's, 1000000 unique values were generated.  Those values were reduced to 999890 unique hashes. There were 110 colliding hash values.
Test [hash_dx], PASSED!

Will execute test: [hash_dx].
        Result of test:
                TEST:   [hash_dx]       Status:         [complete]
        Started at:     [1.40086e+10 microseconds]
        Ended at:       [1.40094e+10 microseconds]
        Elapsed time:   [825852 microseconds]
        Test PASSED.

END TEST:       [hash_dx].

Done with test: [hash_dx].


E:\Source\Repos\cjm-numerics\uint128_test_app\src\Release>
```
  
**LINUX (x64)**:

```bash
csusie@ubuntu:~/CLionProjects/Int128/cmake-build-release-clang$ ./Int128 --specific_test hash_dx
Beginning execution of test [hash_dx].
Of 1000000 uint128_t's, 1000000 unique values were generated.  Those values were reduced to 1000000 unique hashes. There were 0 colliding hash values.
Test [hash_dx], PASSED!

Will execute test: [hash_dx].
        Result of test: 
                TEST:   [hash_dx]       Status:         [complete]
        Started at:     [6.90054e+09 microseconds]
        Ended at:       [6.90118e+09 microseconds]
        Elapsed time:   [641226 microseconds]
        Test PASSED.

END TEST:       [hash_dx].

Done with test: [hash_dx].

csusie@ubuntu:~/CLionProjects/Int128/cmake-build-release-clang$
```

Note that x86 Linux *may* work and (the library itself, not the unit test application) *probably would* work (at least after a few bug fixes) but **x86 Linux has NEVER been tested** and is not, presently, part of the test process applied to this library.  

#### **help switch**

Using the help switch (or, as above, command line arguments the application does not understand) will result in the valid switches (with brief description) and usage examples being printed.  Usage examples below:

*WINDOWS:*  

```dos
E:\Source\Repos\cjm-numerics\uint128_test_app\src\Release>WinInt128.exe /help
Switches include:
        unary_from_file         Execute unary operations tests as specified in unary operations text file.
        binary_from_file        Execute binary operations tests as binary in unary operations text file.
        environ                 Print environmental information.
        default_tests           Run all tests in default battery, logging summary of result to standard output and detail to a log file.
        list                    List all tests in the default battery.
        specific_test           Run the specified test from the default battery.
        help                    Print this information.
Usage examples for all switches:
--unary_from_file                        my_unary_operations_file.txt
--binary_from_file                       my_binary_operations_file.txt
--environ
--default_tests
--list
--specific_test                          name_of_test_to_run
--help
DONE with usage examples.



E:\Source\Repos\cjm-numerics\uint128_test_app\src\Release>
```

*LINUX:*  

```bash
csusie@ubuntu:~/CLionProjects/Int128/cmake-build-release-clang$ ./Int128 --help
Switches include: 
        unary_from_file         Execute unary operations tests as specified in unary operations text file.
        binary_from_file        Execute binary operations tests as binary in unary operations text file.
        environ                 Print environmental information.
        default_tests           Run all tests in default battery, logging summary of result to standard output and detail to a log file.
        list                    List all tests in the default battery.
        specific_test           Run the specified test from the default battery.
        help                    Print this information.
Usage examples for all switches: 
--unary_from_file                        my_unary_operations_file.txt
--binary_from_file                       my_binary_operations_file.txt
--environ 
--default_tests 
--list 
--specific_test                          name_of_test_to_run
--help 
DONE with usage examples.


csusie@ubuntu:~/CLionProjects/Int128/cmake-build-release-clang$
```  

## Test Application Dependencies  

Unlike the library itself and the example code project, which are dependency free, the unit test project relies on several external dependencies.  Those dependencies include:

1. [abseil-C++17][8] (vcpkg: abseil[cxx17])
2. [boost][9] version 1.73+ (vcpkg: boost)
3. [hinnant/date][10] 3.0.0+ (vcpkg: date)  

The visual studio project file for the test application and the CMAKE / CLion project files assume that you are using vcpkg to manage your dependencies.  In Visual Studio, these libraries are built linking against a static CRT (C runtime) for both x64 and x86 Windows and are linked statically into the unit test application (which also links against a static CRT).  On Linux/CLion/CMake, vcpkg default Linux linkage settings (I assume dynamic runtime and dynamic library linkage) are used.   

Abseil and Boost are used to provide control for the applications test cases.  Many take equivalent values of uint128 from this library and boosts uint128_t or abseil's uint128 type and perform identical operations on them.  Tests are deemed passing if the test result matches the boost or abseil control result.  Howard Hinnant's date library (which has been adopted into C++20's standard library but presently not fully supported by GCC, Clang and Microsoft) is used to help generate date-based suffixes for file output. Also, several of Boost's other convenience features are used (such as its output format saver).  

## Binaries For Windows

For Windows, I plan to release the unit test application in binary form with future beta and official releases.  It will be available in Debug and Release builds for both x86 and x64 targets.  The x64 targets will assume AVX2 is available.  If requested, I will create a x64 Windows binary that does not enable AVX2 though most fairly recent processors from AMD and Intel (last 5-7 years or so) support it.  For now, do not attempt to run unit test application binaries built for x64 Windows on a processor too old to support AVX2.  


  [1]: https://github.com/cpsusie/cjm-numerics/blob/48796e42103faf0fe5b7b60d09418abf86f1a216/uint128_test_app/src/WinInt128/random_unary_ops.txt
  [2]: https://github.com/cpsusie/cjm-numerics/blob/f93d362577367b90e23a6979151dedd5b843bbe3/uint128_test_app/src/headers/int128_tests.hpp
  [3]: https://github.com/cpsusie/cjm-numerics/blob/f93d362577367b90e23a6979151dedd5b843bbe3/uint128_test_app/src/headers/int128_tests.hpp#L590
  [4]: https://www.boost.org/doc/libs/1_76_0/libs/multiprecision/doc/html/boost_multiprecision/tut/ints/cpp_int.html
  [5]: https://github.com/cpsusie/cjm-numerics/blob/f93d362577367b90e23a6979151dedd5b843bbe3/uint128_test_app/src/headers/int128_tests.hpp
  [6]: https://github.com/cpsusie/cjm-numerics/blob/f93d362577367b90e23a6979151dedd5b843bbe3/uint128_test_app/src/headers/int128_tests.hpp#L619
  [7]: https://www.boost.org/doc/libs/1_76_0/libs/multiprecision/doc/html/boost_multiprecision/tut/ints/cpp_int.html 
  [8]: https://github.com/abseil/abseil-cpp/tree/master/absl
  [9]: https://github.com/boostorg/boost
  [10]: https://github.com/HowardHinnant/date

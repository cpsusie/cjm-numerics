# CJM_FIXED_PRECISION: (a fixed precision numeric library for C++20)
## Summary
  This library currently has the goal of providing an unsigned 128 bit integer type suitable for direct use or for extending to make other fixed precision types such as signed two's complement integers, larger signed or unsigned integers, rational numbers or flexible units of measure.  The unsigned 128-bit integer is currently feature-complete and extensively tested.
## Sources
  This library is derivative of the unsigned and signed 128-bit provided by abseil.io at [abseil's github repository.](https://github.com/abseil/abseil-cpp/blob/master/absl/numeric/int128.h#L100)  It also has considered and adopted some of the work of other multiprecision numeric libraries such as the one provided by Boost, calcrypto, and even LLVM itself.  

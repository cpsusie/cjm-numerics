# Explanation of Future Feature: Extended Precision Multiplication Algorithm
Copyright © 2020-2021 CJM Screws, LLC  

Future versions of this library will have further extended precision unsigned integer types that will build on uint128.  It is therefore desirable to put forth the algorithm for extended precision multiplication so that it can be expressed generically and effectively and have the best possible performance characteristics.

This algorithm, when generalized, explains how to pull off a multiplication for values of size X by splitting X into 4 multiplications of values of size X/2 (though using variables of size X to perform multiplications) followed by a series of left shifts then add_with_carries of a type with size x*2.

For this example we will be multipling two 16 bit unsigned integers by splitting each of them up into two 8 bit integers, which are then zero-extended back to 16 bit integers.  Thus, for multiplication of 0xfea2 * 0xf00f: fea2 will be split into 0x00fe and 0x00a2; f00f will be split up into 0x00f0 and 0x000f.

## Splitting up two variables of size 16  
## Declarations
* let factor_1 := 0xfe'a2 (std::uint16_t)
* let factor_2 := 0xf0'0f (std::uint16_t)
* let const mult_size := 16 (`std::numeric_limits<std::uint16_t>::digits`)
* let const add_c_size := 32 (mult_size * 2)
* let const base_shift := 8 (mult_size / 2)
* let const full_shift := 16 (mult_size)
* using add_c_size_t = std::uint32_t (unsigned int of size add_c_size)
* using split_size_t = std::uint8_t (unsigned int of size base_shift)
* using multiplicand_t = std::uint16_t (unsigned int of size full_shift)
* let const low_bit_mask = max value of usigned integer of same size as base_shift, zero extended to size multiplicand_t  

## Split up factors
* let factor_1_low := factor_1 & low_bit_mask (0x00a2 : multiplicand_t) 
* let factor_1_high := factor_1 >> base_shift (0x00fe : multiplicand_t)
* let factor_2_low := factor_2 & low_bit_mask (0x000f : multiplicand_t)
* let factor_2_high := factor_2 >> base_shift (0x00f0 : multiplicand_t)    
## Peform 4 split-up multiplications 
* let product_f1_low_f2_low := add_c_size_t(factor_1_low * factor_2_low)      ( ((0x00a2 * 0x000f) -> add_c_size_t) == 0x0000_097e)
* let product_f1_high_f2_low := add_c_size_t(factor_1_high * factor_2_low)    ( ((0x00fe * 0x000f) -> add_c_size_t) == 0x0000_0ee2)
* let product_f1_low_f2_high := add_c_size_t(factor_1_low * factor_2_high)    ( ((0x00a2 * 0x00f0) -> add_c_size_t) == 0x0000_97e0)
* let product_f1_high_f2_high := add_c_size_t(factor_1_high * factor_2_high)  ( ((0x00fe * 0x00f0) -> add_c_size_t) == 0x0000_ee20)
## Adjust products
* product_f1_high_f2_low <<= base_shift (0x000e_e200)
* product_f1_low_f2_high <<= base_shift (0x0097_e000)
* product_f1_high_f2_high <<= full_shift (0xee20_0000)
## Perform 32-bit addition ( *or 16 bit add with carry* )
* let ***sum*** := product_f1_low_f2_low + product_f1_high_f2_low + product_f1_low_f2_high + product_f1_high_f2_high (0xeec6_cb7e)
* **return *sum***

## Second Example: 0xffff * 0xffff  
## Declarations
* let factor_1 := 0xff'ff (std::uint16_t)
* let factor_2 := 0xff'ff (std::uint16_t)
* let const mult_size := 16 (std::numeric_limits<std::uint16_t>::digits)
* let const add_c_size := 32 (mult_size * 2)
* let const base_shift := 8 (mult_size / 2)
* let const full_shift := 16 (mult_size)
* using add_c_size_t = std::uint32_t (unsigned int of size add_c_size)
* using split_size_t = std::uint8_t (unsigned int of size base_shift)
* using multiplicand_t = std::uint16_t (unsigned int of size full_shift)
* let const low_bit_mask = max value of usigned integer of same size as base_shift, zero extended to size multiplicand_t  

## Split up factors
* let factor_1_low := factor_1 & low_bit_mask (0x00ff : multiplicand_t) 
* let factor_1_high := factor_1 >> base_shift (0x00ff : multiplicand_t)
* let factor_2_low := factor_2 & low_bit_mask (0x00ff : multiplicand_t)
* let factor_2_high := factor_2 >> base_shift (0x00ff : multiplicand_t)   

## Peform 4 split-up multiplications 
* let product_f1_low_f2_low := add_c_size_t(factor_1_low * factor_2_low)      ( ((0x00ff * 0x00ff) -> add_c_size_t) == 0x0000_fe01)
* let product_f1_high_f2_low := add_c_size_t(factor_1_high * factor_2_low)    ( ((0x00ff * 0x00ff) -> add_c_size_t) == 0x0000_fe01)
* let product_f1_low_f2_high := add_c_size_t(factor_1_low * factor_2_high)    ( ((0x00ff * 0x00ff) -> add_c_size_t) == 0x0000_fe01)
* let product_f1_high_f2_high := add_c_size_t(factor_1_high * factor_2_high)  ( ((0x00ff * 0x00ff) -> add_c_size_t) == 0x0000_fe01)

## Adjust products
* product_f1_high_f2_low <<= base_shift (0x00fe_0100)
* product_f1_low_f2_high <<= base_shift (0x00fe_0100)
* product_f1_high_f2_high <<= full_shift (0xfe01_0000)

## Perform 32-bit addition (or 16 bit add with carry)
* let ***sum*** := product_f1_low_f2_low + product_f1_high_f2_low + product_f1_low_f2_high + product_f1_high_f2_high (0xfffe_0001)
* **return *sum***


# Elaboration of 16-bit add with carry alternative to 32 bit addition
## Consider the additions from the 0xffff * 0xffff example above
## Peform 4 split-up multiplications (each variable is 16 bit)
* let low_product_f1_low_f2_low := factor_1_low * factor_2_low      ((0x00ff * 0x00ff) == 0xfe01)
* let high_product_f1_low_f2_high := 0
* let low_product_f1_high_f2_low := factor_1_high * factor_2_low    ((0x00ff * 0x00ff) == 0xfe01)
* let high_product_f1_low_f2_high := 0
* let low_product_f1_low_f2_high := factor_1_low * factor_2_high    ((0x00ff * 0x00ff) == 0xfe01)
* let high_product_f1_low_f2_high := 0
* let product_f1_high_f2_high := factor_1_high * factor_2_high      ((0x00ff * 0x00ff) == 0xfe01)
* let high_product_f1_high_f2_high := 0

## Adjust products with left shift to have same results as would if each high_low pair were one 32-bit variable
* (high_product_f1_low_f2_low    :   low_product_f1_low_f2_low)     NOSHIFT           -> (0x0000, 0xfe01)
* (high_product_f1_high_f2_low   :   low_product_f1_high_f2_low)    LBASE SHIFT 8     -> (0x00fe, 0x0100)
* (high_product_f1_low_f2_high   :   low_product_f1_low_f2_high)    LBASE SHIFT 8     -> (0x00fe, 0x0100)
* (high_product_f1_high_f2_high  :   low_product_f1_high_f2_high)   LBASE SHIFT 16    -> (0xfe01, 0x0000)

## Now it's time for the additions
* let carry_out := false
* let carry_in := false
* let low_sum := add_with_carry_u16(low_product_f1_low_f2_low, low_product_f1_high_f2_low, carry_in, out carry_out) (low_sum == 0xff01, carry_out == false )  
* carry_in <- carry_out
* let high_sum := add_with_carry_u16(high_product_f1_low_f2_low, high_product_f1_high_f2_low, carry_in, out carry_out ) (high_sum == 0x00fe, carry_out == false)
* carry_in <- false
* low_sum <- add_with_carry_u16(low_sum, low_product_f1_low_f2_high, carry_in, out carry_out) (low_sum == 0xff01 + 0x0100 == 0x0001, carry_out == true)
* carry_in <- carry_out (true)
* high_sum <- add_with_carry_u16(high_sum, high_product_f1_low_f2_high, carry_in, out carry_out) (high_sum == (0x00fe + 0x00fe + 1)
== 0x01fd, carry_out == false)
* carry_in <- false
* low_sum <- add_with_carry_u16(low_sum, low_product_f1_high_f2_high, carry_in, out carry_out) (low_sum == 0x0001 + 0x0000 == 0x0001, carry_out == false)
* carry_in <- carry_out (false)
* high_sum <- add_with_carry_u16(high_sum, high_product_f1_high_f2_high, carry_in, out carry_out) (high_sum == 0x01fd + 0xfe01 == 0xfffe, carry_out == false)

## Now bring high sum and low some together into a 32 bit value (bitcasting would work or something like this)
* let ***sum*** := add_c_size_t(high_sum)
* sum <- sum << full_shift
* sum <- (sum | add_c_size_t(low_sum))
* debug_assert(carry_out == false)
* **return *sum***
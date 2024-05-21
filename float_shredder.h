#ifndef float_shredder
#define float_shredder

#include <stdint.h>
#include <math.h>

/*
	The float shredder library is a header only library to destroy
	your floats in novel and grotesque ways.

	This library allows you to extract the various parts of your
	floats and allows you to edit them as you please.

	Since this is going to use real hacky bit manipulations,
	don't expect this library to be super elegant
	(although I will try :)).
*/
/*
	The IEEE 754 standard for floating point numbers (the one implemented by the
	C and C++ compilers) is essentially a binary version of scientific notation,
	with a sign bit, exponent bits, and mantissa bits.

	A 32-bit IEEE 754 float is laid out in memory like this:
	Sign	Exponent		Mantissa
	0	0000000		00000000000000000000000
	1-bit |	8-bits	   |	23-bits			= 32-bits

	The exponent doesn't use 2's complement to represent it (no sign bit).
	This is for efficiency in comparing two numbers.
	Instead it uses an exponent bias to render negative numbers.

	The bias for any bit-width IEEE 754 floating point number is:
		2^(e-1)-1
		where e=the number of bits in the exponent.
	
	In the case of 32-bit floating points, the bias is 127. So to get the
	true exponent value you do (raw_exponent - bias), or (raw_exponent - 127)
	in the case of 32-bit floats.

	The point of a library like this is to extract these values and/or modify 
	them then package them back up into the format the compiler expects.
*/

// tabbing the values of these so I can make sure they're all the
// same width
uint32_t float_exp_mask_32 =		0x7F800000;
uint32_t float_sign_mask_32 =		0x80000000;
uint32_t float_mantissa_mask_32 =	0x007FFFFF;
exp_offset_32 = 23;
sign_offset_32 = 31;
exp_bias_32 = 127;
exp_bias_64 = 1023;



/*
	(Almost) Everything from here on will be composed with the raw float memory
	We use a uin32_t to represent the raw float data because it is a container
	of the same size. It also supports all the operations we want to do from
	here on.

	C/C++ implicit conversion will turn it into a value-preserving integer, 
	but not a data-preserving integer.
	For instance, the implicit conversion of 12.5 to an integer will become
	13, which preserves value (as much as possible), but the memory layout
	will be entirely different.

	All explicit conversions to raw memory then to another type are to
	leave memory layout completely unaltered, which allows us to do our
	magic.
*/
uint32_t shredder_raw_float_value_32(float input_float) 
{
	void* input_float_mem = &input_float;
	return *(uint32_t*)input_float_mem;
}

/*
	This is to get the raw data back into a float without dealing with C/C++
	implicit conversion.
*/
float shredder_raw_int_to_float_32(uint32_t input_int)
{
	void* input_int_mem = &input_int;
	return *(float*)input_int_mem;
}

uint32_t shredder_exp_unbiased_32(float input_float)
{
	return (shredder_raw_float_value_32(input_float) & 
		float_exp_mask_32) >> exp_offset_32;
}

uint32_t shredder_raw_exp_unbiased_32(float input_float)
{
	return shredder_raw_float_value_32(input_float) & float_exp_mask_32;
}

uint32_t shredder_exp_biased_32(float input_float)
{
	return shredder_exp_unbiased_32(input_float) - 127;
}

// I can't yet foresee a use for this function, but it didn't make sense to leave
// it out.
uint32_t shredder_raw_exp_biased_32(float input_float)
{
	return shredder_exp_biased_32(input_float) << exp_offset_32;
}

uint32_t shredder_raw_mantissa_32(float input_float)
{
	return shredder_raw_float_value_32(input_float) & float_mantissa_mask_32;
}

bool shredder_is_negative_32(float input_float)
{
	return (shredder_raw_float_value_32(input_float) & float_sign_mask_32) >> 31;
}

float shredder_inc_shift_exp_32(float input_float, int shift)
{
	// this isn't enough to prevent overflows but it'll do for now
	if(shift > 8)
	{
		shift -= (shift-8);
	}
	uint32_t float_exp = shredder_raw_float_value_32(input_float) & float_exp_mask_32;
	uint32_t float_no_exp = shredder_raw_float_value_32(input_float) & ~float_exp_mask_32;
	return shredder_raw_int_to_float_32
		(((float_exp >> shift) & float_exp_mask_32) | float_no_exp);
}

float shredder_dec_shift_exp_32(float input_float, int shift)
{
	// this isn't enough to prevent overflows but it'll do for now
	if(shift > 8)
	{
		shift -= (shift-8);
	}
	uint32_t float_exp = shredder_raw_float_value_32(input_float) & float_exp_mask_32;
	uint32_t float_no_exp = shredder_raw_float_value_32(input_float) & ~float_exp_mask_32; 
	return shredder_raw_int_to_float_32
		(((float_exp << shift) & float_exp_mask_32) | float_no_exp);
}

float shredder_inc_shift_mant_32(float input_float, int shift)
{
	// this isn't enough to prevent overflows but it'll do for now
	if(shift > 23)
	{
		shift -= (shift-23);
	}
	uint32_t float_mant = shredder_raw_float_value_32(input_float) & float_mantissa_mask_32;
	uint32_t float_no_mant = 
		shredder_raw_float_value_32(input_float) & ~float_mantissa_mask_32;

	return shredder_raw_int_to_float_32
		(((float_mant << shift) & float_mantissa_mask_32) | float_no_mant);

}

float shredder_inc_shift_mant_32(float input_float, int shift)
{
	// this isn't enough to prevent overflows but it'll do for now
	if(shift > 23)
	{
		shift -= (shift-23);
	}
	uint32_t float_mant = shredder_raw_float_value_32(input_float) & float_mantissa_mask_32;
	uint32_t float_no_mant = 
		shredder_raw_float_value_32(input_float) & ~float_mantissa_mask_32;

	return shredder_raw_int_to_float_32
		(((float_mant >> shift) & float_mantissa_mask_32) | float_no_mant);

}



#endif

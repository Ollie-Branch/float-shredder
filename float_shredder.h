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
const uint32_t float_exp_mask =			0x7F800000;
const uint32_t float_sign_mask =		0x80000000;
const uint32_t float_mantissa_mask =		0x007FFFFF;
const int float_exp_offset = 23;
const int float_sign_offset = 31;
const int float_exp_bias = 127;
const int double_exp_bias = 1023;

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
uint32_t ShredFloatToData(float input_float) 
{
	void* input_float_mem = &input_float;
	return *(uint32_t*)input_float_mem;
}

/*
	This is to get the raw data back into a float without dealing with C/C++
	implicit conversion.
*/
float ShredDataToFloat(uint32_t input_int)
{
	void* input_int_mem = &input_int;
	return *(float*)input_int_mem;
}

uint32_t ShredFloatExpUnbiased(float input_float)
{
	return (ShredFloatToData(input_float) & 
		float_exp_mask) >> float_exp_offset;
}

uint32_t ShredFloatExpUnbiasedRaw(float input_float)
{
	return ShredFloatToData(input_float) & float_exp_mask;
}

int32_t ShredFloatExp(float input_float)
{
	return (int32_t)ShredFloatExpUnbiased(input_float) - 127;
}

// I can't yet foresee a use for this function, but it didn't make sense to leave
// it out.
int32_t ShredFloatExpRaw(float input_float)
{
	return ShredFloatExp(input_float) << float_exp_offset;
}

uint32_t ShredFloatMantissaRaw(float input_float)
{
	return ShredFloatToData(input_float) & float_mantissa_mask;
}

// TODO: test
float ShredFloatMantissa(float input_float)
{
	if(ShredFloatExpUnbiased(input_float) > 0)
	{
		return ShredDataToFloat
			(ShredFloatMantissaRaw(input_float)) + 1;
	} else {
		return ShredDataToFloat
			(ShredFloatMantissaRaw(input_float));
	}
}

// TODO: test
bool ShredFloatIsNegative(float input_float)
{
	return (ShredFloatToData(input_float) & float_sign_mask) >> 31;
}

// TODO: test
float ShredFloatShiftExpUp(float input_float, int shift)
{
	// this isn't enough to prevent overflows but it'll do for now
	if(shift > 8)
	{
		shift -= (shift-8);
	}
	uint32_t float_exp = ShredFloatToData(input_float) & float_exp_mask;
	uint32_t float_no_exp = ShredFloatToData(input_float) & ~float_exp_mask;
	return ShredDataToFloat
		(((float_exp << shift) & float_exp_mask) | float_no_exp);
}

// TODO: test
float ShredFloatShiftExpDown(float input_float, int shift)
{
	// this isn't enough to prevent overflows but it'll do for now
	if(shift > 8)
	{
		shift -= (shift-8);
	}
	uint32_t float_exp = ShredFloatToData(input_float) & float_exp_mask;
	uint32_t float_no_exp = ShredFloatToData(input_float) & ~float_exp_mask; 
	return ShredDataToFloat
		(((float_exp >> shift) & float_exp_mask) | float_no_exp);
}

// TODO: test
float ShredFloatShiftMantUp(float input_float, int shift)
{
	// this isn't enough to prevent overflows but it'll do for now
	if(shift > 23)
	{
		shift -= (shift-23);
	}
	uint32_t float_mant = ShredFloatToData(input_float) & float_mantissa_mask;
	uint32_t float_no_mant = 
		ShredFloatToData(input_float) & ~float_mantissa_mask;

	return ShredDataToFloat
		(((float_mant << shift) & float_mantissa_mask) | float_no_mant);

}

// TODO: test
float ShredFloatShiftMantDown(float input_float, int shift)
{
	// this isn't enough to prevent overflows but it'll do for now
	if(shift > 23)
	{
		shift -= (shift-23);
	}
	uint32_t float_mant = ShredFloatToData(input_float) & float_mantissa_mask;
	uint32_t float_no_mant = 
		ShredFloatToData(input_float) & ~float_mantissa_mask;

	return ShredDataToFloat
		(((float_mant >> shift) & float_mantissa_mask) | float_no_mant);

}



#endif

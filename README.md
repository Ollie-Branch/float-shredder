# [WIP] Float Shredder
## A C/C++ header only library to eviscerate your floats in novel and grotesque ways

Whether you want to simply gather information about a particular floating point number that's not usually represented in the value itself, or you want to do unique bit-shifting operations on the float (like, to efficiently increment or decrement the float by powers of two) this library will hopefully be able to achieve that, eventually.

As this is a WIP and I'm not a C developer by trade, expect this code to not be the most optimal or sane C code you've ever read. This library was started to assist with a pet project to analyze the binary contents of floating point numbers, and as such probably isn't up to professional standards of C or C++ that you may expect.

This library should compile in both C and C++ codebases, but as of now it's only been tested with C++ code. I made sure to use only the C standard library and none of the C++ features to make this code as portable as possible.

Currently being used in my project, [float analyzer](https://github.com/Ollie-Branch/float-analyzer)

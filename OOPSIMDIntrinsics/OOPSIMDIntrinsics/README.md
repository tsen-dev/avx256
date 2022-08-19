# Contents
- [Overview](https://github.com/tsen-dev/avx256#overview)
- [Quickstart Guide](https://github.com/tsen-dev/avx256#quickstart-guide)
- [Demos](https://github.com/tsen-dev/avx256#demos)
- [Documentation](https://github.com/tsen-dev/avx256#documentation)
- [Inclusion Guide](https://github.com/tsen-dev/avx256#inclusion-guide)
- [Limitations](https://github.com/tsen-dev/avx256#limitations)


#Overview

AVX256 is a [SIMD](https://en.wikipedia.org/wiki/Single_instruction,_multiple_data) library that provides an intuitive and readable interface to 256-bit [AVX](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions) and [AVX2](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions#Advanced_Vector_Extensions_2) SIMD instructions using zero-cost abstractions.

It provides common SIMD operations, both one-to-one mappings to AVX instructions and operations achieved using sequences of AVX instructions 

It is built on [compiler intrinsics](https://docs.microsoft.com/en-us/cpp/intrinsics/compiler-intrinsics?view=msvc-170), hence takes advantage of register allocation, instruction scheduling, and other compiler optimisations for the best performance and integration with surrounding code.


# Quickstart Guide
1. Identify whether your CPU supports AVX2 instructions. This can be done in one of two ways:
    1. Check your CPU model's page on the manufacturer's site
    2. Call the `bool AVX256Utils::HasAVX2Support(void)` function
        - As this is a [MASM](https://en.wikipedia.org/wiki/Microsoft_Macro_Assembler) function, it is defined separately in the `avx256utils_asm.asm` file
        - In Visual Studio, this file can be included in your build process by `right clicking your project > Build Dependencies > Build Customisations... > masm` (must be done before any MASM files are imported into the project)
    

2. Create an AVX256 object:
    - An `AVX256<T>` object points to a contiguous 256-bit (32 bytes) of `T` data, the starting address of which is stored in its `T* Data` attribute
    - An AVX256 object can be created in one of four ways:
        1. `AVX256<T>{};`: Allocates 32-bytes of `T` memory (e.g. 32 chars or 8 for floats, etc) and sets its `Data` to point to it. The allocated memory is de-allocated during destruction
        2. `AVX256<T>{dataPtr};`: Sets its `Data` to point to the `T* dataPtr`. No de-allocation is performed during destruction since the data is not 'owned' by the object 
        3. `AVX256<T>{array}`: Allocates 32-bytes of memory for the specified type `T` (e.g. 32 chars or, 8 floats, etc) and sets its `Data` to point to it. The data from `std::array<T, 32 / sizeof(T)> array` is then copied into AVX256's own memory via 256-bit AVX move instructions. The allocated memory is de-allocated during destruction
            - This constructor can also be used with initialiser lists, e.g. `AVX256<double>{{1.0, 2.0, 3.0, 4.0}};`
        4. `AVX256<T>{avx}`: Allocates 32-bytes of memory for the specified type `T` (e.g. 32 chars or, 8 floats, etc) and sets its `Data` to point to it. The data pointed to by `AVX256<T> avx` is then copied into AVX256's own memory via 256-bit AVX move instructions. The allocated memory is de-allocated during destruction
    

3. Call the AVX256 object's methods:
    - An AVX256 object exposes SIMD operations via its methods. There are four types of operations:
        1. Destructive operations (i.e. ones that overwrite the AVX256 object's `Data`) return a reference to the AVX256 object, allowing chained method calls. E.g. `avx1.Clear().Negate().Sub(avx2);`
           - These operations can also be called via assignment operators, e.g. `avx1 += avx2` (addition assignment) is equivalent to `avx1.Add(avx2)` 
        2. Non-destructive operations return their result in an `std::array`. E.g. `avx1.LessThan(avx2)` returns a mask array
           - These operations can also be called via their operators, e.g. `avx1 < avx2` is equivalent to `avx1.LessThan(avx2)` 
        3. Methods that return a single value. E.g. `avx1.IsZero()` or `avx1.Sum()`
        4. Methods that return no values. E.g. `avx1.Next()` or `avx1.Previous()`
    - See the [documentation](https://github.com/tsen-dev/avx256#documentation) below for all available operations
    

4. The items of an AVX256 object can be accessed via the subscript operator (e.g. `avx[15]`), and can be written to an `std::ostream` via the `<<` operator (e.g. `std::cout << AVX256<uint64_t>{{1, 2, 3, 4}}` prints `|1|2|3|4|`)
    

# Demos


![Demo_1](readme_animations/add_demo.gif)


![Demo_2](readme_animations/threshold_demo.gif)


![Demo_3](readme_animations/abs_diff_demo.gif)


![Demo_4](readme_animations/bgr_to_rgb_demo.gif)

# Documentation
##Members
##Methods
###Construction
###Destruction

###Arithmetic

###Math

###Bitwise

###Logical

###Assignment

###Utility



#Limitations
Does it allow use of 3 operands (check dissassembly of threshold demo)

Requires C++17 or above due to the use of `if constexpr (...)` for conditional compilation
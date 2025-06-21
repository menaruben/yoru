# yoru
> ⚠️ WIP
yoru is a c library that aims to add some convenience to C and to make life simpler.
I decided to write this library because I really enjoy writing C, but I often find myself repeating the same code in different projects.

Yoru is not here to save you from C, but to make it easier to use and to get work done. This means that I am adding some features like:
- basic generic data structures without being able to go out of bounds
    - static array
    - linked list / queue
    - stack
    - dynamic array
    - treemap
    - hash map and hash set    
- strings, string builders and other string utilities
- some typedefs i enjoy using like `uint8_t` = `u8`
- allocators
    - arena allocator
    - might add more in the future...
- some math utilities like an actual modulo function (because C's `%` operator is not a modulo operator but a remainder operator)
- and probably a lot more when I think of other things I want to add

# Examples
You can find examples in the [examples](./src/_examples/) folder and run them with `make run-<example_number>`. 

# Installation
You can just copy the content of the `src` folder into your project and include the header file [yoru.h](./src/yoru.h) in your project.

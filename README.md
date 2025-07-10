# yoru
> ⚠️ WIP

yoru is a c library that aims to add some convenience to C and to make life simpler.
I decided to write this library because I really enjoy writing C, but I often find myself repeating the same code in different projects.

Yoru is not here to save you from C, but to make it easier to use and to get work done. If you really want to use a modern C you might want to use
something like Zig instead. ^^

I am adding some features like:
- basic generic data structures without being able to go out of bounds
    - linked list
    - dynamic array / vector
    - trie
    - hash map and hash set (TODO)
    - iterators (TODO) -> might also implement some [fluent interface stuff](https://en.wikipedia.org/wiki/Fluent_interface)
- strings
- simpler string lib (TODO)
- string builders (TODO)
- some typedefs i enjoy using like `uint8_t` = `u8`
- allocators
    - arena allocator
    - heap allocator
- some math utilities like an actual modulo function (because C's `%` operator is not a modulo operator but a remainder operator)
- futures (only working on POSIX compliant OS due to usage of `pthread.h` altough im looking for a more portable solution)
- results and errors (need to be properly integrated with the rest of the library)
- generators sound really cool and interesting! I will probably implement them once I am working on the iterators since they are quite similar. (TODO)

Right now I enforce some behaviour and assert these. If the assertion failed then the program stops.
This is okay for now but I will soon return error codes rather than instantly aborting, making error handling
and debugging easier.. :) Or I will make new functions (that end with `try` or something like that) that return a result or error code instead of asserting. I am not sure yet. 

Tests need to be written and improved. 

# Tests and Examples
You can run the [tests](./src/testing/yoru_tests.c) with `./yoru.py test`. 

[Examples](./src/_examples/) can be run with `./yoru.py demo <demo-number>`. 

# Installation
You can just copy the content of the `src` folder into your project and include the header file [yoru.h](./src/yoru.h) in your project.

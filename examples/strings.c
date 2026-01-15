#define YORU_IMPL
#include "../yoru.h"

#include <stdio.h>

int main() {
  Yoru_GlobalAllocator allocator = yoru_global_allocator_make();
  Yoru_String          s1        = {0};
  Yoru_String          s1_copy   = {0};

  // create stringview of length 20 with initial value of "hello world!"
  if (!yoru_string_from_str(&allocator, "hello world!", 20, &s1)) return 1;
  if (!yoru_string_copy(&allocator, &s1, &s1_copy)) return 2;

  // points to the same string as the s1_copy!
  Yoru_StringView s1_copy_substr = {0};
  if (!yoru_string_substring(&s1_copy, 6, 11, &s1_copy_substr)) return 3;
  s1_copy.data[6]  = 'w';
  s1_copy.data[7]  = 'e';
  s1_copy.data[8]  = 'l';
  s1_copy.data[9]  = 't';
  s1_copy.data[10] = '!';

  // you can also just create a new empty string
  Yoru_String s2 = {0};
  if (!yoru_string_make(&allocator, 69, &s2)) return 4;
  // a string view is just a pointer of u8 with a length. so as long as you are within the
  // boundaries you can just assign u8 values where you want to
  s2.data[0] = 'h';
  s2.data[1] = 'i';

  // you can print the strings by using the string fmt (which is just %.*s) and the args which are the length as int and the ptr to data
  // you need to print them like that because the strings are NOT null terminated in Yoru because the length already provides the length
  // information... this is done so you can also print binary data in the same way and therefore keep consistency
  printf("s1 = `" Yoru_String_Fmt "`\n", Yoru_String_Fmt_Args(&s1));
  printf("transformed s1_copy = `" Yoru_String_Fmt "`\n", Yoru_String_Fmt_Args(&s1_copy));
  printf("s1_copy_substr = `" Yoru_String_Fmt "`\n", Yoru_String_Fmt_Args(&s1_copy_substr));
  printf("s2 = `" Yoru_String_Fmt "`\n", Yoru_String_Fmt_Args(&s2));

  yoru_string_destroy(&s1);
  yoru_string_destroy(&s2);
  return 0;
}

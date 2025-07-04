#include "../yoru.h"

int main(void)
{
    String_t s = String_new("hello world");
    ASSERT_NOT_NULL(s.str);
    ASSERT_EQUAL(s.length, 11); // length does NOT include NULL terminator
}
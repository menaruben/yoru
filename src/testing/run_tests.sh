#/bin/bash

gcc yoru_tests.c -o yoru_tests -std=c99 -Wall && \
./yoru_tests && \
rm yoru_tests


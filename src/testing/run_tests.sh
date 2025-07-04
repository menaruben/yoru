#/bin/bash

gcc yoru_tests.c -o yoru_tests -std=c23 && \
./yoru_tests && \
rm yoru_tests


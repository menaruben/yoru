#!/bin/bash

args=("$@")

option="${args[0]}"

case "$option" in
    "test"|"tests")
        echo "Running all tests..."
        cd src/testing || exit 1
        ./run_tests.sh || exit 1
        cd - || exit 1
        ;;
    "demo")
        demo_num="${args[1]}"
        if [[ -z "$demo_num" ]]; then
            echo "Please specify a demo number (01, 02, 06, etc.)"
            exit 1
        fi
        echo "Running demo $demo_num..."
        cd src/_examples || exit 1
        make "run-$demo_num"
        cd - || exit 1
        ;;
    *)
        echo "Usage: $0 {test|demo <demo_number>}"
        exit 1
        ;;
esac
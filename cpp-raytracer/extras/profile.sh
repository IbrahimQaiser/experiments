#!/bin/sh
g++ ../src/*.cpp -I../include -O3 -ggdb3 -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-missing-field-initializers -o raytracer
valgrind --tool=callgrind ./raytracer
# https://stackoverflow.com/questions/375913/how-do-i-profile-c-code-running-on-linux
gprof2dot -f callgrind callgrind.out.29395 | dot -Tsvg -o output.svg

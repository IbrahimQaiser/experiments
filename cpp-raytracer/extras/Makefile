# A simple makefile for debug builds using clang and various checks
export ASAN_OPTIONS=strict_string_checks=1:detect_stack_use_after_return=1:check_initialization_order=1:strict_init_order=1:detect_leaks=1
CXX=clang++
CXXFLAGS=-W -Wall -Wextra -pedantic -gdwarf-4 -O0 -fsanitize=integer,address,undefined,integer-divide-by-zero,shift,null,return,signed-integer-overflow,float-divide-by-zero,float-cast-overflow,bounds,alignment,vptr,leak  -pedantic -Wno-sign-conversion -Wno-sign-compare -ftrapv -Wno-unused-parameter -Wno-missing-field-initializers -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -fstack-protector
INCLUDE_DIR=../include
SRC_DIR=../src
# May have slow build time because it does not cache individual compilations
build: $(SRC_DIR)/material.cpp $(SRC_DIR)/scene.cpp $(SRC_DIR)/main.cpp $(SRC_DIR)/camera.cpp $(SRC_DIR)/image.cpp $(SRC_DIR)/raytracer.cpp $(SRC_DIR)/objects.cpp 
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) $(SRC_DIR)/material.cpp $(SRC_DIR)/scene.cpp $(SRC_DIR)/main.cpp $(SRC_DIR)/camera.cpp $(SRC_DIR)/image.cpp $(SRC_DIR)/raytracer.cpp $(SRC_DIR)/objects.cpp -o raytracer
run: build 
	./raytracer
clean:
	rm -rf raytracer

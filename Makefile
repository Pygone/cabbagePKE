#no print directory
MAKEFLAGS += --no-print-directory
cmake-flags := -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
NINJA := $(shell command -v ninja 2> /dev/null)
ifneq ($(NINJA),)
	cmake-flags += -GNinja
endif
all:build
.PHONY:build

build:
	@echo "Building cabbageOS"
	@cmake -S . -B build  $(cmake-flags)
	@cmake --build build --target all -- -j 8
	@echo "Done"

clean:
	@echo "Cleaning cabbageOS"
	@if [ -d "build" ]; then rm -r build; fi
	@if [ -d "bin" ]; then rm -r bin; fi
	@if [ -d "lib" ]; then rm -r lib; fi
	@if [ -d "hostfs_root/bin" ]; then rm -r hostfs_root/bin; fi
	@if [ -d "output" ]; then rm -r output; fi
	@if [ -d "obj" ]; then rm -r obj; fi
	@echo "Done"

run: build
	@echo "Running cabbageOs"
	@spike -p2 bin/cabbageOS /bin/sh
	@echo "Done"

.PHONY:build

test: build
	@mkdir -p output
	@echo "Running cabbageOS tests"
	-@spike bin/cabbageOS /bin/c_1_1 > output/c_1_1.txt
	-@spike bin/cabbageOS /bin/c_1_2 > output/c_1_2.txt
	-@spike bin/cabbageOS /bin/c_2_1 > output/c_2_1.txt
	-@spike bin/cabbageOS /bin/c_2_2 > output/c_2_2.txt
	-@spike bin/cabbageOS /bin/c_3_2 > output/c_3_2.txt
	-@spike bin/cabbageOS /bin/c_3_3 > output/c_3_3.txt
	-@spike bin/cabbageOS /bin/c_4_1 > output/c_4_1.txt
	@echo "Done"
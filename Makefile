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
	@echo "Building cabbageOs"
	@cmake -S . -B build  $(cmake-flags)
	@cmake --build build --target all -- -j 8
	@echo "Done"
clean:
	@echo "Cleaning cabbageOs"
	@if [ -d "build" ]; then rm -r build; fi
	@if [ -d "bin" ]; then rm -r bin; fi
	@if [ -d "lib" ]; then rm -r lib; fi
	@if [ -d "hostfs_root/bin" ]; then rm -r hostfs_root/bin; fi
	@echo "Done"

run: build
	@echo "Running cabbageOs"
	@spike bin/cabbageOS /bin/sh
	@echo "Done"
.PHONY:build
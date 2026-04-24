.PHONY: all build test clean validate-phase0 validate-phase1 validate-phase2 \
        validate-phase3 validate-phase4 validate-phase5 validate-phase6

BUILD_DIR := build

all: build

build:
	cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug
	cmake --build $(BUILD_DIR) -- -j$(shell nproc)

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

clean:
	rm -rf $(BUILD_DIR)

validate-phase0:
	cmake --build $(BUILD_DIR) --target validate-phase0

validate-phase1: build
	cmake --build $(BUILD_DIR) --target validate-phase1

validate-phase2: build
	cmake --build $(BUILD_DIR) --target validate-phase2

validate-phase3: build
	cmake --build $(BUILD_DIR) --target validate-phase3

validate-phase4: build
	cmake --build $(BUILD_DIR) --target validate-phase4

validate-phase5: build
	cmake --build $(BUILD_DIR) --target validate-phase5

validate-phase6: build
	cmake --build $(BUILD_DIR) --target validate-phase6

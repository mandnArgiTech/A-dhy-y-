# Story 0.1 — Repository Scaffold & Build System

**Phase:** 0 — Bootstrap & Data Ingestion  
**Difficulty:** Easy  
**Estimated time:** 1–2 hours  
**Depends on:** Nothing

---

## Objective

Establish a clean, compilable project skeleton so every subsequent story has a working build to add into.

---

## Context

The repository at `https://github.com/mandnArgiTech/A-dhy-y-.git` starts empty. This story creates the full directory tree, CMake build system, clang-format config, and verifies the build compiles to zero errors and zero warnings.

---

## Tasks

### 1. Verify directory structure exists
The following directories must be present (created by repo init):
```
core/phonology/
core/sutrapatha/
core/metadata/
core/conflict/
ancillary/dhatupatha/
ancillary/ganapatha/
ancillary/unadipatha/
ancillary/linganushasana/
prakriya/tinanta/
prakriya/subanta/
prakriya/krit/
prakriya/taddhita/
sandhi/
samasa/
encoding/
tests/unit/
tests/data/
tests/regression/
tools/
examples/
vendor/
data/
include/
.cursor/stories/
```
Create any that are missing. Add a `.gitkeep` in each empty leaf directory.

### 2. Download Unity test framework
```bash
mkdir -p tests/unity
curl -L https://raw.githubusercontent.com/ThrowTheSwitch/Unity/master/src/unity.c \
  -o tests/unity/unity.c
curl -L https://raw.githubusercontent.com/ThrowTheSwitch/Unity/master/src/unity.h \
  -o tests/unity/unity.h
curl -L https://raw.githubusercontent.com/ThrowTheSwitch/Unity/master/src/unity_internals.h \
  -o tests/unity/unity_internals.h
```

### 3. Create a stub `include/ashtadhyayi.h` placeholder if missing
The file already exists in this repo. Verify it is present.

### 4. Create minimal stub source files so CMake can link

Create `encoding/encoding.c`:
```c
#include "encoding.h"
const char *ash_version(void) { return "0.1.0"; }
char *ash_encode(const char *input, int from, int to) {
  (void)from; (void)to;
  /* stub — implemented in Story 1.3 */
  if (!input) return NULL;
  char *out = strdup(input);
  return out;
}
```

Create `encoding/encoding.h`:
```c
#ifndef ENCODING_H
#define ENCODING_H
#include "ashtadhyayi.h"
char *ash_encode(const char *input, ASH_Encoding from, ASH_Encoding to);
#endif
```

Create stub `.c` files for every module listed in `CMakeLists.txt`'s `ALL_SRC`:
- Each stub must at minimum `#include` its header and define one empty function so the compiler does not error.
- Pattern for each stub:
  ```c
  /* <module>.c — stub; implemented in Story <X.Y> */
  #include "<module>.h"
  /* Implementation pending */
  ```

### 5. Create minimal first unit test

Create `tests/unit/test_build.c`:
```c
#include "unity.h"
#include "ashtadhyayi.h"

void setUp(void) {}
void tearDown(void) {}

void test_version(void) {
  TEST_ASSERT_NOT_NULL(ash_version());
  TEST_ASSERT_EQUAL_STRING("0.1.0", ash_version());
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_version);
  return UNITY_END();
}
```

### 6. Build and verify

Run:
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -- -j$(nproc)
ctest --test-dir build --output-on-failure
```

---

## Acceptance Criteria

- [ ] `cmake --build build` exits with code 0
- [ ] Zero compiler errors
- [ ] Zero compiler warnings (all flags: `-Wall -Wextra -Werror -pedantic`)
- [ ] `ctest --test-dir build` shows `test_build` PASSED
- [ ] `clang-format --dry-run --Werror` passes on all `.c` and `.h` files

---

## Notes for Cursor

- The `CMakeLists.txt` uses `if(EXISTS ...)` guards so stub files are enough — no need to implement functions yet.
- Do **not** remove the `AGENTS.md` or `.cursor/stories/` directory.
- If CMake version < 3.18 is installed, update the minimum version line temporarily.

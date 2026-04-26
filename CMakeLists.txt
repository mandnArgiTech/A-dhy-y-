cmake_minimum_required(VERSION 3.18)
project(AshtadhyayiLib VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags
add_compile_options(-Wall -Wextra -Werror -pedantic)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-g -fsanitize=address,undefined)
  add_link_options(-fsanitize=address,undefined)
endif()

include_directories(include core core/phonology core/sutrapatha core/metadata core/conflict ancillary prakriya sandhi samasa encoding)

# ── Source files (populated incrementally per story) ─────────────────────────

set(ENCODING_SRC
    encoding/encoding.c
)

set(PHONOLOGY_SRC
    core/phonology/varna.c
    core/phonology/pratyahara.c
)

set(SANDHI_SRC
    sandhi/sandhi_vowel.c
    sandhi/sandhi_hal.c
    sandhi/sandhi_visarga.c
)

set(SUTRAPATHA_SRC
    core/sutrapatha/sutra_loader.c
)

set(METADATA_SRC
    core/metadata/samjna.c
    core/metadata/anubandha.c
    core/metadata/adhikara.c
    core/metadata/anuvrtti.c
    core/metadata/paribhasha.c
)

set(CONFLICT_SRC
    core/conflict/conflict.c
)

set(PRAKRIYA_SRC
    prakriya/term.c
    prakriya/context.c
    prakriya/pipeline.c
    prakriya/tinanta/lakara.c
    prakriya/tinanta/ting.c
    prakriya/tinanta/lat_bhvadi.c
    prakriya/tinanta/vikaranas.c
    prakriya/subanta/vibhakti.c
    prakriya/subanta/a_stem.c
    prakriya/subanta/aaiu_stems.c
    prakriya/subanta/consonant_stems.c
    prakriya/subanta/karaka.c
    prakriya/krit/krit_primary.c
    prakriya/taddhita/taddhita.c
    prakriya/guna_vrddhi.c
)

set(ANCILLARY_SRC
    ancillary/dhatupatha/dhatupatha.c
    ancillary/ganapatha/ganapatha.c
    ancillary/unadipatha/unadi.c
)

set(SAMASA_SRC
    samasa/samasa.c
)

# Collect only existing sources (stories are added incrementally)
set(ALL_SRC)
foreach(src IN LISTS ENCODING_SRC PHONOLOGY_SRC SANDHI_SRC SUTRAPATHA_SRC
        METADATA_SRC CONFLICT_SRC PRAKRIYA_SRC ANCILLARY_SRC SAMASA_SRC)
  if(EXISTS "${CMAKE_SOURCE_DIR}/${src}")
    list(APPEND ALL_SRC ${src})
  endif()
endforeach()

# ── Library target ─────────────────────────────────────────────────────────
add_library(ashtadhyayi STATIC ${ALL_SRC})
target_include_directories(ashtadhyayi PUBLIC include)

# ── Demo executable ────────────────────────────────────────────────────────
if(EXISTS "${CMAKE_SOURCE_DIR}/examples/demo.c")
  add_executable(ash_demo examples/demo.c)
  target_link_libraries(ash_demo PRIVATE ashtadhyayi)
endif()

# ── Unity test framework ───────────────────────────────────────────────────
add_library(unity STATIC tests/unity/unity.c)
target_include_directories(unity PUBLIC tests/unity)

# ── Unit tests (auto-discover) ─────────────────────────────────────────────
enable_testing()
file(GLOB UNIT_TEST_SRCS tests/unit/test_*.c)
foreach(test_src IN LISTS UNIT_TEST_SRCS)
  get_filename_component(test_name ${test_src} NAME_WE)
  add_executable(${test_name} ${test_src})
  target_link_libraries(${test_name} PRIVATE ashtadhyayi unity)
  add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

# ── Phase validation targets ───────────────────────────────────────────────
add_custom_target(validate-phase0
  COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/ingest_source.py --validate
  COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/ingest_dhatupatha.py --validate
  COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/ingest_ganapatha.py --validate
  COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/ingest_pratyahara.py --validate
  COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/ingest_shabda.py --validate
  COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/ingest_dhatuforms.py --validate
  COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/ingest_shabdaprakriya.py --validate
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Phase 0: Validating data ingestion (4 source files + 3 oracle files)"
)

add_custom_target(validate-phase1
  COMMAND ${CMAKE_CTEST_COMMAND} -R "test_varna|test_pratyahara|test_encoding|test_sandhi"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Phase 1: Validating phonology engine"
)

add_custom_target(validate-phase2
  COMMAND ${CMAKE_CTEST_COMMAND} -R "test_sutra|test_samjna|test_anubandha|test_adhikara|test_anuvrtti|test_paribhasha"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Phase 2: Validating sutra engine"
)

add_custom_target(validate-phase3
  COMMAND ${CMAKE_CTEST_COMMAND} -R "test_term|test_conflict|test_lakara|test_guna|test_lat"
  COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/run_vyakarana_oracle.py --validate
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Phase 3: Validating tinanta derivation"
)

add_custom_target(validate-phase4
  COMMAND ${CMAKE_CTEST_COMMAND} -R "test_vibhakti|test_a_stem|test_aaiu|test_consonant|test_karaka"
  COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/run_scl_oracle.py --validate
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Phase 4: Validating subanta declension"
)

add_custom_target(validate-phase5
  COMMAND ${CMAKE_CTEST_COMMAND} -R "test_pipeline\\|test_samasa\\|test_krit\\|test_taddhita\\|test_unadi" --output-on-failure
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Phase 5: Validating pipeline and derivatives"
)

add_custom_target(validate-phase6
  COMMAND ${CMAKE_CTEST_COMMAND}
  COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/coverage_report.py
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Phase 6: Full validation and coverage report"
)

# ── Install ────────────────────────────────────────────────────────────────
install(TARGETS ashtadhyayi DESTINATION lib)
install(FILES include/ashtadhyayi.h DESTINATION include)

# ── pkg-config ─────────────────────────────────────────────────────────────
configure_file(libashtadhyayi.pc.in libashtadhyayi.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/libashtadhyayi.pc DESTINATION lib/pkgconfig)

# single tuto
cmake_minimum_required(VERSION 3.7.2)
set(TARGET_NAME referenced_unreferenced_actor.bin)

include("../../common_simplx.cmake")

simplx_core_set_cxx_flags()

set(SOURCE_FILES    ${CMAKE_CURRENT_SOURCE_DIR}/ref_unref_actor.cpp

                )

simplx_core_add_topdir(src/simplx_core)

add_executable(${TARGET_NAME} ${SOURCE_FILES})
target_link_libraries(${TARGET_NAME} engine)

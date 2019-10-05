# single timer tuto
cmake_minimum_required(VERSION 3.7.2)
set(TARGET_NAME tuto_timer.bin)

include("../../common_simplx.cmake")

simplx_core_set_cxx_flags()

list(APPEND SOURCE_FILES    ${CMAKE_CURRENT_SOURCE_DIR}/timer.cpp
                        
                        )


simplx_core_add_topdir("src/simplx_core")
simplx_core_add_topdir("src/pattern/timer")

# create executable
add_executable(${TARGET_NAME} ${SOURCE_FILES})
target_link_libraries(${TARGET_NAME} engine timer)

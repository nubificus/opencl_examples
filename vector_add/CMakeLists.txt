include_directories ("${PROJECT_SOURCE_DIR}/include" ${OpenCL_INCLUDE_DIRS})

add_library(vector_add SHARED vector_add.cpp)
target_compile_options(vector_add PUBLIC -Wall -Wextra )
set_property(TARGET vector_add PROPERTY LINK_FLAGS "-lOpenCL -shared")



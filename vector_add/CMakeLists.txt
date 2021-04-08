include_directories ("${PROJECT_SOURCE_DIR}/include" ${OpenCL_INCLUDE_DIRS})

add_library(vector_add SHARED vector_add.cpp)
target_compile_options(vector_add PUBLIC -Wall -Wextra )
set_target_properties(vector_add PROPERTIES ENABLE_EXPORTS on)
target_link_libraries(vector_add  ${OpenCL_LIBRARIES})
set_property(TARGET vector_add PROPERTY LINK_FLAGS "-lOpenCL -shared")



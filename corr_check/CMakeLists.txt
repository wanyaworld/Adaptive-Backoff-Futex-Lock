project(corr_check)

# Create the executable
add_executable(${PROJECT_NAME} main.cpp utils.cpp bench.cpp)

# Link the static library from subproject1 using it's alias sub::lib1
# Link the header only library from subproject2 using it's alias sub::lib2
# This will cause the include directories for that target to be added to this project
message("${PROJECT_SOURCE_DIR}/../")
target_include_directories(${PROJECT_NAME} PRIVATE
  "${PROJECT_SOURCE_DIR}/../"
)
target_link_libraries(${PROJECT_NAME}
    futex_lock
    pthread
)

# Set the project name
project (futex_lock)

# Add a library with the above sources
add_library(${PROJECT_NAME} SHARED src/futex_lock.cpp)

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries( ${PROJECT_NAME}
    pthread
)

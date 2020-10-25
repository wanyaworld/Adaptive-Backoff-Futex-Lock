cmake_minimum_required (VERSION 3.5)

project(futex)

# Add sub directories
add_subdirectory(futex_backoff_test)
add_subdirectory(locks_test)
add_subdirectory(futex_lock)
add_subdirectory(corr_check)

cmake_minimum_required(VERSION 3.16)
project(StudentGradeManagement LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(student_core
    src/Student.cpp
    src/Storage.cpp
    src/InputValidation.cpp
)
target_include_directories(student_core PUBLIC include)

add_executable(student_grade_manager
    src/main.cpp
    src/GradeBook.cpp
)
target_link_libraries(student_grade_manager PRIVATE student_core)

add_executable(storage_tests tests/storage_tests.cpp)
target_link_libraries(storage_tests PRIVATE student_core)

add_executable(member2_tests tests/member2_tests.cpp)
target_link_libraries(member2_tests PRIVATE student_core)

enable_testing()
add_test(NAME storage_tests COMMAND storage_tests)
add_test(NAME member2_tests COMMAND member2_tests)

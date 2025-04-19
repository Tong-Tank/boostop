
# set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR}/../../dependence)


find_package(Boost COMPONENTS program_options)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER g++-13)
set(CMAKE_C_COMPILER gcc-13)

include_directories(
    ${Boost_INCLUDE_DIRS}
)

add_executable( sdc-smi main.cpp test.cpp optionHandler.cpp)
add_executable( test example.cpp)

target_link_libraries(sdc-smi
    ${Boost_PROGRAM_OPTIONS_LIBRARY} pthread
)

target_link_libraries(test
    ${Boost_PROGRAM_OPTIONS_LIBRARY} pthread
)

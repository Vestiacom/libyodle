## Sources #####################################################################
file(GLOB_RECURSE SRC ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE TESTS_SRC *.cpp)

enable_testing()

## Setup target ################################################################
set(UT_PROJECT_NAME "${PROJECT_NAME}-tests")
add_executable(${UT_PROJECT_NAME} ${SRC} ${TESTS_SRC})

## Defines #####################################################################

## Link libraries ##############################################################
find_package(GTest REQUIRED)

include_directories(${SRC_DIR} ${TESTS_DIR})
include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})


message(STATUS ${GTEST_BOTH_LIBRARIES})
message(STATUS ${GTEST_LIBRARIES})
message(STATUS ${GTEST_MAIN_LIBRARIES})
target_link_libraries(${UT_PROJECT_NAME} ev.a pthread gtest )


## Integrate with CTest runner #################################################
add_test(NAME    libyodle-unittests 
         COMMAND ${UT_PROJECT_NAME})

## Install #####################################################################
install(TARGETS ${UT_PROJECT_NAME}
        COMPONENT tests
        DESTINATION bin)

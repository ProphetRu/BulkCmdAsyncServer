cmake_minimum_required(VERSION 3.10)

#--------------------------------------------------------------------------------------------------
# variables
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)
set(PROJECT_NAME BulkCmdAsyncServer)
set(TEST_NAME UnitTest)


#--------------------------------------------------------------------------------------------------
# project
project(${PROJECT_NAME})

if (MSVC)
	# enable VS-Solution filters support
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif (MSVC)


#--------------------------------------------------------------------------------------------------
# builds
add_executable(${PROJECT_NAME}	
	${SRC_DIR}/CommandHandler.cpp
	${SRC_DIR}/CommandReceiverSafe.cpp
	${SRC_DIR}/LogerAsync.cpp
	${SRC_DIR}/async.cpp
	${SRC_DIR}/main.cpp
)

add_executable(${TEST_NAME} 
	${TEST_DIR}/tests.cpp
)

# properties
set_target_properties(${PROJECT_NAME} ${TEST_NAME} 
	PROPERTIES
	CXX_STANDARD 23
	CXX_STANDARD_REQUIRED ON
)


#--------------------------------------------------------------------------------------------------
# gtest
#
find_package(GTest REQUIRED)

# add src to the include directories for the test target
target_include_directories(${TEST_NAME} 
	PRIVATE
	${SRC_DIR}
	${GTEST_INCLUDE_DIRS}
	${GTEST_LIBRARY} 
	${GTEST_INCLUDE_DIR}
	${GTEST_MAIN_LIBRARY}
)

# link test target against gtest libraries
target_link_libraries(${TEST_NAME} 
	PRIVATE
	GTest::gtest 
	GTest::gtest_main 
	#GTest::gmock 
	#GTest::gmock_main
)


# boost
#
SET(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS system)
if(Boost_FOUND)
    target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()


#--------------------------------------------------------------------------------------------------
# compiler settings
if (MSVC)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

	add_compile_options("/std:c++latest")
	
	target_compile_options(${PROJECT_NAME} PRIVATE
		/W4
	)
	target_compile_options(${TEST_NAME} PRIVATE
		/W4
	)
else ()
	target_compile_options(${PROJECT_NAME} PRIVATE
		-std=c++23 -Wall -Wextra -pedantic -Werror -lboost_regex
	)
	target_compile_options(${TEST_NAME} PRIVATE
		-std=c++23 -Wall -Wextra -pedantic -Werror
	)
endif (MSVC)


#--------------------------------------------------------------------------------------------------
# deb pkg
if (UNIX)
	install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

	set(CPACK_GENERATOR DEB)

	set(CPACK_PACKAGE_VERSION_MAJOR "1")
	set(CPACK_PACKAGE_VERSION_MINOR "0")
	set(CPACK_PACKAGE_VERSION_PATCH "0")

	set(CPACK_PACKAGE_CONTACT 155024765+ProphetRu@users.noreply.github.com)

	include(CPack)
endif (UNIX)


#--------------------------------------------------------------------------------------------------
# testing
enable_testing()
include(GoogleTest)
gtest_discover_tests(${TEST_NAME})

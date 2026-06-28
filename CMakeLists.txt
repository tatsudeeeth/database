cmake_minimum_required(VERSION 3.23)
project (database VERSION 2.0)

add_library(database_compiler_flags INTERFACE)
target_compile_features(database_compiler_flags INTERFACE cxx_std_17)

configure_file(database_config.h.in database_config.h)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

#find_package(sqlite)
#include_directories("C:/Users/TatsuyaSano/01_cpp/sqlite/out/install/include")

include_directories("C:/Users/TatsuyaSano/01_cpp/sqlite/out/install/include")
link_directories("C:/Users/TatsuyaSano/01_cpp/sqlite/out/install/lib")

include_directories("C:/Program Files/MariaDB/MariaDB Connector C 64-bit/include")
link_directories("C:/Program Files/MariaDB/MariaDB Connector C 64-bit/lib")

option(BUILD_SHARED_LIBS OFF)
add_subdirectory(base)
find_package(loader)

add_executable(tester "tester.cpp"
		test_common.cpp)


target_link_libraries(tester
		ssr_connector
		lite_connector
		mdb_connector
		database_compiler_flags)

if(CMAKE_BUILD_TYPE MATCHES Debug)
	file(COPY origin.sqlite3 database.sqlite3 proto.csv book1.csv DESTINATION "${PROJECT_BINARY_DIR}/bin")
	option(USE_mariaDB OFF)

	option(FROM_ORIGIN OFF)

	option(INSERT_WITH_PROCEDURE OFF)
	if(INSERT_WITH_PROCEDURE)
		target_compile_definitions(tester PRIVATE INSERT_WITH_PROCEDURE)
	endif()

	if(FROM_ORIGIN)
		target_compile_definitions(tester PRIVATE FROM_ORIGIN)
	endif()

	option(INSERT_TEST OFF)
	if(INSERT_TEST)
		target_compile_definitions(tester PRIVATE INSERT_TEST)
	endif()

	option(SELECT_TEST OFF)
	if(SELECT_TEST)
		target_compile_definitions(tester PRIVATE SELECT_TEST)
	endif()

	option(UPDATE_TEST OFF)
	if(UPDATE_TEST)
		target_compile_definitions(tester PRIVATE UPDATE_TEST)
	endif()

	if(USE_mariaDB)
		target_compile_definitions(tester PRIVATE USE_MARIADB)
	endif()

endif()

target_include_directories(tester PUBLIC "${PROJECT_BINARY_DIR}")


# add_executable(uploader uploader.cpp)
# if(BUILD_DLL MATCHES ON)
# 	target_compile_definitions(uploader PRIVATE BUILD_DLL)
# endif()

# target_link_libraries(uploader loader database_compiler_flags)
# target_include_directories(uploader PUBLIC "${PROJECT_BINARY_DIR}")

set(installable_libs db_connector ssr_connector lite_connector mdb_connector database_compiler_flags)

install(
		TARGETS ${installable_libs}
		EXPORT DatabaseTargets
		DESTINATION lib
)

install(
	EXPORT DatabaseTargets
	FILE DatabaseTargets.cmake
	DESTINATION "cmake"
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
	"${CMAKE_CURRENT_BINARY_DIR}/DatabaseConfig.cmake"
	INSTALL_DESTINATION "cmake/example"
	NO_SET_AND_CHECK_MACRO
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install(FILES
		${CMAKE_CURRENT_BINARY_DIR}/DatabaseConfig.cmake
		DESTINATION "cmake"
  )

export(EXPORT DatabaseTargets FILE "${CMAKE_CURRENT_BINARY_DIR}/DatabaseTargets.cmake")


#include(InstallRequiredSystemLibraries)
#set(CPACK_SOURCE_GENERATOR "TGZ")
#include(CPack)

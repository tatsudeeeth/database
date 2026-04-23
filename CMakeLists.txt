cmake_minimum_required(VERSION 3.23)
project (database VERSION 2.0)

add_library(database_compiler_flags INTERFACE)
target_compile_features(database_compiler_flags INTERFACE cxx_std_17)

configure_file(database_config.h.in database_config.h)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")


include_directories("")
link_directories("")

include_directories("")
link_directories("")

option(BUILD_SHARED_LIBS OFF)
add_subdirectory(base)



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


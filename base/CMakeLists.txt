add_library(db_connector
		STATIC
		#user.cpp
		write_log.cpp
		#cursor.cpp
		id_mgr.cpp
        db_connector.cpp
		constants.cpp
        str_function.cpp
        str_function.h
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
	target_compile_definitions(db_connector PUBLIC DEBUG)
endif()

if(BUILD_SHARED_LIBS)
	target_compile_definitions(db_connector PUBLIC BUILD_DLL)
endif()

target_include_directories(db_connector INTERFACE
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
$<INSTALL_INTERFACE:include>
)

target_link_libraries(db_connector PUBLIC database_compiler_flags)

add_subdirectory(ssr_conn)
add_subdirectory(lite_conn)
add_subdirectory(mdb_conn)
install(FILES db_connector.h constants.h DESTINATION include)

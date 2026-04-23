add_library(lite_connector

        lite_func.cpp
        lite_connector.cpp
)

target_include_directories(lite_connector INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(lite_connector PUBLIC
        db_connector
        sqlite
        database_compiler_flags)

install(FILES lite_connector.h DESTINATION include)

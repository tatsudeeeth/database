add_library(ssr_connector
        ssr_connector.cpp
        ssr_func.cpp
)

target_include_directories(ssr_connector INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(ssr_connector PUBLIC
        odbc32
        db_connector
        database_compiler_flags)

install(FILES ssr_connector.h DESTINATION include)

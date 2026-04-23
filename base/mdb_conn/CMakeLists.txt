add_library(mdb_connector

        mdb_connector.cpp
        mdb_func.cpp
)

target_link_libraries(mdb_connector PUBLIC
        db_connector
        libmariadb
        database_compiler_flags
)

target_include_directories(mdb_connector INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

install(FILES mdb_connector.h DESTINATION include)
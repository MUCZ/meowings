aux_source_directory(. SRC_LIST)

add_executable(Loadtest ${SRC_LIST})
target_link_libraries(Loadtest muduo_net muduo_base pthread mysqlclient)

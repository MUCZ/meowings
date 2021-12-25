
aux_source_directory(. SRC_LIST) # 该目录下所有源文件

add_executable(ChatServer ${SRC_LIST})
target_link_libraries(ChatServer muduo_net muduo_base pthread mysqlclient)

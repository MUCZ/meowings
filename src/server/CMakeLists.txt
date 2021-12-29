
aux_source_directory(. SRC_LIST) # all files in current folder
aux_source_directory(./model SRC_LIST) 

add_executable(ChatServer ${SRC_LIST})
target_link_libraries(ChatServer muduo_net muduo_base pthread mysqlclient hiredis)

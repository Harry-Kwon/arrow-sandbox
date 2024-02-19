add_executable(test test.m.cpp)
target_link_libraries(test PRIVATE arrow_shared)

add_executable(builder builder.m.cpp)
target_link_libraries(builder PRIVATE arrow_shared)

add_executable(struct struct.m.cpp)
target_link_libraries(struct PRIVATE arrow_shared)

add_executable(struct-with-list struct-with-list.m.cpp)
target_link_libraries(struct-with-list PRIVATE arrow_shared)

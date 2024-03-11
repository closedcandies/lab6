add_executable(hamarc main.cpp)

target_link_libraries(hamarc PUBLIC HamArc)
target_link_libraries(hamarc PUBLIC ArgParser)
target_include_directories(hamarc PUBLIC ${PROJECT_SOURCE_DIR})

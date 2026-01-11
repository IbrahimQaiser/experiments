project(game_of_life)


add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${raylib_SOURCE_DIR}/raylib/include)

target_sources(${PROJECT_NAME}
    PRIVATE
        main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE grid)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
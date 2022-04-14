add_executable(apple main.cpp ${GLAD_SOURCES} ${IMGUI_SOURCES})

target_link_libraries(apple PRIVATE glfw3)

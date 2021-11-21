cmake_minimum_required(VERSION 3.12)

project(Pacman2D_group5)

find_package(OpenGL REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(external/glfw)
add_subdirectory(external/glad)
add_subdirectory(external/glm)

add_executable(
	${PROJECT_NAME}
	main.cpp
	camera.cpp
	camera.h
	map.cpp
	map.h
	ebo.cpp
	ebo.h
	entity.cpp
	entity.h
	vao.cpp
	vao.h
	vbo.cpp
	vbo.h
	shaderClass.cpp
	shaderClass.h)

target_link_libraries(
	${PROJECT_NAME}
	glfw
	glm
  	glad
	OpenGL::GL)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/levels/level0
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/levels)

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_LIST_DIR}/resources
  ${CMAKE_CURRENT_BINARY_DIR}/resources)

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/shaders
  ${CMAKE_CURRENT_BINARY_DIR}/shaders)

set(CMAKE_LEGACY_CYGWIN_WIN32 OFF)
project(Nova-Engine C CXX)
cmake_minimum_required(VERSION 3.9)


#set(SHADER_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/Nova/Nova")
set(NOVA_INCLUDE_DIRECTORY "${CMAKE_SOURCE_DIR}/Nova")
set(EXAMPLES_BINARY_DIR "${CMAKE_BINARY_DIR}/Examples/bin")
set(NOVA_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Nova/lib")
set(VENDOR_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/Nova/vendor/spdlog-1.3.1/include"
								"${CMAKE_SOURCE_DIR}/Nova/vendor/glfw-3.3/include"
								"${CMAKE_SOURCE_DIR}/Nova/vendor/GLAD/include"
								"${CMAKE_SOURCE_DIR}/Nova/vendor/imgui"
								"${CMAKE_SOURCE_DIR}/Nova/vendor/glm"
								"${CMAKE_SOURCE_DIR}/Nova/vendor/stb")
								
	

add_subdirectory(Nova)
add_subdirectory(Examples)

#SET(CONTENTS "")
#FOREACH(line ${SHADER_INCLUDE_DIRECTORIES})
#   SET(CONTENTS "${CONTENTS}${line}\n")
#ENDFOREACH(line)
#file(WRITE shader_includes_directories.txt ${CONTENTS})


file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/HexMap/HexMap-Assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
#Firt Example
file(GLOB_RECURSE examples_textures "${CMAKE_CURRENT_SOURCE_DIR}/Assets/Textures/*")
file(GLOB_RECURSE examples_shaders  "${CMAKE_CURRENT_SOURCE_DIR}/Assets/Shaders/*")
source_group("Example-Assets" FILES ${examples_shaders} ${examples_textures})
add_executable(FirstExample FirstExample.cpp
							${examples_shaders}
							${examples_textures})


set_target_properties(FirstExample
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${EXAMPLES_BINARY_DIR}
)


target_link_libraries(FirstExample PRIVATE Nova)
target_include_directories(FirstExample PRIVATE ${NOVA_INCLUDE_DIRECTORY} ${VENDOR_INCLUDE_DIRECTORIES})
target_compile_definitions(FirstExample PRIVATE "$<$<CONFIG:DEBUG>:NOVA_DEBUG>")
target_compile_definitions(FirstExample PRIVATE "$<$<CONFIG:RELEASE>:NOVA_RELEASE>")

add_custom_target(copyFirstExampleAssets
COMMAND ${CMAKE_COMMAND} -E copy_if_different ${examples_shaders}  "${CMAKE_CURRENT_BINARY_DIR}/Assets/Shaders"
#COMMAND ${CMAKE_COMMAND} -E copy_if_different ${examples_textures}  "${CMAKE_CURRENT_BINARY_DIR}/Assets/Textures"
)
add_dependencies(FirstExample copyFirstExampleAssets)

#HExMap					  
file(GLOB_RECURSE hexmap_textures "${CMAKE_CURRENT_SOURCE_DIR}/HexMap/HexMap-Assets/Textures/*")
file(GLOB_RECURSE hexmap_shaders  "${CMAKE_CURRENT_SOURCE_DIR}/HexMap/HexMap-Assets/Shaders/*") 
source_group("HexMap-Assets" FILES ${hexmap_shaders} ${hexmap_textures})
add_executable(HexMap	HexMap/HexMap.h
						HexMap/HexMap.cpp
						HexMap/HexApp.cpp
						HexMap/HexLayer.h
						HexMap/HexLayer.cpp
						HexMap/HexMapBrush.h
						HexMap/HexMapBrush.cpp
						HexMap/HexMapCameraController.h
						HexMap/HexMapCameraController.cpp
						HexMap/HexTile.h
						HexMap/HexUtility.h
						${hexmap_shaders}
						${hexmap_textures}
			)
						


set_target_properties(HexMap
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${EXAMPLES_BINARY_DIR}
)
target_link_libraries(HexMap PRIVATE Nova)
target_include_directories(HexMap PRIVATE ${NOVA_INCLUDE_DIRECTORY} ${VENDOR_INCLUDE_DIRECTORIES})
target_compile_definitions(HexMap PRIVATE "$<$<CONFIG:DEBUG>:NOVA_DEBUG>")
target_compile_definitions(HexMap PRIVATE "$<$<CONFIG:RELEASE>:NOVA_RELEASE>")
add_custom_target(copyHexMapAssets
COMMAND ${CMAKE_COMMAND} -E copy_if_different ${hexmap_shaders}  "${CMAKE_CURRENT_BINARY_DIR}/HexMap-Assets/Shaders"
COMMAND ${CMAKE_COMMAND} -E copy_if_different ${hexmap_textures}  "${CMAKE_CURRENT_BINARY_DIR}/HexMap-Assets/Textures"
)

add_dependencies(HexMap copyHexMapAssets)


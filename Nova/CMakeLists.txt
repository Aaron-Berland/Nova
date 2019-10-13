add_subdirectory(vendor/glfw-3.3)
add_subdirectory(vendor/GLAD)
add_subdirectory(vendor/imgui)
add_subdirectory(vendor/stb)
#TODO(BYRON): This will need to be way more complex after adding multiple platforms
set(NOVA_PRIVATE_LIBRARIES glfw Glad ImGui stb)
set(NOVA_UTILITY
		Nova/Utility/Collision.h
		Nova/Utility/Geometry.h
		Nova/Utility/CameraController.h
		Nova/Utility/CameraController.cpp
	)
set(NOVA_RENDERER
		Nova/Renderer/GraphicsContext.h
		Nova/Renderer/Shader.h
		Nova/Renderer/Shader.cpp
		Nova/Renderer/Renderer.h
		Nova/Renderer/Renderer.cpp
		Nova/Renderer/Buffer.h
		Nova/Renderer/Buffer.cpp
		Nova/Renderer/VertexArray.h
		Nova/Renderer/VertexArray.cpp
		Nova/Renderer/RendererAPI.h
		Nova/Renderer/RendererAPI.cpp
		Nova/Renderer/RenderCommand.h
		Nova/Renderer/RenderCommand.cpp
		Nova/Renderer/Camera.h
		Nova/Renderer/Camera.cpp
		Nova/Renderer/Texture.h
		Nova/Renderer/Texture.cpp
		
		)
set(NOVA_HEADERS 
		Nova/Core.h
		Nova/Application.h
		Nova/EntryPoint.h
		Nova/Log.h
		Nova/Window.h
		Nova/Layer.h
		Nova/LayerStack.h
		Nova/Events/Event.h
		Nova/Events/KeyEvent.h
		Nova/Events/MouseEvent.h
		Nova/Events/ApplicationEvent.h
		Nova/Input.h
		Nova/KeyCodes.h
		Nova/MouseButtonCodes.h
		Nova/Core/Timestep.h
		Nova/Core/Benchmarking.h

	)
set(NOVA_SOURCES
		Nova/Window.cpp
		Nova/Application.cpp
		Nova/Log.cpp
		Nova/Layer.cpp
		Nova/LayerStack.cpp
		Nova/Input.cpp)
		
set(NOVA_PLATFORM_SPECIFIC
		Nova/Platform/GLFWWindow.cpp
		Nova/Platform/GLFWWindow.h
		Nova/Platform/ImGuiOpenGLBuild.cpp
		Nova/Platform/ImGuiOpenGLLayer.h
		Nova/Platform/ImGuiOpenGLLayer.cpp
		Nova/Platform/GLFWInput.h
		Nova/Platform/GLFWInput.cpp	
		Nova/Platform/OpenGLContext.h
		Nova/Platform/OpenGLContext.cpp
		Nova/Platform/OpenGLBuffer.h
		Nova/Platform/OpenGLBuffer.cpp
		Nova/Platform/OpenGLVertexArray.h
		Nova/Platform/OpenGLVertexArray.cpp
		Nova/Platform/OpenGLRendererAPI.h
		Nova/Platform/OpenGLRendererAPI.cpp
		Nova/Platform/OpenGLShader.h
		Nova/Platform/OpenGLShader.cpp
		Nova/Platform/OpenGLTexture.h
		Nova/Platform/OpenGLTexture.cpp
	)
set(NOVA_IMGUI
		Nova/ImGui/ImGuiLayer.h
		Nova/ImGui/ImGuiLayer.cpp)
source_group("Nova-Headers" FILES ${NOVA_HEADERS} )
source_group("Nova-Sources" FILES ${NOVA_SOURCES} )
source_group("Nova-Platform-Specific" FILES ${NOVA_PLATFORM_SPECIFIC})
source_group("Nova-ImGui" FILES ${NOVA_IMGUI})
source_group("Nova-Engine-Header" FILES Nova.h)
source_group("Nova-Renderer" FILES ${NOVA_RENDERER})
source_group("Nova-Utility" FILES ${NOVA_UTILITY})
add_library(Nova STATIC ${NOVA_SOURCES} ${NOVA_HEADERS} ${NOVA_PLATFORM_SPECIFIC}  ${NOVA_IMGUI} ${NOVA_RENDERER} ${NOVA_UTILITY} Nova.h)
#TODO(BYRON): This will need to be way more complex after adding multiple platforms
find_package(OpenGL REQUIRED)
list(APPEND NOVA_PRIVATE_LIBRARIES ${OPENGL_LIBRARIES})
target_include_directories(Nova PRIVATE ${NOVA_INCLUDE_DIRECTORY} ${VENDOR_INCLUDE_DIRECTORIES})
target_link_libraries(Nova 
						PRIVATE ${NOVA_PRIVATE_LIBRARIES})
#target_compile_definitions(Nova PRIVATE "NOVA_BUILD_DLL")
target_compile_definitions(Nova PRIVATE "$<$<CONFIG:DEBUG>:NOVA_DEBUG>")
target_compile_definitions(Nova PRIVATE "$<$<CONFIG:RELEASE>:NOVA_RELEASE>")
set_target_properties(Nova
						PROPERTIES
						RUNTIME_OUTPUT_DIRECTORY ${NOVA_LIBRARY_DIR}
						LIBRARY_OUTPUT_DIRECTORY ${NOVA_LIBRARY_DIR})

if (WIN32)
# copy the .dll file to the same folder as the executable
	add_custom_command(TARGET Nova POST_BUILD 
					COMMAND ${CMAKE_COMMAND} -E make_directory "${EXAMPLES_BINARY_DIR}/$<CONFIGURATION>")
	add_custom_command(TARGET Nova POST_BUILD
		               COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Nova>  "${EXAMPLES_BINARY_DIR}/$<CONFIGURATION>")
endif()						



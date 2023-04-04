set(APP_NAME sdlTest)

file(GLOB HEADER_FILES 	CONFIGURE_DEPENDS "*.h")
file(GLOB CPP_FILES 	CONFIGURE_DEPENDS "*.cpp")
file(GLOB BINDING_HEADER_FILES 	CONFIGURE_DEPENDS "${CMAKE_BINARY_DIR}/bindings/*.h")
file(GLOB BINDING_CPP_FILES 	CONFIGURE_DEPENDS "${CMAKE_BINARY_DIR}/bindings/*.cpp")

add_executable(${APP_NAME} 
	${HEADER_FILES}
	${CPP_FILES}
	${CMAKE_BINARY_DIR}/bindings/imgui_impl_sdl.h
	${CMAKE_BINARY_DIR}/bindings/imgui_impl_sdl.cpp
	${CMAKE_BINARY_DIR}/bindings/imgui_impl_opengl3.h
	${CMAKE_BINARY_DIR}/bindings/imgui_impl_opengl3.cpp
)
find_package(EnTT CONFIG REQUIRED)			   
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

target_link_libraries (${APP_NAME} 
	PRIVATE				
		${PROJECT_NAMESPACE}::common
		EnTT::EnTT
		spdlog::spdlog_header_only
		nlohmann_json::nlohmann_json
		SDL2::SDL2
		imgui::imgui
		glfw::glfw
)

target_include_directories(${APP_NAME}
	PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
				$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" 
	FILES ${HEADER_FILES} ${CPP_FILES}
)

set_target_properties(${APP_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
set_target_properties(${APP_NAME} PROPERTIES RELWITHDEBINFO_POSTFIX ${CMAKE_RELWITHDEBINFO_POSTFIX})
set_target_properties(${APP_NAME} PROPERTIES FOLDER "Sandbox")

install(TARGETS ${APP_NAME}
        CONFIGURATIONS Debug Release RelWithDebInfo 
        RUNTIME DESTINATION bin)


set_target_properties(
    ${APP_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/bin")

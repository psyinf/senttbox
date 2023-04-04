set(APP_NAME loaderTest)

file(GLOB HEADER_FILES 	CONFIGURE_DEPENDS "*.h")
file(GLOB CPP_FILES 	CONFIGURE_DEPENDS "*.cpp")

add_executable(${APP_NAME} 
	${HEADER_FILES}
	${CPP_FILES}
)
find_package(EnTT CONFIG REQUIRED)			   
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(cereal CONFIG REQUIRED)

target_link_libraries (${APP_NAME} 
	PRIVATE				
		${PROJECT_NAMESPACE}::common
		EnTT::EnTT
		spdlog::spdlog_header_only
		nlohmann_json::nlohmann_json
		cereal::cereal
)

target_include_directories(${APP_NAME}
	PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
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

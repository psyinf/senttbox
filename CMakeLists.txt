cmake_minimum_required(VERSION 3.21)
set(PROJECT SenTTBox)
project(${PROJECT} VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_DEBUG_POSTFIX _d)
set(CMAKE_RELWITHDEBINFO_POSTFIX _rd)

set(PROJECT_NAMESPACE ${PROJECT})			# namespace for target aliases in this project
set(PROJECT_EXPORT_NAME ${PROJECT})			# export name to be used by install() and downstream find_package()
set(PROJECT_INSTALL_PREFIX ${PROJECT})		# folder 'prefix' for installed includes and libs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Run conan install automatically, or display warning if conan is not installed
include(cmake/conan_config.cmake)
include(cmake/helpers.cmake)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/../install  CACHE PATH <comment> FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)


if(WIN32)
    add_compile_definitions(WINDOWS=1)
	add_definitions(-DNOMINMAX)
endif ()


add_subdirectory(src/common)
add_subdirectory(src/sandbox)
#add_subdirectory(src/core)
#add_subdirectory(src/taskEngine)
#EXAMPLES
#add_subdirectory(src/plugins)
#EXAMPLES
#add_subdirectory(examples)
#TESTS
#enable_testing()
#add_subdirectory(src/tests/common)
#add_subdirectory(src/tests/core)

if(WIN32)
 add_custom_target( conan-install ALL COMMAND conan install ${CMAKE_SOURCE_DIR}/conanfile.py -if ${CMAKE_INSTALL_PREFIX} -s build_type=$<CONFIG> DEPENDS install)       
endif()


install(DIRECTORY
		data/
	DESTINATION
		${CMAKE_INSTALL_BINDIR}/data
	COMPONENT
		${RUNTIME_COMPONENT}
)

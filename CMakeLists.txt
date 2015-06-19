cmake_minimum_required (VERSION 2.8)

set(PROJNAME "nanohtml")

file(GLOB MAIN_SOURCES_C "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB MAIN_SOURCES_CPP "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB LITEHTML_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/litehtml/src/*.cpp")
file(GLOB LITEHTML_GUMBO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/litehtml/src/gumbo/*.c")

set(SOURCES
	${MAIN_SOURCES_CPP} ${MAIN_SOURCES_C}
	"${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src/nanovg.c" 
	${LITEHTML_SOURCES} ${LITEHTML_GUMBO_SOURCES}
)

project (${PROJNAME})

file(GLOB_RECURSE RESOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/res/" "res/*.*")

IF(APPLE)
	add_executable(
		${PROJNAME}
		MACOSX_BUNDLE 
		${SOURCES}
		${RESOURCES}
	)

	foreach(item IN LISTS RESOURCES)
		message(STATUS ${item})
		get_filename_component(dir ${item} DIRECTORY)
		set_source_files_properties(${item} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/${dir})
	endforeach()

  	set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJNAME})
  	
ELSE()

	# we don't want to copy if we're building in the source dir
	if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)

		foreach(item IN LISTS RESOURCES)
			message(STATUS ${item})
			add_custom_command(
				OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${item}"
				COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/res/${item}" "${CMAKE_CURRENT_BINARY_DIR}/${item}"
				DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/res/${item}"
			)
		endforeach()
	endif()

	add_executable(
		${PROJNAME}
		${SOURCES}
		${RESOURCES}
	)

ENDIF(APPLE)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src 
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/litehtml/include
	${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src
	${CMAKE_CURRENT_SOURCE_DIR}/glfw/include
)

#SET(GLFW_BUILD_DOCS OFF CACHE FORCE)
#SET(GLFW_BUILD_EXAMPLES OFF CACHE FORCE)
#SET(GLFW_BUILD_TESTS OFF CACHE FORCE)

add_subdirectory("glfw")

IF(APPLE)
	SET(CMAKE_C_FLAGS "-std=gnu99")
	SET(CMAKE_CXX_FLAGS "-std=c++11")

    SET(EXTRA_LIBS "-framework Cocoa -framework OpenGL")
	SET(CMAKE_EXE_LINKER_FLAGS "-std=gnu99")
ELSEIF(WIN32)
	SET(CMAKE_C_FLAGS " -DWIN32 -DLITEHTML_UTF8 -std=gnu99")
	SET(CMAKE_CXX_FLAGS " -DWIN32 -DLITEHTML_UTF8 -std=c++11")

	SET(EXTRA_LIBS "-lglu32 -lopengl32")
	SET(CMAKE_EXE_LINKER_FLAGS "-std=gnu99 -static -static-libgcc -static-libstdc++ -mconsole")
ENDIF()

target_link_libraries(${PROJNAME} glfw ${GLFW_LIBRARIES} ${EXTRA_LIBS})

add_custom_target(data-target ALL DEPENDS "${RESOURCES}")
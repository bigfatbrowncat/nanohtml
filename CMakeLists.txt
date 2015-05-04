cmake_minimum_required (VERSION 2.8)

set(PROJNAME "AlphaClock")

set(ALLRESOURCES 
	"fonts/Walkway_Black.ttf"
	"fonts/Walkway_Bold.ttf"
)

set(SOURCES
	"src/clock.c" 
	"${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src/nanovg.c" 
)

project (${PROJNAME})

IF(APPLE)
	add_executable(
		${PROJNAME}
		MACOSX_BUNDLE 
		${SOURCES}
		${ALLRESOURCES}
	)
	
	set_source_files_properties(
    		${ALLRESOURCES}
	    PROPERTIES
    		MACOSX_PACKAGE_LOCATION Resources/fonts
  	)
  	set(MACOSX_BUNDLE_BUNDLE_NAME "Alpha Clock")
  	
ELSE()
	add_executable(
		${PROJNAME} 
		${SOURCES} 
		${ALLRESOURCES}
	)

	file(COPY ${ALLRESOURCES} DESTINATION "fonts/")
ENDIF(APPLE)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src 
	${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src
	${CMAKE_CURRENT_SOURCE_DIR}/glfw/include
)

add_subdirectory("glfw")

IF(APPLE)
    SET(EXTRA_LIBS "-framework Cocoa -framework OpenGL")
ELSEIF(WIN32)
	SET(EXTRA_LIBS "-static -static-libgcc -lglu32 -lopengl32")
ENDIF()

target_link_libraries(${PROJNAME} glfw ${GLFW_LIBRARIES} ${EXTRA_LIBS})


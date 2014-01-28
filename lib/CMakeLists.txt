

file(GLOB modules *) 

SET(SOURCES_ALL "")
foreach (_module ${modules})
	if (IS_DIRECTORY ${_module})
		file(GLOB_RECURSE tmp "${_module}/src/*.c")
		LIST(APPEND SOURCES_ALL ${tmp})		
	endif()
endforeach()


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)


add_library(osld ${SOURCES_ALL})
		











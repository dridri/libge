cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")
project(LibGE)

<<<<<<< HEAD
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99") # -g3
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x") # -g3
set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -std=c99 -std=c++0x") # -g3
=======
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -std=c99 -std=c++0x -g3")
>>>>>>> a856bc891ae3b2c9b4b481e102e1ff17324e6d09
add_definitions(-DLIBGE_LIB)
#add_definitions(-DOPENCL)

option(target "target" )
option(video "video" )
option(static "static" )

if(${target} MATCHES OFF)
	if(WIN32)
		set(target win)
	elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		set(target mac)
	elseif(UNIX)
		set(target linux)
	endif()
endif()

if("${target}" MATCHES "win*")
	set(ARCH 64) ## TODO
	set(CMAKE_C_COMPILER C:/MinGW/bin/x86_64-w64-mingw32-gcc.exe) ## TODO
	set(CMAKE_CXX_COMPILER C:/MinGW/bin/x86_64-w64-mingw32-g++.exe) ## TODO
elseif("${target}" MATCHES "mac*")
	execute_process(COMMAND getconf LONG_BIT OUTPUT_VARIABLE ARCH)
	set(CMAKE_C_COMPILER gcc)
	set(CMAKE_CXX_COMPILER g++)
elseif("${target}" MATCHES "linux*")
	execute_process(COMMAND getconf LONG_BIT OUTPUT_VARIABLE ARCH)
	set(platform linux)
	find_package(X11 REQUIRED)
	find_package(ALSA REQUIRED)
	include_directories(${X11_INCLUDE_DIRS})
	include_directories(${ALSA_INCLUDE_DIRS})
endif()

if(${video} MATCHES OFF)
	if(WIN32 OR UNIX)
		set(video opengl30)
		find_package(OpenGL REQUIRED)
		include_directories(${OPENGL_INCLUDE_DIRS})
	else()
		set(video ${target})
	endif()
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
if(NOT WIN32)
	find_package(PNG REQUIRED)
	find_package(JPEG REQUIRED)
	find_package(Freetype REQUIRED)
	find_package(Lua52 REQUIRED)
	find_package(ZLIB REQUIRED)
	include_directories(${PNG_INCLUDE_DIRS})
	include_directories(${JPEG_INCLUDE_DIRS})
	include_directories(${FREETYPE_INCLUDE_DIRS})
	include_directories(${LUA_INCLUDE_DIRS})
	include_directories(${ZLIB_INCLUDE_DIRS})
endif()

add_definitions(-DPLATFORM_${target})

set(SOURCES
	src/video/geimg.c
	src/video/gelensflare.c
	src/video/gelightscattering.c
	src/video/gewater.c
	src/audio/geaudio_idtag.c
	src/audio/geaudio_music.c
	src/audio/geaudio_sound.c
	src/audio/geaudio_wav.c
	src/audio/geaudio_fx.c
	src/geloader_obj.c
	src/geloader_obj_obj.c
	src/geloader_obj_dae.c
	src/geloader_obj_dae_anim.c
	src/geloader_obj_geo.c
	src/geloader_scene.c
	src/geloader_scene_ge3d.c
	src/geloader_img.c
	src/geloader_img_tga.c
	src/geloader_img_bmp.c
	src/geloader_img_png.c
	src/geloader_img_jpeg.c
	src/geloader_img_xpm.c
	src/gesaver_obj.c
	src/gesaver_obj_geo.c
	src/lua/gelua.c
	src/lua/gelua_ge.c
	src/lua/gelua_image.c
	src/lua/gelua_screen.c
	src/gui/gegui.c
	src/gui/gegui_button.c
	src/gui/gegui_checkbox.c
	src/gui/gegui_inputbox.c
	src/gui/gegui_progressbar.c
	src/gui/gegui_combobox.c
	src/gui/gegui_listwidget.c
	src/gui/gegui_window.c
	src/geanimation.c
	src/gecamera.c
	src/gephysics.c
	src/gekeyboard.c
	src/gefont.c
	src/gefont_ttf.c
	src/ge.c
	src/getimers.c
	src/geframerate.c
	src/gefile.c
	src/gesocket.c
	src/geutils.c
	src/gesys.c
#	src/gecl.c
	)

if("${ARCH}" MATCHES "64")
	set(ARCH_MSG "64 bit architecture")
	set(SOURCES ${SOURCES} src/ge_internal_64.c)
	set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -m64")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
	set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -m64")
else()
	set(ARCH_MSG "32 bit architecture")
	set(SOURCES ${SOURCES} src/ge_internal_32.c)
	set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -m32")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
	set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -m32")
endif()

if(WIN32)
	set(SOURCES ${SOURCES} src/system/win/gewinkeyboard.c)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(SOURCES ${SOURCES} src/system/mac/mac.m)
elseif(UNIX)
	set(SOURCES ${SOURCES} src/system/linux/gelinuxkeyboard.c)
endif()

set(SOURCES
	${SOURCES}
	src/system/${target}/gesystem.c
	src/system/${target}/ge_window.c
	src/system/${target}/gememory.c
	src/system/${target}/gekeys.c
	src/system/${target}/gethreads.c
	src/system/${target}/genet.c
	)

set(SOURCES
	${SOURCES}
	src/audio/${target}/geaudio_mp3.c
	src/audio/${target}/geaudio_sys.c
	)

set(SOURCES
	${SOURCES}
	src/video/${video}/geparticles.c
	src/video/${video}/ge_context.c
	src/video/${video}/ge_viddrv.c
	src/video/${video}/geshaders.c
	src/video/${video}/gematrix.c
	src/video/${video}/gedraw2d.c
	src/video/${video}/gedraw3d.c
	src/video/${video}/geclouds.c
	src/video/${video}/geloader_3d.c
	src/video/${video}/gemaths.c
	)

set(INCLUDES
	include/libge.h
	include/getypes.h
	include/geresources.h
	include/gefile.h
	include/gesocket.h
	include/geenums.h
	include/gephysics.h
	include/gekeys.h
	include/gegui.h
	include/getimers.h
	include/gelua.h
	include/geaudio.h
	include/gefont.h
	include/geutils.h
	include/gecl.h
	include/geerror.h
	include/gesys.h
	include/gegfx.h
	)

if("${video}" MATCHES "opengl*")
	set(VID_MSG " supporting ${video}+ hardware")
endif()
message(STATUS "LibGE will be compiled for '${target}' in ${ARCH_MSG}${VID_MSG}")

AUX_SOURCE_DIRECTORY(testsrc TEST_SOURCES)

if(static)
	add_library(ge_static STATIC ${SOURCES})
else()
	add_library(ge SHARED ${SOURCES})
endif()

add_executable(test ${TEST_SOURCES} ${SOURCES})

if(WIN32)
	set(LIBS
		m
		mad
		png
		openGL32
		lua
		jpeg
		freetype
		z
		bz2
		wsock32
		gdi32
		user32
		winmm)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -framework Cocoa -framework OpenGL")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -framework Cocoa -framework OpenGL")
	set(LIBS
		objc
		m
		mad
		${OPENGL_LIBRARIES}
		${LUA_LIBRARY}
		${JPEG_LIBRARY}
		${PNG_LIBRARY}
		${ZLIB_LIBRARY}
		${FREETYPE_LIBRARY}
		${ALSA_LIBRARY}
		pthread
		)
elseif(UNIX)
	set(LIBS
		m
		mad
#		OpenCL
		${OPENGL_LIBRARIES}
		${LUA_LIBRARY}
		${JPEG_LIBRARY}
		${PNG_LIBRARY}
		${ZLIB_LIBRARY}
		${FREETYPE_LIBRARY}
		${ALSA_LIBRARY}
		pthread
		)
endif()

message(STATUS "LIBS : ${LIBS}")

if(static)
	target_link_libraries(ge_static ${LIBS})
	INSTALL(TARGETS ge_static ARCHIVE DESTINATION /usr/lib LIBRARY DESTINATION /usr/lib)
else()
	target_link_libraries(ge ${LIBS})
	INSTALL(TARGETS ge ARCHIVE DESTINATION /usr/lib LIBRARY DESTINATION /usr/lib)
endif()

target_link_libraries(test ${LIBS})
INSTALL(FILES ${INCLUDES} DESTINATION include/libge)

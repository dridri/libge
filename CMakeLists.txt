cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")
project(LibGE)

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c99 -g3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -g3")
set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -std=c99 -std=c++0x -g3")

option(target "target" )
option(video "video" )

if(${target} MATCHES OFF)
	if(WIN32)
		set(target win)
	elseif(UNIX)
		set(target linux)
	endif()
endif()

if("${target}" MATCHES "win*")
	set(ARCH 32) ## TODO
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

add_library(ge SHARED ${SOURCES})

target_link_libraries(ge
					  m
					  mad
					  ${OPENGL_LIBRARIES}
					  ${LUA_LIBRARIES}
					  ${JPEG_LIBRARIES}
					  ${PNG_LIBRARIES}
					  ${ZLIB_LIBRARIES}
					  ${FREETYPE_LIBRARIES}
					  ${ALSA_LIBRARIES}
					  )

install(TARGETS ge LIBRARY DESTINATION /usr/lib)
INSTALL(FILES ${INCLUDES} DESTINATION include/libge)
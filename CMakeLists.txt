cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")
project(LibGE)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -g3")
set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -std=c++0x -g3")

add_definitions(-DLIBGE_LIB)

#add_definitions(-DOPENCL)
#set(SOURCES ${SOURCES} src/gecl.c)
#set(LIBS ${LIBS} -lOpenCL)

option(target "target")
option(video "video")
option(static "static")
option(mini "mini")
option(opttest "opttest")
option(debug "debug" )
option(opencl "opencl")
option(ARCH "ARCH")

if(mini)
	add_definitions(-DLIBGE_NOLUA)
	add_definitions(-DLIBGE_MINI)
endif()

if(${debug} MATCHES "1")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")
	set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -g3")
endif()

if(${target} MATCHES OFF)
	if(WIN32)
		set(target win)
	elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		set(target mac)
	elseif(UNIX)
		set(target linux)
	endif()
endif()

if("${target}" MATCHES "psp")
	message(STATUS "PSP")
	execute_process(COMMAND psp-config -d OUTPUT_VARIABLE psp_path)
	set(CMAKE_SYSTEM_NAME Generic) 
	set(ARCH 32)
	set(CMAKE_C_COMPILER "${psp_path}/bin/psp-gcc.exe")
	set(CMAKE_CXX_COMPILER "${psp_path}/bin/psp-g++.exe")
elseif("${target}" MATCHES "win*")
	if(${ARCH} MATCHES OFF)
		message(WARNING "Architecture defaulted to Windows 64bits")
		set(ARCH 64) ## TODO
		set(CMAKE_C_COMPILER C:/MinGW/mingw64/bin/x86_64-w64-mingw32-gcc.exe) ## TODO
		set(CMAKE_CXX_COMPILER C:/MinGW/mingw64/bin/x86_64-w64-mingw32-g++.exe) ## TODO
	endif()
elseif("${target}" MATCHES "mac*")
	if(${ARCH} MATCHES OFF)
		execute_process(COMMAND getconf LONG_BIT OUTPUT_VARIABLE ARCH)
	endif()
#	set(CMAKE_C_COMPILER gcc)
#	set(CMAKE_CXX_COMPILER g++)
	include_directories(/usr/include/freetype2)
	include_directories(/usr/include/freetype2/freetype)
elseif("${target}" MATCHES "linux*")
	if(${ARCH} MATCHES OFF)
		execute_process(COMMAND getconf LONG_BIT OUTPUT_VARIABLE ARCH)
	endif()
	set(platform linux)
	find_package(X11 REQUIRED)
	include_directories(${X11_INCLUDE_DIRS})
	if(!mini)
		find_package(ALSA REQUIRED)
		include_directories(${ALSA_INCLUDE_DIRS})
	endif()
elseif("${target}" MATCHES "qt")
	execute_process(COMMAND getconf LONG_BIT OUTPUT_VARIABLE ARCH)
#	find_package(Qt REQUIRED)
# 	find_package(Qt5Widgets REQUIRED)
# 	find_package(Qt5OpenGL REQUIRED)
# 	include_directories(${QT_INCLUDES} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5OpenGL_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
endif()

if(${video} MATCHES OFF)
	if("${target}" MATCHES "qt")
		set(video opengl30)
	elseif(WIN32 OR UNIX)
		set(video opengl30)
	else()
		set(video ${target})
	endif()
endif()

# if("${target}" MATCHES "qt" OR WIN32 OR UNIX)
#if("${target}" MATCHES "qt" OR UNIX)
if("${target}" MATCHES "qt" OR "${video}" MATCHES "opengl*")
	find_package(OpenGL REQUIRED)
	message(WARNING "Opengl include : ${OPENGL_INCLUDE_DIR}")
	include_directories(${OPENGL_INCLUDE_DIR})
endif()

if("${video}" MATCHES "opengl20")
	add_definitions(-DLIBGE_OPENGL20)
else()
	add_definitions(-DLIBGE_OPENGL30)
endif()

if(NOT "${opencl}" MATCHES OFF)
	find_package(OpenCL REQUIRED)
	include_directories(${OPENCL_INCLUDE_DIRS})
	add_definitions(-DOPENCL)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
if(NOT WIN32 AND NOT mini)
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

if(${video} MATCHES opengl21 AND ${target} MATCHES win)
	add_definitions(-DPLATFORM_${target}_low)
else()
	add_definitions(-DPLATFORM_${target})
endif()

set(SOURCES ${SOURCES}
	src/video/geshader.c
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
	src/lua/gelua_file.c
	src/lua/gelua_font.c
	src/lua/gelua_image.c
	src/lua/gelua_screen.c
	src/lua/gelua_shader.c
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
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
	set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -m64")
	set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -m64")
else()
	set(ARCH_MSG "32 bit architecture")
	set(SOURCES ${SOURCES} src/ge_internal_32.c)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
	set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -m32")
	set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -m32")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")
endif()

if("${target}" MATCHES "qt")
	set(SOURCES ${SOURCES} src/system/qt/qt.cpp)
elseif(WIN32)
	set(SOURCES ${SOURCES} src/system/win/gewinkeyboard.c)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(SOURCES ${SOURCES} src/system/mac/mac.m)
	set_source_files_properties(src/system/mac/mac.m PROPERTIES C_COMPILER clang)
elseif(UNIX)
	set(SOURCES ${SOURCES} src/system/linux/gelinuxkeyboard.c)
endif()

if(NOT "${opencl}" MATCHES OFF)
	set(SOURCES ${SOURCES} src/gecl.c)
endif()

if(${video} MATCHES opengl21 AND ${target} MATCHES win)
	set(SOURCES
		${SOURCES}
		src/system/win_low/gesystem.c
		src/system/win_low/ge_window.c
		src/system/win_low/gememory.c
		src/system/win_low/gekeys.c
		src/system/win_low/gethreads.c
		src/system/win_low/genet.c
		)
else()
	set(SOURCES
		${SOURCES}
		src/system/${target}/gesystem.c
		src/system/${target}/ge_window.c
		src/system/${target}/gememory.c
		src/system/${target}/gekeys.c
		src/system/${target}/gethreads.c
		src/system/${target}/genet.c
		)
endif()

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



set(MINI_SOURCES ${MINI_SOURCES}
	src/video/geimg.c
	src/video/gelensflare.c
	src/video/gelightscattering.c
	src/video/gewater.c
	src/geloader_img.c
	src/gesaver_obj.c
	src/gesaver_obj_geo.c
	src/geanimation.c
	src/gecamera.c
	src/gephysics.c
	src/gekeyboard.c
	src/gefont.c
	src/ge.c
	src/getimers.c
	src/geframerate.c
	src/gefile.c
	src/gesocket.c
	src/geutils.c
	src/gesys.c
	)

if("${target}" MATCHES "qt")
	set(MINI_SOURCES ${MINI_SOURCES} src/system/qt/qt.cpp)
elseif(WIN32)
	set(MINI_SOURCES ${MINI_SOURCES} src/system/win/gewinkeyboard.c)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(MINI_SOURCES ${MINI_SOURCES} src/system/mac/mac.m)
elseif(UNIX)
	set(MINI_SOURCES ${MINI_SOURCES} src/system/linux/gelinuxkeyboard.c)
endif()

set(MINI_SOURCES
	${MINI_SOURCES}
	src/system/${target}/gesystem.c
	src/system/${target}/ge_window.c
	src/system/${target}/gememory.c
	src/system/${target}/gekeys.c
	src/system/${target}/gethreads.c
	src/system/${target}/genet.c
	)

set(MINI_SOURCES
	${MINI_SOURCES}
	src/video/${video}/geparticles.c Demandes
Messages
1 Notifications
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


if("${video}" MATCHES "opengl*")
	set(VID_MSG " supporting ${video}+ hardware")
endif()
message(STATUS "LibGE will be compiled for '${target}' in ${ARCH_MSG}${VID_MSG}")

AUX_SOURCE_DIRECTORY(testsrc TEST_SOURCES)

if("${ARCH}" MATCHES "64")
	set(OBJCPY_OUT elf64-x86-64)
else()
	set(OBJCPY_OUT elf32-i386)
endif()
set(OBJCPY_ARCH -B i386)

if(${video} MATCHES "opengl21")
	set(geshader_v geshader_gl2v.h)
	set(geshader_g geshader_gl2g.h)
	set(geshader_f geshader_gl2f.h)
elseif(${video} MATCHES "opengl30")
	set(geshader_v geshader_gl3v.h)
	set(geshader_g geshader_gl3g.h)
	set(geshader_f geshader_gl3f.h)
endif()

if(UNIX AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(geshader_obj_files
				${CMAKE_CURRENT_BINARY_DIR}/geshader_vert.o
				${CMAKE_CURRENT_BINARY_DIR}/geshader_geom.o
				${CMAKE_CURRENT_BINARY_DIR}/geshader_frag.o
				)

	set_source_files_properties(
				${geshader_obj_files}
				PROPERTIES
				EXTERNAL_OBJECT true
				GENERATED true
				)

	add_custom_command(
				OUTPUT ${geshader_obj_files}
				WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/default_shaders
				COMMAND objcopy -I binary -O ${OBJCPY_OUT} ${OBJCPY_ARCH} --rename-section .data=.rodata,alloc,load,data,contents,readonly ${geshader_v} ${CMAKE_CURRENT_BINARY_DIR}/geshader_vert.o
				COMMAND objcopy -I binary -O ${OBJCPY_OUT} ${OBJCPY_ARCH} --rename-section .data=.rodata,alloc,load,data,contents,readonly ${geshader_g} ${CMAKE_CURRENT_BINARY_DIR}/geshader_geom.o
				COMMAND objcopy -I binary -O ${OBJCPY_OUT} ${OBJCPY_ARCH} --rename-section .data=.rodata,alloc,load,data,contents,readonly ${geshader_f} ${CMAKE_CURRENT_BINARY_DIR}/geshader_frag.o
				)

	set(SOURCES ${SOURCES} ${geshader_obj_files})
endif()

if(static)
	add_library(ge_static STATIC ${SOURCES})
elseif(opttest)
	add_executable(test ${TEST_SOURCES} ${SOURCES})
elseif(mini)
	add_library(ge STATIC ${MINI_SOURCES})
elseif("${target}" MATCHES "qt")
	add_library(ge_qt SHARED ${SOURCES})
else()
	add_library(ge SHARED ${SOURCES})
endif()

if(WIN32)
	set(LIBS ${LIBS}
		mad
		png
		openGL32
		lua
		jpeg
		freetype
		z
		m
		wsock32
		gdi32
		user32
		winmm)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -framework Cocoa -framework OpenGL")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -framework Cocoa -framework OpenGL")
	set(LIBS ${LIBS}
		objc
		m
		${OPENGL_LIBRARIES}
		pthread
		)
	if(NOT mini)
		set(LIBS ${LIBS}
		mad
			bz2
			${LUA_LIBRARY}
			${JPEG_LIBRARY}
			${PNG_LIBRARY}
			${ZLIB_LIBRARY}
			${FREETYPE_LIBRARY}
		)
	endif()
elseif(UNIX)
	set(LIBS ${LIBS}
		m
#		OpenCL
		${OPENCL_LIBRARIES}
		${OPENGL_LIBRARIES}
		pthread
		)
	if(NOT mini)
		set(LIBS ${LIBS}
			mad
			${LUA_LIBRARY}
			${JPEG_LIBRARY}
			${PNG_LIBRARY}
			${ZLIB_LIBRARY}
			${FREETYPE_LIBRARY}
			#${ASOUND2_LIBRARY}
			asound
		)
	endif()
endif()

# if("${target}" MATCHES "qt")
# 	if(opttest)
# 		qt5_use_modules(test Widgets)
# 	elseif()
# 		qt5_use_modules(ge_qt Widgets)
# 	endif()
# 	set(LIBS ${LIBS}
# 		Qt5::Core
# 		Qt5::Widgets
# 		Qt5::OpenGL
# 		)
# endif()

message(STATUS "LIBS : ${LIBS}")

if(static)
	target_link_libraries(ge_static ${LIBS})
	INSTALL(TARGETS ge_static ARCHIVE DESTINATION /usr/lib LIBRARY DESTINATION /usr/lib)
elseif(opttest)
	target_link_libraries(test ${LIBS})
elseif("${target}" MATCHES "qt")
	target_link_libraries(ge_qt ${LIBS})
	INSTALL(TARGETS ge_qt ARCHIVE DESTINATION /usr/lib LIBRARY DESTINATION /usr/lib)
else()
	target_link_libraries(ge  ${LIBS})
	INSTALL(TARGETS ge ARCHIVE DESTINATION /usr/lib LIBRARY DESTINATION /usr/lib)
endif()

INSTALL(FILES ${INCLUDES} DESTINATION include/libge)


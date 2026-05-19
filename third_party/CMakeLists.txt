set(BUILD_SHARED_LIBS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
set(GLFW_LIBRARY_TYPE "STATIC")
add_subdirectory(glfw-3.4)

set(IMGUI_DIR ${CMAKE_CURRENT_LIST_DIR}/imgui-1.92.6)

set(IMGUI_SOURCES
	${IMGUI_DIR}/imconfig.h
	${IMGUI_DIR}/imgui.cpp
	${IMGUI_DIR}/imgui.h
	${IMGUI_DIR}/imgui_draw.cpp
	${IMGUI_DIR}/imgui_internal.h
	${IMGUI_DIR}/imgui_tables.cpp
	${IMGUI_DIR}/imgui_widgets.cpp
	${IMGUI_DIR}/imstb_rectpack.h
	${IMGUI_DIR}/imstb_textedit.h
	${IMGUI_DIR}/imstb_truetype.h
	${IMGUI_DIR}/imgui_demo.cpp)

add_library(imgui STATIC ${IMGUI_SOURCES})
add_library(imgui::imgui ALIAS imgui)

target_include_directories(imgui PUBLIC
	$<BUILD_INTERFACE:${IMGUI_DIR}>)

target_sources(imgui PRIVATE
	${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
	${IMGUI_DIR}/misc/cpp/imgui_stdlib.h)
target_include_directories(imgui PUBLIC $<BUILD_INTERFACE:${IMGUI_DIR}/misc/cpp>)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT IOS)
	include(CheckCXXCompilerFlag)
	check_cxx_compiler_flag("-Wnontrivial-memaccess" HAS_WNONTRIVIAL_MEMACCESS)

	if (HAS_WNONTRIVIAL_MEMACCESS)
		target_compile_options(imgui PUBLIC -Wno-nontrivial-memaccess)
	endif()
endif()
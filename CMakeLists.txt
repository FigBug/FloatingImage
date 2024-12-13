cmake_minimum_required (VERSION 3.16.0 FATAL_ERROR)

#
# Set for each plugin
#
file (READ "${CMAKE_CURRENT_LIST_DIR}/VERSION" VERSION)
set (BUNDLE_ID com.figbug.floatingimage)

set (CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set (CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set (CMAKE_SUPPRESS_REGENERATION true)
set (CMAKE_SKIP_INSTALL_RULES YES)
set (CMAKE_POSITION_INDEPENDENT_CODE ON)
set_property (GLOBAL PROPERTY DEBUG_CONFIGURATIONS "Debug")

set (CMAKE_C_FLAGS_DEVELOPMENT ${CMAKE_C_FLAGS_RELEASE})
set (CMAKE_CXX_FLAGS_DEVELOPMENT ${CMAKE_CXX_FLAGS_RELEASE})

project ("FloatingImage" VERSION ${VERSION} LANGUAGES CXX C HOMEPAGE_URL "https://figbug.com/")

include (CMakeDependentOption)

set_property (DIRECTORY APPEND PROPERTY LABELS "FloatingImage")

set_property (DIRECTORY APPEND PROPERTY LABELS Figbug)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "FloatingImage")

set (CMAKE_OSX_DEPLOYMENT_TARGET 10.13)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION ON)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_OBJCXX_STANDARD 20)
set (CMAKE_OBJCXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_VISIBILITY_PRESET hidden)
set (CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set (CMAKE_MINSIZEREL_POSTFIX -rm)
set (CMAKE_RELWITHDEBINFO_POSTFIX -rd)
set (CMAKE_OPTIMIZE_DEPENDENCIES OFF)

set (BUILD_SHARED_LIBS OFF)

if(APPLE)
	set (CMAKE_OSX_ARCHITECTURES arm64 x86_64)
endif()

set_property (GLOBAL PROPERTY USE_FOLDERS YES)
set_property (GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER utility)
set_property (GLOBAL PROPERTY REPORT_UNDEFINED_PROPERTIES "${CMAKE_BINARY_DIR}/undefined_properties.log")
set_property (GLOBAL PROPERTY JUCE_COPY_PLUGIN_AFTER_BUILD YES)

set_property (DIRECTORY APPEND PROPERTY LABELS External)

# JUCE

set (JUCE_MODULES_ONLY OFF)
set (JUCE_ENABLE_MODULE_SOURCE_GROUPS ON)
set (JUCE_BUILD_EXTRAS OFF)
set (JUCE_BUILD_EXAMPLES OFF)

add_subdirectory (modules/juce)

set_property (DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/modules/juce" APPEND PROPERTY LABELS JUCE)

#

# Gin modules

foreach(module_name IN ITEMS gin gin_gui)
	juce_add_module (
		"${CMAKE_CURRENT_LIST_DIR}/modules/gin/modules/${module_name}"
		)

	set_property (TARGET "${module_name}" APPEND PROPERTY LABELS Gin)
endforeach()

# Binary Data

set_property (DIRECTORY APPEND PROPERTY LABELS Assets)

file (GLOB_RECURSE binary_files CONFIGURE_DEPENDS "./Assets/*")

juce_add_binary_data ("FloatingImage_Assets" SOURCES ${binary_files})

set_target_properties("FloatingImage_Assets" PROPERTIES UNITY_BUILD ON UNITY_BUILD_MODE BATCH UNITY_BUILD_BATCH_SIZE 10)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

juce_add_gui_app ("FloatingImage"
				  PRODUCT_NAME "FloatingImage"
				  VERSION ${VERSION}
				  COMPANY_NAME FigBug
				  ICON_BIG ${CMAKE_CURRENT_SOURCE_DIR}/Assets/icon.png
				  COMPANY_WEBSITE "https://figbug.com/")

file (GLOB_RECURSE source_files CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h)

target_sources ("FloatingImage" PRIVATE ${source_files})

file (GLOB_RECURSE asset_files CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/Assets/*)

target_sources ("FloatingImage" PRIVATE ${asset_files})
source_group (TREE ${CMAKE_CURRENT_SOURCE_DIR}/Assets PREFIX Assets FILES ${asset_files})

target_link_libraries ("FloatingImage" PRIVATE
						"FloatingImage_Assets"

						gin
						gin_gui

						juce::juce_core
						juce::juce_data_structures
						juce::juce_events
						juce::juce_graphics
						juce::juce_gui_basics
						juce::juce_gui_extra

    					juce::juce_recommended_config_flags
					)

target_include_directories ("FloatingImage" PRIVATE modules/fmt/include)
target_include_directories ("FloatingImage" PRIVATE modules/ASIO/common)
target_include_directories ("FloatingImage" PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source/Definitions)

juce_generate_juce_header ("FloatingImage")

target_compile_definitions ("FloatingImage" PRIVATE
								JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=1
								JUCE_COREGRAPHICS_DRAW_ASYNC=1
								JUCE_MODAL_LOOPS_PERMITTED=0
								JUCE_WEB_BROWSER=0
								JUCE_USE_FLAC=1
								JUCE_USE_CURL=1
								JUCE_USE_MP3AUDIOFORMAT=0
								JUCE_USE_LAME_AUDIO_FORMAT=0
								JUCE_USE_WINDOWS_MEDIA_FORMAT=0
								JUCE_SILENCE_XCODE_15_LINKER_WARNING=1
								_CRT_SECURE_NO_WARNINGS
							)

if (APPLE)
    foreach(t ${FORMATS} "Assets" "All" "")
        set(tgt ${CMAKE_PROJECT_NAME})
        if (NOT t STREQUAL "")
            set(tgt ${tgt}_${t})
        endif()
        if (TARGET ${tgt})
			set_target_properties(${tgt} PROPERTIES
				XCODE_ATTRIBUTE_CLANG_LINK_OBJC_RUNTIME NO
				#XCODE_ATTRIBUTE_DEPLOYMENT_POSTPROCESSING[variant=Release] YES
				XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
				)
			if (NOT t STREQUAL "All")
				target_compile_options(${tgt} PRIVATE
					-Wall -Wstrict-aliasing -Wunused-parameter -Wconditional-uninitialized -Woverloaded-virtual -Wreorder -Wconstant-conversion -Wbool-conversion -Wextra-semi
					-Wunreachable-code -Winconsistent-missing-destructor-override -Wshift-sign-overflow -Wnullable-to-nonnull-conversion -Wuninitialized -Wno-missing-field-initializers
					-Wno-ignored-qualifiers -Wno-missing-braces -Wno-char-subscripts -Wno-unused-private-field -fno-aligned-allocation -Wunused-private-field -Wunreachable-code
					-Wenum-compare -Wshadow -Wfloat-conversion -Wshadow-uncaptured-local -Wshadow-field -Wsign-compare -Wdeprecated-this-capture -Wimplicit-float-conversion
					-ffast-math -fno-finite-math-only)
			endif()
       	endif()
    endforeach()
endif()

if (WIN32)
    foreach(t ${FORMATS} "Assets" "All" "")
        set(tgt ${CMAKE_PROJECT_NAME})
        if (NOT t STREQUAL "")
            set(tgt ${tgt}_${t})
        endif()
        if (TARGET ${tgt})
			set_property(TARGET ${tgt} APPEND_STRING PROPERTY LINK_FLAGS_DEBUG " /INCREMENTAL:NO")
			set_target_properties(${tgt} PROPERTIES LINK_FLAGS "/ignore:4099")
		endif()
	endforeach()
endif()

if(UNIX AND NOT APPLE)
	target_link_libraries ("FloatingImage" PRIVATE curl)
endif()


if(WIN32)
	set (dest "Program Files")
else()
	set (dest "Applications")
endif()

install (TARGETS "FloatingImage" DESTINATION "${dest}")

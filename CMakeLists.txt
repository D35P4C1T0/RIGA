cmake_minimum_required(VERSION 3.22)
project(Riga VERSION 0.2.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(STRIP_DSP_ONLY "Build the renderer and DSP checks without JUCE" OFF)
add_library(strip_dsp STATIC Source/Engine.cpp Source/Parameters.cpp Source/Tone.cpp Source/Dynamics.cpp)
set_target_properties(strip_dsp PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(strip_dsp PUBLIC Source)
if(MSVC)
    target_compile_options(strip_dsp PRIVATE /fp:strict)
else()
    target_compile_options(strip_dsp PRIVATE -ffp-contract=off)
endif()
add_executable(strip-render tools/render.cpp)
target_link_libraries(strip-render PRIVATE strip_dsp)
include(CTest)
if(BUILD_TESTING)
    add_executable(strip-dsp-checks tests/dsp_checks.cpp)
    target_link_libraries(strip-dsp-checks PRIVATE strip_dsp)
    add_test(NAME dsp-checks COMMAND strip-dsp-checks)
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(Python3_Interpreter_FOUND)
        add_test(NAME render-checks COMMAND "${Python3_EXECUTABLE}"
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/render_checks.py" "$<TARGET_FILE:strip-render>")
    endif()
endif()
if(STRIP_DSP_ONLY)
    return()
endif()
set(JUCE_PATH "" CACHE PATH "Local JUCE source checkout")
if(JUCE_PATH)
    add_subdirectory("${JUCE_PATH}" "${CMAKE_BINARY_DIR}/juce")
else()
    include(FetchContent)
    FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 8.0.13 GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(JUCE)
endif()
juce_add_plugin(Riga
    COMPANY_NAME "Local Audio"
    BUNDLE_ID com.localaudio.strip
    PLUGIN_MANUFACTURER_CODE LcAu
    PLUGIN_CODE StRp
    FORMATS VST3 Standalone
    PRODUCT_NAME "Riga"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)
target_sources(Riga PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp)
target_compile_definitions(Riga PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(Riga PRIVATE strip_dsp juce::juce_audio_utils juce::juce_dsp PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_warning_flags)
option(STRIP_EDITOR_CHECK "Build editor snapshot and state checks" OFF)
if(STRIP_EDITOR_CHECK)
    juce_add_console_app(strip-editor-check PRODUCT_NAME "Riga Editor Check")
    target_sources(strip-editor-check PRIVATE tools/editor_check.cpp)
    target_include_directories(strip-editor-check PRIVATE Source)
    target_compile_definitions(strip-editor-check PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
    target_link_libraries(strip-editor-check PRIVATE Riga juce::juce_audio_utils)
endif()

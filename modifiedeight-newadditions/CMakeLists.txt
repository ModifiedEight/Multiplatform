cmake_minimum_required(VERSION 3.16)
project(modifiedeight-newadditions LANGUAGES CXX)

enable_language(ASM)
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -I${CMAKE_CURRENT_SOURCE_DIR}")
if(MCPE_IOS)
    enable_language(OBJCXX)
endif()

if(ANDROID)
    set(ZLIB_LIBRARIES z)
elseif(MCPE_IOS)
    find_package(ZLIB REQUIRED)
else()
    find_package(ZLIB REQUIRED)
    find_package(OpenGL REQUIRED)
    if(NOT WIN32)
        find_package(OpenAL REQUIRED)
    endif()
    find_package(SDL REQUIRED)
    if(NOT WIN32)
        find_package(CURL REQUIRED)
    endif()
endif()

add_compile_options(
    $<$<COMPILE_LANGUAGE:CXX>:-fpermissive>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-out-of-line-declaration>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-write-strings>
    -Werror=infinite-recursion
    -Werror=return-type
    -DDIFFERENTTWEAKS
)

if(ANDROID)
    add_compile_options(-DUSEGLES -DIMTOOLAZYTOFIX16ERRS -DEVILOREOBROKEPTHREADS)
elseif(MCPE_IOS)
    add_compile_options(-DUSEGLES -DIMTOOLAZYTOFIX16ERRS -DMCPE_IOS -DSTBI_NO_THREAD_LOCALS)
else()
    add_compile_options(-DIMTOOLAZYTOFIX16ERRS -DPCTWEAKS)
endif()

if(DEBUG)
    add_compile_options(-DAUTH_SERVER="127.0.0.1:8000" -DPEOAPI_SERVER="127.0.0.1:8001")
    add_compile_options(-g -DDEBUG)
else()
    add_compile_options(-g0)
endif()

file(GLOB_RECURSE IMPLCPP CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE IMPLC   CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
file(GLOB_RECURSE IMPLASM CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.S")
file(GLOB_RECURSE HEADC   CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE HEADCPP CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

file(GLOB ROOT_CPP CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/modifiedeight-newadditions/*.cpp")
file(GLOB ROOT_C   CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/modifiedeight-newadditions/*.c")

list(APPEND IMPLCPP ${ROOT_CPP})
list(APPEND IMPLC   ${ROOT_C})

if(MCPE_IOS)
    list(FILTER IMPLCPP EXCLUDE REGEX ".*/impl/main\\.cpp$")
    list(FILTER IMPLCPP EXCLUDE REGEX ".*/impl/AppPlatform_sdl\\.cpp$")
endif()

if(ANDROID)
    if(NOT TARGET native_app_glue)
        add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
    endif()
    add_library(minecraftpe SHARED ${IMPLCPP} ${IMPLC} ${IMPLASM} ${HEADC} ${HEADCPP})
    target_include_directories(minecraftpe PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/headers"
        "${ANDROID_NDK}/sources/android/native_app_glue"
    )
    target_link_libraries(minecraftpe PRIVATE z EGL GLESv1_CM OpenSLES oaes_lib RakNetStatic native_app_glue jsoncpp_lib utf8proc log android)
    set_target_properties(minecraftpe PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/build"
    )
elseif(MCPE_IOS)
    set(IOS_PLATFORM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../platforms/ios")
    file(GLOB IOS_PLATFORM_SRC
        "${IOS_PLATFORM_DIR}/*.mm"
        "${IOS_PLATFORM_DIR}/*.m"
    )
    add_executable(modifiedeight-newadditions ${IMPLCPP} ${IMPLC} ${IMPLASM} ${HEADC} ${HEADCPP} ${IOS_PLATFORM_SRC})
    target_include_directories(modifiedeight-newadditions PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/headers"
        "${IOS_PLATFORM_DIR}"
    )
    if(ZLIB_LIBRARY)
        set(ZLIB_LINK_TARGET ${ZLIB_LIBRARY})
    else()
        set(ZLIB_LINK_TARGET ZLIB::ZLIB)
    endif()
    target_link_libraries(modifiedeight-newadditions
        oaes_lib RakNetStatic jsoncpp_lib utf8proc
        ${ZLIB_LINK_TARGET}
        objc
        "-framework Foundation"
        "-framework CoreGraphics"
        "-framework QuartzCore"
        "-framework UIKit"
        "-framework OpenGLES"
        "-framework OpenAL"
        "-framework AVFoundation"
        "-framework GameController"
    )
    set_target_properties(modifiedeight-newadditions PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/build"
    )
else()
    add_executable(modifiedeight-newadditions ${IMPLCPP} ${IMPLC} ${IMPLASM} ${HEADC} ${HEADCPP})
    target_include_directories(modifiedeight-newadditions PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/headers"
        ${SDL_INCLUDE_DIR}
        ${SDL_INCLUDE_DIR}/SDL
    )

    if(WIN32)
        target_link_options(modifiedeight-newadditions PRIVATE -static -static-libgcc -static-libstdc++)
        target_link_libraries(modifiedeight-newadditions pthread dsound dxguid opengl32 winmm ws2_32 gdi32 psapi)
    else()
        target_link_libraries(modifiedeight-newadditions ${OPENAL_LIBRARY})
    endif()

    if(WIN32)
        set(CURL_LINK_TARGET "")
    elseif(CURL_LIBRARY)
        set(CURL_LINK_TARGET ${CURL_LIBRARY})
    else()
        set(CURL_LINK_TARGET CURL::libcurl)
    endif()

    if(ZLIB_LIBRARY)
        set(ZLIB_LINK_TARGET ${ZLIB_LIBRARY})
    else()
        set(ZLIB_LINK_TARGET ZLIB::ZLIB)
    endif()

    target_link_libraries(modifiedeight-newadditions
        ${CURL_LINK_TARGET}
        ${ZLIB_LINK_TARGET}
        ${OPENGL_LIBRARIES}
        ${SDL_LIBRARIES}
        oaes_lib
        RakNetStatic
        jsoncpp_lib
        utf8proc
    )
    if(NOT WIN32 AND NOT APPLE)
        target_link_libraries(modifiedeight-newadditions dl)
    endif()

    set_target_properties(modifiedeight-newadditions PROPERTIES
        ENABLE_EXPORTS ON
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/build"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/build"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/build"
    )

    if(EXISTS "${CMAKE_SOURCE_DIR}/platforms/android/app/src/newadditions/assets")
        add_custom_command(TARGET modifiedeight-newadditions POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/platforms/android/app/src/newadditions/assets"
                "${CMAKE_CURRENT_BINARY_DIR}/build/assets"
        )
    endif()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
        add_custom_command(TARGET modifiedeight-newadditions POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/assets"
                "${CMAKE_CURRENT_BINARY_DIR}/build/assets"
        )
    endif()
endif()


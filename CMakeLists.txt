# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

include_directories(include)
add_library(openCV SHARED IMPORTED)
set_target_properties(openCV PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI}/libopencv_java3.so)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             libImageCompareLite

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/native-ImageMerge.h
             src/main/cpp/native-ImageMerge.cpp
             src/main/cpp/native-NativeBitmap.h
             src/main/cpp/native-NativeBitmap.cpp
             src/main/cpp/inc.h
             src/main/cpp/log.h
             src/main/cpp/Debugger.h
             src/main/cpp/Debugger.cpp
             src/main/cpp/Feature.h
             src/main/cpp/FeatureCompare.h
             src/main/cpp/FeatureCompare.cpp
             src/main/cpp/HashCompare.h
             src/main/cpp/HashCompare.cpp
             src/main/cpp/ImageMerge.h
             src/main/cpp/ImageMerge.cpp
             src/main/cpp/MultiThreadTask.h
             src/main/cpp/MultiThreadTask.cpp
             src/main/cpp/NativeBitmap.h
             src/main/cpp/NativeBitmap.cpp
             src/main/cpp/NativeBitmapUtil.h
             src/main/cpp/NativeBitmapUtil.cpp
             )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       libImageCompareLite

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       jnigraphics)
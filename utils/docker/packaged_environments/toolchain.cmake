#source : https://localcoder.org/cmake-custom-toolchain-cmake-ar-and-cmake-cxx-archive-create
SET(CMAKE_C_COMPILER    "clang")
SET(CMAKE_CXX_COMPILER  "clang++")
SET(CMAKE_NM            "llvm-nm" CACHE FILEPATH "" FORCE)
SET(CMAKE_RANLIB        "llvm-ranlib" CACHE FILEPATH "" FORCE)
SET(CMAKE_AR            "llvm-ar" CACHE FILEPATH "" FORCE)

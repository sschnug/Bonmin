cmake_minimum_required(VERSION 3.14)

project(sepa)

# SETUP: where to find compiled COIN projects
# -------------------------------------------
set(COIN_DIR "X:/msys64_coin_v3/usr/local/coin")
# -------------------------------------------

# pkg-config based setup
find_package(PkgConfig REQUIRED)
include(FindPkgConfig)

# pkg-config -> bonmin
set(ENV{PKG_CONFIG_PATH} "${COIN_DIR}/lib/pkgconfig")

pkg_search_module(BONMIN REQUIRED bonmin)
link_directories(${BONMIN_LIBRARY_DIRS})
include_directories(SYSTEM ${BONMIN_INCLUDE_DIRS})

pkg_search_module(BONMINAMPL REQUIRED bonminamplinterface)
link_directories(${BONMINAMPL_LIBRARY_DIRS})
include_directories(SYSTEM ${BONMINAMPL_INCLUDE_DIRS})

# C++ COMPILER
# ------------

# !!!!!!! fpermissive needed it seems !!!!!!!
set(CMAKE_CXX_FLAGS "-Wall -Wextra -fpermissive")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# BUILD TYPE
# ----------

set(CMAKE_BUILD_TYPE Release)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# BINARY
# ------

add_executable(run "${CMAKE_CURRENT_SOURCE_DIR}/Sepa.cpp"
                   "${CMAKE_CURRENT_SOURCE_DIR}/SepaSetup.cpp"
                   "${CMAKE_CURRENT_SOURCE_DIR}/BonHeuristicInnerApproximation.cpp"
                   "${CMAKE_CURRENT_SOURCE_DIR}/BonOuterDescription.cpp"
                   "${CMAKE_CURRENT_SOURCE_DIR}/SepaHeuristicInnerApproximation.cpp"
                   "${CMAKE_CURRENT_SOURCE_DIR}/SepaTMINLP2OsiLP.cpp")

# DEPS
# ----

# Coin bonmin
target_link_libraries(run ${BONMIN_LIBRARIES} -static)
target_include_directories(run PUBLIC ${BONMIN_INCLUDE_DIRS})
target_compile_options(run PUBLIC ${BONMIN_CFLAGS_OTHER})

target_link_libraries(run ${BONMINAMPL_LIBRARIES} -static)
target_include_directories(run PUBLIC ${BONMINAMPL_INCLUDE_DIRS})
target_compile_options(run PUBLIC ${BONMINAMPL_CFLAGS_OTHER})

# zlib, bzip2
find_library(z
  NAMES libz.a
  HINT C:/msys64/mingw64/lib
)

find_library(bz2
  NAMES libbz2.a
  HINT C:/msys64/mingw64/lib
)

target_link_libraries(run z -static)
target_link_libraries(run bz2 -static)

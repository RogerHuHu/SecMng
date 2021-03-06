cmake_minimum_required(VERSION 3.4.1) 
PROJECT (SECMNG)

SET(CMAKE_SYSTEM_NAME macOS) 
MESSAGE(STATUS "This is BINARY dir " ${SECMNG_BINARY_DIR}) 
MESSAGE(STATUS "This is SOURCE dir " ${SECMNG_SOURCE_DIR}) 
#SET(LIBRARY_OUTPUT_PATH ${AES_BINARY_DIR}/lib)
SET(COMPILER_PATH /opt)

OPTION(CROSS "ON for cross-compiler, OFF for normal-compiler" OFF)
IF(CROSS)
    MESSAGE(STATUS "CROSS COMPILING")
    SET(CMAKE_CROSSCOMPILING TRUE)
    #SET(CMAKE_C_COMPILER "/opt/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/bin/arm-none-linux-gnueabi-gcc")
    #SET(CMAKE_CXX_COMPILER "/opt/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/bin/arm-none-linux-gnueabi-g++")
    SET(CMAKE_C_COMPILER "/home/roger/Compiler/Raspberry/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf-gcc")
    SET(CMAKE_CXX_COMPILER "/home/roger/Compiler/Raspberry/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf-g++")
    SET(CMAKE_C_FLAGS_DEBUG "-g -Wall")
    SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -std=c++11")
    SET(CMAKE_C_FLAGS_RELEASE "-s -O2 -Wall")
    SET(CMAKE_CXX_FLAGS_RELEASE "-s -Wall -std=c++11")
    SET(CMAKE_LD "/home/roger/Compiler/Raspberry/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf-ld")
    #SET(CMAKE_FIND_ROOT_PATH "/opt/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/")
    SET(CMAKE_FIND_ROOT_PATH "/home/roger/Compiler/Raspberry/gcc-linaro-arm-linux-gnueabihf-raspbian/")
    SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
ELSE(CROSS)
    MESSAGE(STATUS "NORMAL COMPILING")
    SET(CMAKE_C_FLAGS_DEBUG "-g -Wall")
    SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -std=c++11")
    SET(CMAKE_C_FLAGS_RELEASE "-O2 -Wall")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -std=c++11")
ENDIF(CROSS)

OPTION(DEBUG "ON for debug or OFF for release" OFF)
IF(DEBUG)
    MESSAGE(STATUS "Debug mode")
    SET(CMAKE_BUILD_TYPE Debug)
ELSE(DEBUG)
    MESSAGE(STATUS "Release mode")
    SET(CMAKE_BUILD_TYPE Release)
ENDIF(DEBUG)

INCLUDE_DIRECTORIES(${SECMNG_SOURCE_DIR}/include
                    ${SECMNG_SOURCE_DIR}/include/mongoose
                    ${SECMNG_SOURCE_DIR}/include/frozen
                    /home/roger/Software/sqlite_rpi/include)

LINK_DIRECTORIES(/home/roger/Software/sqlite_rpi/lib/)

SET(SRC_LIST
    include/mongoose/mongoose.h
    include/frozen/frozen.h
    src/mongoose/mongoose.c
    src/frozen/frozen.c
    include/AES.hpp
    src/AES.cpp
    include/AESResource.hpp
    src/AESResource.cpp
    include/AesEncryptor.hpp
    src/AesEncryptor
    /home/roger/Software/sqlite_rpi/include/sqlite3.h
#    src/big_upload.c)
#    src/server.c)
    include/SecMng.hpp
    include/Login.hpp
    include/PatternMatch.hpp
    include/Database.hpp
    include/AccountMng.hpp
    src/SecMng.cpp
    src/Login.cpp
    src/PatternMatch.cpp
    src/Database.cpp
    src/AccountMng.cpp
    src/main.cpp)

ADD_DEFINITIONS("-DMG_ENABLE_HTTP_STREAMING_MULTIPART")

ADD_EXECUTABLE(secmng ${SRC_LIST})

TARGET_LINK_LIBRARIES(secmng sqlite3)
#TARGET_LINK_LIBRARIES(supervisor sqlite3 pthread crc socklib sock_client serial_port database
#file directory log)

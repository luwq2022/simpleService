cmake_minimum_required(VERSION 3.10)

project(SimpleService)

SET(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


set(SRC_LIST 
SimpleService.cpp 
ServiceBase.cpp 
LinkUser.cpp 
TCPSession.cpp
NetService.cpp
deps/common/HYBuffer.cpp
deps/common/Logger.cpp
deps/common/HYTimer.cpp
deps/common/HYEvent.cpp
deps/common/ThreadPool.cpp
deps/common/HYFunc.cpp
)

include_directories("deps/asio/include")
include_directories("deps/common")

#add_compile_definitions(_GLIBCXX_HAS_GTHREADS)

#add_executable(SimpleService ${SRC_LIST})
add_library(ServiceBase STATIC ${SRC_LIST})


if (WIN32)

set(link_LIBRARIES
WSock32
WS2_32
)
#target_link_libraries(SimpleService ${link_LIBRARIES})

endif()


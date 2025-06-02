# CMakeList.txt: CMNexus 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

# 将源代码添加到此项目的可执行文件。
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable (CMNexus "CMNexus.cpp" "include/CMNexus.h" "include/CMAbstractTcpServer.h" "src/CMAbstractTcpServer.cpp" "include/CMIOCPTcpServer.h" "src/CMIOCPTcpServer.cpp" "include/CMAbstractThreadPool.h"  "src/CMIOCPThreadPool.cpp")

target_link_libraries(CMNexus PRIVATE ws2_32 mswsock)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CMNexus PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。

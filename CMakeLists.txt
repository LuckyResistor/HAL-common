# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.14)

# Set the project name
project(HAL-common)

# Make sure we use the C++17 compiler standard
set(CMAKE_CXX_STANDARD 17)

# Create a static library.
add_library(HAL-common BCD.hpp DateTime.cpp Fixed16.cpp String.cpp DateTime.hpp Direction.hpp Duration.hpp
        EnumStringMap.hpp event/Entry.hpp event/Loop.hpp Fixed16.hpp Flags.hpp GPIO.hpp
        IntegerMath.hpp InterruptLock.hpp RingBuffer.hpp SerialLine.hpp StatusTools.hpp String.hpp Timer.hpp
        Timestamp.hpp WireMaster.hpp WireMasterChip.hpp WireMasterChipRegister.hpp SerialLineBuffer.cpp
        SerialLineBuffer.hpp Watchdog.hpp event/Entry.hpp SerialLineStringWriter.hpp SerialLineStringWriter.cpp
        StringWriter.hpp event/Data.hpp event/Loop.cpp BitTools.hpp SerialLineShell.cpp SerialLineShell.hpp
        StringTokenizer.cpp StringTokenizer.hpp FreeMemory.hpp)


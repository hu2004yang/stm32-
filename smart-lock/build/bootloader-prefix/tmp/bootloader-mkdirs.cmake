# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/BaiduNetdiskDownload/STM32/ESP32/Espressif/frameworks/esp-idf-v5.3/components/bootloader/subproject"
  "D:/BaiduNetdiskDownload/STM32/STM32project/smart-lock/build/bootloader"
  "D:/BaiduNetdiskDownload/STM32/STM32project/smart-lock/build/bootloader-prefix"
  "D:/BaiduNetdiskDownload/STM32/STM32project/smart-lock/build/bootloader-prefix/tmp"
  "D:/BaiduNetdiskDownload/STM32/STM32project/smart-lock/build/bootloader-prefix/src/bootloader-stamp"
  "D:/BaiduNetdiskDownload/STM32/STM32project/smart-lock/build/bootloader-prefix/src"
  "D:/BaiduNetdiskDownload/STM32/STM32project/smart-lock/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/BaiduNetdiskDownload/STM32/STM32project/smart-lock/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/BaiduNetdiskDownload/STM32/STM32project/smart-lock/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()

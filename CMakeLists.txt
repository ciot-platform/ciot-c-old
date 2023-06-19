# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

if(DEFINED ENV{IDF_PATH})

  include($ENV{IDF_PATH}/tools/cmake/project.cmake)
  project(ciot)

else()

  project(ciot)
  include_directories(main/include)
  include_directories(lib/cJSON)
  add_executable(${PROJECT_NAME}
    lib/cJSON/cJSON.c
    main/common/ciot_app.c
    main/common/ciot_msg_dto.c 
    main/common/ciot_ntp_dto.c
    main/common/ciot_settings.c
    main/common/ciot_system_dto.c
    main/common/ciot_tcp_dto.c 
    main/common/ciot_wifi_dto.c 
    main/host/ciot_http_server.c
    main/host/ciot_ntp.c
    main/host/ciot_storage.c
    main/host/ciot_tcp.c
    main/host/ciot_wifi.c
    main/main.c
  )
endif()

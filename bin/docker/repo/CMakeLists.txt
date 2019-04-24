cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 14)

project(media)

set (CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Ice 3.7.1 REQUIRED Ice )


if (Ice_FOUND)
message(${Ice_LIBRARIES})
endif()

add_definitions(-DICE_CPP11_MAPPING)

set(_PROTOBUF_PROTOC slice2cpp)
  # Proto file
  get_filename_component(hw_proto "${CMAKE_SOURCE_DIR}/ices/stream.ice" ABSOLUTE)
  message(STATUS "proto path: ${hw_proto}")
  get_filename_component(hw_proto_path "${hw_proto}" PATH)
  message(STATUS "hw_proto_path path: ${hw_proto_path}")
  
  # Generated sources
  set(ice_srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/stream.cpp")
  set(ice_hdrs "${CMAKE_CURRENT_SOURCE_DIR}/src/stream.h")

  add_custom_command(
        OUTPUT "${ice_srcs}" "${ice_hdrs}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --output-dir "${CMAKE_CURRENT_SOURCE_DIR}/src"
          "${hw_proto}"
        DEPENDS "${hw_proto}")
  
  add_custom_target(buildice ALL 
  DEPENDS "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}")

  set_source_files_properties("${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}" PROPERTIES GENERATED 1)

  include_directories(src src/hksdk/include src/pub src/hkdevice)
  #link_directories(/opt/CH_HCNetSDK_V5.2.7.4_build20170606_Linux64/lib /opt/CH_HCNetSDK_V5.2.7.4_build20170606_Linux64/lib/HCNetSDKCom)
  link_directories(/opt/CH_HCNetSDK_V5.3.5.2_build20171124_Linux64/lib /opt/CH_HCNetSDK_V5.3.5.2_build20171124_Linux64/lib/HCNetSDKCom)

  add_definitions(-DHKDEVICE_EXPORTS -DGLOG_NO_ABBREVIATED_SEVERITIES -DGOOGLE_GLOG_DLL_DECL= -D__x86_64__ -DSTANDALONE)


  #aux_source_directory(./src SRCS)
set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/buffer.cpp
    ${CMAKE_SOURCE_DIR}/src/mediastream.cpp
    ${CMAKE_SOURCE_DIR}/src/rtpsender.cpp
    ${CMAKE_SOURCE_DIR}/src/stream.cpp
    ${CMAKE_SOURCE_DIR}/src/streamI.cpp
    ${CMAKE_SOURCE_DIR}/src/streammanager.cpp
    ${CMAKE_SOURCE_DIR}/src/media.cpp
    ${CMAKE_SOURCE_DIR}/src/hkdevice/hikcontroller.cpp
    ${CMAKE_SOURCE_DIR}/src/hkdevice/sdkuser.cpp
    ${CMAKE_SOURCE_DIR}/src/zmd5.c)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
link_libraries(Glacier2++11 IceGrid++11 Ice++11 pthread hcnetsdk dl glog libjrtp.a libjthread.a)
add_executable(${PROJECT_NAME} ${SOURCES} )

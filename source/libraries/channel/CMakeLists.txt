set(LIBNAME ArtemisrpiChannels)

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)


##################################################################
# Add Library - Install List
##################################################################
add_library(ArtemisrpiChannels STATIC ${SOURCES} ${INCLUDES})
target_link_libraries(ArtemisrpiChannels CosmosDeviceI2C CosmosDeviceSerial CosmosAgent CosmosCommand CosmosPacket CosmosTransfer CosmosModule)

install(TARGETS ArtemisrpiChannels DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/${PROJECT_NAME}/channel)

if(${DEPLOY_SOURCE_FILES_TO_REMOTE} MATCHES "yes")
    add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
endif()


message("Build ${LIBNAME} from ${CMAKE_CURRENT_SOURCE_DIR}")

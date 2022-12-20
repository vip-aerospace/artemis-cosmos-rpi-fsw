cmake_minimum_required(VERSION 3.1)

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(file ${files})
    get_filename_component(bfile ${file} NAME_WE)
    add_executable(${bfile} ${bfile}.cpp)
    install(TARGETS ${bfile} DESTINATION bin)

    if(${DEPLOY_FILES_TO_REMOTE} MATCHES "yes")
        add_deployment_binary(${bfile} "bin")
        add_deployment_source(${bfile}.cpp "src")
    endif()

endforeach()

target_link_libraries(agent_rpi ArtemisrpiChannels CosmosString CosmosAgent CosmosPacketHandler CosmosTask)
#target_link_libraries(agent_teensy KSAT Teensy CosmosDeviceI2C CosmosDeviceSerial CosmosPhysics CosmosAgent CosmosPacket)

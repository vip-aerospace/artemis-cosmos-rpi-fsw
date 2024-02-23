/**
 * @file rpi_channels.h
 * @brief Declarations of Raspberry Pi COSMOS channels.
 * 
 * Declares the channels running on the Raspberry Pi, and their methods and 
 * members.
 */
#ifndef RPI_CHANNELS_H
#define RPI_CHANNELS_H

#include "support/configCosmos.h"
#include "agent/agentclass.h"
#include "agent/command_queue.h"
#include "device/serial/serialclass.h"
#include "support/transferclass.h"
#include "support/packethandler.h"
#include "module/file_module.h"
#include "device/i2c/i2c.h"

/** @brief The Artemis spacecraft. */
namespace Artemis
{
    /** @brief The Raspberry Pi COSMOS agent. */
    namespace RaspberryPi
    {
        /** @brief The channels on the Raspberry Pi COSMOS agent. */
        namespace Channels
        {
            /** @brief The ID number of the Raspberry Pi node. */
            extern NODE_ID_TYPE rpi_node_id;
            /** @brief The ID number of the Teensy node. */
            extern NODE_ID_TYPE teensy_node_id;
            /** @brief The ID number of the ground station node. */
            extern NODE_ID_TYPE ground_node_id;

            /** @brief The thread for the Teensy channel. */
            extern thread teensy_thread;
            /** @brief The thread for the payload channel. */
            extern thread payload_thread;
            /** @brief The thread for file access. */
            extern thread file_thread;

            /** @todo This appears to be redundant. */
            // extern Cosmos::Module::FileModule *file_module;

            /** @todo This appears to be redundant. */
            // extern TeensyChannel *teensy_channel;
            /** @todo This appears to be redundant. */
            // extern PayloadChannel *payload_channel;

            /** @todo This appears to not be used. */
            static CRC16 calc_crc;

            int32_t init_rpi_channels(Agent *agent, bool start_file, bool start_teensy, bool start_payload);
            
            /** @brief The Teensy COSMOS channel class. */
            class TeensyChannel
            {
            public:
                int32_t channelNumber;
                uint16_t channelDataSize = 0;
                float channelDataSpeed = 0.;
                /** @todo This appears to not be used. */
                bool running;
                Agent *channelAgent;
                /** @todo This appears to not be used. */
                unordered_map<string, uint16_t> tsen_didx;

                TeensyChannel();
                int32_t Init(Agent *agent);
                void Loop();
                /** @todo This appears to not be used. */
                uint32_t Test(uint32_t id, uint8_t dest, uint8_t start, uint8_t step, uint8_t stop, uint32_t total);
                
            private:
                /** @brief The I2C connection to the Teensy. */
                I2C *i2c;
                /** @brief The UART serial connection to the Teensy. */
                Serial *serial;  
                /** @brief A packet being received from the Teensy. */
                PacketComm incomingPacket;
                /** @brief A packet to be sent to the Teensy. */
                PacketComm outgoingPacket;

                void receiveFromTeensySerial();
                void sendToTeensySerial();
                void receiveFromTeensyI2C();
                void sendToTeensyI2C();
            };

            /** @brief The payload COSMOS channel class. */
            class PayloadChannel
            {
            public:
                int32_t channelNumber;
                uint16_t channelDataSize = 0;
                float channelDataSpeed = 0.;
                /** @todo This appears to not be used. */
                bool running;
                Agent *channelAgent;

                PayloadChannel();
                int32_t Init(Agent *agent);
                void Loop();
            };
        }
    }
}

#endif // RPI_CHANNELS_H

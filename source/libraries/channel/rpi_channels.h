/**
 * @file rpi_channels.h
 * @brief Definitions of Raspberry Pi COSMOS channels.
 * 
 * Defines the channels running on the Raspberry Pi.
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

namespace Artemis
{
    namespace RaspberryPi
    {
        /** @brief The Raspberry Pi channels. */
        namespace Channels
        {
            /** @brief The ID number of the Raspberry Pi node. */
            extern NODE_ID_TYPE rpi_node_id;
            /** @brief The ID number of the Teensy node. */
            extern NODE_ID_TYPE teensy_node_id;
            /** @brief The ID number of the ground station node. */
            extern NODE_ID_TYPE ground_node_id;
            /** @todo This appears to not be used. */
            static CRC16 calc_crc;

            int32_t init_rpi_channels(Agent *agent, bool start_file, bool start_teensy, bool start_payload);
            
            /** @brief The Teensy COSMOS channel class. */
            class TeensyChannel
            {
            public:
                int32_t teensyChannelNumber;
                uint16_t teensyChannelDataSize = 0;
                float teensyChannelDataSpeed = 0.;
                /** @todo This appears to not be used. */
                bool running;
                Agent *teensyAgent;
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

                int32_t i2c_recv(PacketComm &packet);
            };

            /** @brief The payload COSMOS channel class. */
            class PayloadChannel
            {
            public:
                int32_t payloadChannelNumber;
                uint16_t payloadChannelDataSize = 0;
                float payloadChannelDataSpeed = 0.;
                /** @todo This appears to not be used. */
                bool running;
                Agent *payloadAgent;

                PayloadChannel();
                int32_t Init(Agent *agent);
                void Loop();
            };

            // Teensy stuff
            extern thread teensy_thread;
            /** @todo This appears to be redundant. */
            // extern TeensyChannel *teensy_channel;

            // File stuff
            extern thread file_thread;
            extern Cosmos::Module::FileModule *file_module;

        }
    }
}

#endif // RPI_CHANNELS_H

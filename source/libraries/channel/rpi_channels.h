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
    namespace rpi
    {
        namespace Channel
        {
            extern NodeData::NODE_ID_TYPE rpi_node_id;
            extern NodeData::NODE_ID_TYPE teensy_node_id;
            extern NodeData::NODE_ID_TYPE ground_node_id;
            static CRC16 calc_crc;

            int32_t init_rpi_channels(Agent *agent, bool start_file, bool start_teensy, bool start_payload);
            class TeensyChannel
            {
            public:
                TeensyChannel();
                int32_t Init(Agent *agent);
                void Loop();
                uint32_t Test(uint32_t id, uint8_t dest, uint8_t start, uint8_t step, uint8_t stop, uint32_t total);
                int32_t mychannel;
                uint16_t mydatasize = 0;
                float mydataspeed = 0.;
                bool running;
                Agent *agent;
                unordered_map<string, uint16_t> tsen_didx;

            private:
                I2C *i2c;
                Serial *serial;

                int32_t i2c_recv(PacketComm &packet);
            };

            class PayloadChannel
            {
            public:
                PayloadChannel();
                int32_t Init(Agent *agent);
                void Loop();
                int32_t mychannel;
                uint16_t mydatasize = 0;
                float mydataspeed = 0.;
                bool running;
                Agent *agent;
            };

            // Teensy stuff
            extern thread teensy_thread;
            extern TeensyChannel *teensy_channel;

            // File stuff
            extern thread file_thread;
            extern Cosmos::Module::FileModule *file_module;

        }
    }
}

#endif // RPI_CHANNELS_H

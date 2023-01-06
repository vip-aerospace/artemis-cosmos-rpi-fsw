#include "rpi_channels.h"

namespace Artemis
{
    namespace rpi
    {
        namespace Channel
        {
            TeensyChannel::TeensyChannel()
            {
            }

            int32_t TeensyChannel::Init(Agent *agent)
            {
                this->agent = agent;
                i2c = new I2C("/dev/i2c-1", 0x08);
                i2c->connect();
                return 0;
            }

            // Communicates with Teensy via PacketComm packets
            void TeensyChannel::Loop()
            {
                int32_t iretn = 0;

                PacketComm packet;
                mychannel = agent->channel_number("TOTEENSY");
                mydatasize = 0;
                if (mychannel >= 0)
                {
                    mydatasize = agent->channel_datasize(mychannel);
                    mydataspeed = agent->channel_speed(mychannel);
                }

                agent->debug_error.Printf("Starting Teensy Loop\n");

                //                ElapsedTime et;
                while (agent->running())
                {
                    struct sysinfo meminfoin;
                    sysinfo(&meminfoin);

                    if (i2c_recv(packet) >= 0)
                    {
                        iretn = agent->channel_push("EXEC", packet);
                    }

                    // Comm - Internal
                    if ((iretn = agent->channel_pull(mychannel, packet)) > 0)
                    {
                    }

                    std::this_thread::yield();
                }

                return;
            }

            int32_t TeensyChannel::i2c_recv(PacketComm &packet)
            {
                int32_t iretn = 0;
                std::string msg;

                // I2C From Teensy
                iretn = i2c->receive(msg, 50);

                if (iretn <= 0 || msg.length() < sizeof(PacketComm::header) + sizeof(PacketComm::crc))
                    return -1;

                packet.wrapped.resize(0);
                uint8_t data_len = msg[0];
                for (size_t i = 0; i < sizeof(PacketComm::header) + data_len + sizeof(PacketComm::crc); i++)
                {
                    packet.wrapped.push_back(msg[i]);
                }

                iretn = packet.Unwrap();

                if (iretn < 0)
                    return -1;
// push to 0 channel
                return 0;
            }
        }
    }
}

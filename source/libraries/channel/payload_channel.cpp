#include "rpi_channels.h"

namespace Artemis
{
    namespace RaspberryPi
    {
        namespace Channels
        {
            PayloadChannel::PayloadChannel()
            {
            }

            int32_t PayloadChannel::Init(Agent *agent)
            {
                this->channelAgent = agent;
                return 0;
            }

            // Communicates with Payload via PacketComm packets
            void PayloadChannel::Loop()
            {
                int32_t iretn = 0;

                PacketComm packet;
                channelNumber = channelAgent->channel_number("PAYLOAD");
                channelDataSize = 0;
                if (channelNumber >= 0)
                {
                    channelDataSize = channelAgent->channel_datasize(channelNumber);
                    channelDataSpeed = channelAgent->channel_speed(channelNumber);
                }

                channelAgent->debug_log.Printf("Starting Payload Loop\n");

                //                ElapsedTime et;
                while (channelAgent->running())
                {
                    // Comm - Internal
                    if ((iretn = channelAgent->channel_pull(channelNumber, packet)) > 0)
                    {
                        printf("%u\n", packet.header.type);
                        switch (packet.header.type)
                        {
                        case PacketComm::TypeId::CommandCameraCapture:
                        {
                            string response;
                            iretn = data_execute("python3 ~/artemis-cosmos-rpi-fsw/source/scripts/capture.py", response);
                            printf("%s\n", response);
                        }
                        break;
                        case PacketComm::TypeId::CommandObcHalt:
                        {
                            string response;
                            iretn = data_execute("sudo shutdown now", response);
                            printf("%s\n", response);
                        }
                        break;
                        }
                    }

                    std::this_thread::yield();
                }

                return;
            }
        }
    }
}

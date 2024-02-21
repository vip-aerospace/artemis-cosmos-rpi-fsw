#include "rpi_channels.h"

namespace Artemis
{
    namespace rpi
    {
        namespace Channel
        {
            PayloadChannel::PayloadChannel()
            {
            }

            int32_t PayloadChannel::Init(Agent *agent)
            {
                this->payloadAgent = agent;
                return 0;
            }

            // Communicates with Payload via PacketComm packets
            void PayloadChannel::Loop()
            {
                int32_t iretn = 0;

                PacketComm packet;
                payloadChannelNumber = payloadAgent->channel_number("PAYLOAD");
                payloadChannelDataSize = 0;
                if (payloadChannelNumber >= 0)
                {
                    payloadChannelDataSize = payloadAgent->channel_datasize(payloadChannelNumber);
                    payloadChannelDataSpeed = payloadAgent->channel_speed(payloadChannelNumber);
                }

                payloadAgent->debug_log.Printf("Starting Payload Loop\n");

                //                ElapsedTime et;
                while (payloadAgent->running())
                {
                    // Comm - Internal
                    if ((iretn = payloadAgent->channel_pull(payloadChannelNumber, packet)) > 0)
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

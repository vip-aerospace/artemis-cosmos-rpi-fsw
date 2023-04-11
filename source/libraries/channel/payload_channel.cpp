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
                this->agent = agent;
                return 0;
            }

            // Communicates with Payload via PacketComm packets
            void PayloadChannel::Loop()
            {
                int32_t iretn = 0;

                PacketComm packet;
                mychannel = agent->channel_number("PAYLOAD");
                mydatasize = 0;
                if (mychannel >= 0)
                {
                    mydatasize = agent->channel_datasize(mychannel);
                    mydataspeed = agent->channel_speed(mychannel);
                }

                agent->debug_log.Printf("Starting Payload Loop\n");

                //                ElapsedTime et;
                while (agent->running())
                {

                    // Comm - Internal
                    if ((iretn = agent->channel_pull(mychannel, packet)) > 0)

                    {
                        switch (packet.header.type)
                        {
                        case (PacketComm::TypeId)200: // TODO: PacketComm::TypeId::CommandTakePicture
                        {
                            string response;
                            iretn = data_execute("python3 ~/capture.py", response);
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

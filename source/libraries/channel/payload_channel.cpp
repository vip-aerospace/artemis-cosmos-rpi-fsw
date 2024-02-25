/**
 * @file payload_channel.cpp
 * @brief Definitions of the payload channel.
 * 
 * Defines the methods of the payload channel.
 */
#include "rpi_channels.h"

namespace Artemis
{
    namespace RaspberryPi
    {
        namespace Channels
        {
           /**
            * @brief Construct a new Payload Channel:: Payload Channel object
            */
            PayloadChannel::PayloadChannel() {}

            /**
             * @brief Payload channel initialization method.
             * 
             * This method describes the payload channel initialization. Like an
             * Arduino script, this method runs once, when the channel is first
             * being set up.
             * 
             * @param agent Agent: The agent that will contain this channel.
             * @return int32_t 0 on successful initialization, negative value if
             * unsuccessful.
             */
            int32_t PayloadChannel::Init(Agent *agent)
            {
                int32_t iretn;

                if (agent == NULL) {
                    iretn = COSMOS_GENERAL_ERROR_NULLPOINTER;
                    printf("Unable to initialize PayloadChannel under invalid Agent. iretn=%d\n", iretn);
                    return iretn;
                }

                this->channelAgent = agent;

                channelNumber = channelAgent->channel_number("PAYLOAD");
                if (channelNumber < 0)
                {
                    iretn = channelNumber;
                    channelAgent->debug_log.Printf("Error setting up payload channel number. iretn=%d\n", iretn);
                    return iretn;
                }

                channelDataSize = channelAgent->channel_datasize(channelNumber);
                channelDataSpeed = channelAgent->channel_speed(channelNumber);

                return 0;
            }

            /**
             * @brief Payload channel worker method.
             * 
             * This method describes the payload channel functionality. Like an
             * Arduino script, this method runs in an infinite loop. This method
             * runs in its own thread.
             */
            void PayloadChannel::Loop()
            {
                channelAgent->debug_log.Printf("Starting payload Loop\n");
                
                // ElapsedTime et;
                while (channelAgent->running())
                {
                    controlPayload();

                    std::this_thread::yield();
                }

                return;
            }

            /**
             * @brief Helper function to control the satellite's payload.
             * 
             * @todo Use proper debug logging and log relevant feedback.
             */
            void PayloadChannel::controlPayload()
            {
                int32_t iretn = channelAgent->channel_pull(channelNumber, incomingPacket);

                if (iretn <= 0)
                {
                    if(iretn < 0)
                    {
                        channelAgent->debug_log.Printf("Error in checking payload channel for incoming packet. iretn=%d\n", iretn);
                    }
                    return;
                }

                switch (incomingPacket.header.type)
                {
                    case PacketComm::TypeId::CommandCameraCapture:
                    {
                        string response;
                        iretn = data_execute("python3 ~/artemis-cosmos-rpi-fsw/source/scripts/capture.py", response);
                        channelAgent->debug_log.Printf("Tried to run CameraCapture command. iretn=%d response=%s\n", iretn, response.c_str());
                    }
                    break;
                    case PacketComm::TypeId::CommandObcHalt:
                    {
                        string response;
                        iretn = data_execute("sudo shutdown now", response);
                        channelAgent->debug_log.Printf("Tried to run OBCHalt command. iretn=%d response=%s\n", iretn, response.c_str());
                    }
                    break;
                }

                return;
            }
        }
    }
}

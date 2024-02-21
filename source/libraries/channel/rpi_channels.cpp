/**
 * @file rpi_channels.cpp
 * @brief Definitions of Raspberry Pi COSMOS channel.
 * 
 * Defines the variables and function of the Raspberry Pi COSMOS channel. 
 */
#include "rpi_channels.h"

namespace Artemis
{
    namespace RaspberryPi
    {
        namespace Channels
        {
            NODE_ID_TYPE rpi_node_id;
            NODE_ID_TYPE teensy_node_id;
            NODE_ID_TYPE ground_node_id;

            thread teensy_thread;
            thread payload_thread;
            thread file_thread;

            Cosmos::Module::FileModule *file_module;

            TeensyChannel *teensy_channel;
            PayloadChannel *payload_channel;

            /**
             * @brief A helper function to initialize the channels for the 
             * Raspberry Pi agent.
             * 
             * @param agent Agent: The Raspberry Pi agent that will contain the 
             * channels.
             * @param start_file bool: Whether to start the file channel. 
             * @param start_teensy bool: Whether to start the Teensy channel.
             * @param start_payload bool: Whether to start the payload channel.
             * @return int32_t Always returns zero.
             * 
             * @todo This always returns zero. It therefore either needs no 
             * return type and should be void, or should halt and return an 
             * error value.
             * 
             * @todo The file thread (and thus file channel) is never started.
             */
            int32_t init_rpi_channels(Agent *agent, bool start_file, bool start_teensy, bool start_payload)
            {
                int32_t iretn = 0;

                teensy_node_id = lookup_node_id(agent->cinfo, "teensy");
                ground_node_id = lookup_node_id(agent->cinfo, "ground");
                rpi_node_id = lookup_node_id(agent->cinfo, "rpi");

                // Start File Thread TODO: Uncomment when file thread is created
                // if (start_file)
                // {
                //     file_module = new Cosmos::Module::FileModule();
                //     iretn = file_module->Init(agent, {"ground"});
                //     if (iretn < 0)
                //     {
                //         printf("%f File Thread Init Error - Not Starting Loop: %s\n", agent->uptime.split(), cosmos_error_string(iretn).c_str());
                //         fflush(stdout);
                //     }
                //     else
                //     {
                //         file_thread = thread([=]
                //                              { file_module->Loop(); });
                //         secondsleep(3.);
                //         printf("%f File Thread started\n", agent->uptime.split());
                //         fflush(stdout);
                //     }
                //     // Set radios to use and in the order of the use priority, highest to lowest
                //     uint8_t TOTEENSY_CHANNELNUM = agent->channel_number("TOTEENSY");
                //     file_module->set_radios({TOTEENSY_CHANNELNUM});
                // }

                if (start_teensy)
                {
                    teensy_channel = new TeensyChannel();
                    iretn = teensy_channel->Init(agent);
                    if (iretn < 0)
                    {
                        printf("%f Teensy: Init Error - Not Starting Loop: %s\n", agent->uptime.split(), cosmos_error_string(iretn).c_str());
                        fflush(stdout);
                    }
                    else
                    {
                        teensy_thread = thread([=]
                                               { teensy_channel->Loop(); });
                        secondsleep(3.);
                        printf("%f Teensy: Thread started\n", agent->uptime.split());
                        fflush(stdout);
                    }
                }

                if (start_payload)
                {
                    payload_channel = new PayloadChannel();
                    iretn = payload_channel->Init(agent);
                    if (iretn < 0)
                    {
                        printf("%f Payload: Init Error - Not Starting Loop: %s\n", agent->uptime.split(), cosmos_error_string(iretn).c_str());
                        fflush(stdout);
                    }
                    else
                    {
                        payload_thread = thread([=]
                                                { payload_channel->Loop(); });
                        secondsleep(3.);
                        printf("%f Payload: Thread started\n", agent->uptime.split());
                        fflush(stdout);
                    }
                }

                printf("All threads started\n");
                fflush(stdout);

                return 0;
            }
        }
    }
}

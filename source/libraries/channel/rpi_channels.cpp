#include "rpi_channels.h"

namespace Artemis
{
    namespace rpi
    {
        namespace Channel
        {
            NodeData::NODE_ID_TYPE rpi_node_id;
            NodeData::NODE_ID_TYPE teensy_node_id;
            NodeData::NODE_ID_TYPE ground_node_id;

            thread teensy_thread;
            TeensyChannel *teensy_channel;
            thread payload_thread;
            PayloadChannel *payload_channel;
            thread file_thread;
            Cosmos::Module::FileModule *file_module;

            int32_t init_rpi_channels(Agent *agent, bool start_file, bool start_teensy, bool start_payload)
            {
                int32_t iretn = 0;

                teensy_node_id = agent->nodeData.lookup_node_id("teensy");
                ground_node_id = agent->nodeData.lookup_node_id("ground");
                rpi_node_id = agent->nodeData.lookup_node_id("rpi");

                if (start_file)
                {
                    file_module = new Cosmos::Module::FileModule();
                    iretn = file_module->Init(agent, {"ground"});
                    if (iretn < 0)
                    {
                        printf("%f FILE: Init Error - Not Starting Loop: %s\n", agent->uptime.split(), cosmos_error_string(iretn).c_str());
                        fflush(stdout);
                    }
                    else
                    {
                        file_thread = thread([=]
                                             { file_module->Loop(); });
                        secondsleep(3.);
                        printf("%f FILE: Thread started\n", agent->uptime.split());
                        fflush(stdout);
                    }
                    // Set radios to use and in the order of the use priority, highest to lowest
                    uint8_t TOTEENSY_CHANNELNUM = agent->channel_number("TOTEENSY");
                    file_module->set_radios({TOTEENSY_CHANNELNUM});
                }

                if (start_teensy)
                {
                    teensy_channel = new TeensyChannel();
                    iretn = teensy_channel->Init(agent);
                    if (iretn < 0)
                    {
                        printf("%f TEENSY: Init Error - Not Starting Loop: %s\n", agent->uptime.split(), cosmos_error_string(iretn).c_str());
                        fflush(stdout);
                    }
                    else
                    {
                        teensy_thread = thread([=]
                                               { teensy_channel->Loop(); });
                        secondsleep(3.);
                        printf("%f TEENSY: Thread started\n", agent->uptime.split());
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

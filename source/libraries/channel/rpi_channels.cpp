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
            thread file_thread;
            Cosmos::Module::FileModule *file_module;
            thread exec_thread;
            ExecChannel *exec_channel;

            int32_t init_rpi_channels(Agent *agent, bool start_exec, bool start_file, bool start_teensy)
            {
                int32_t iretn = 0;

                teensy_node_id = agent->nodeData.lookup_node_id("teensy");
                ground_node_id = agent->nodeData.lookup_node_id("ground");
                rpi_node_id = agent->nodeData.lookup_node_id("rpi");

                if (start_exec)
                {
                    exec_channel = new ExecChannel();
                    iretn = exec_channel->Init(agent);
                    if (iretn < 0)
                    {
                        printf("%f EXEC: Init Error - Not Starting Loop: %s\n", agent->uptime.split(), cosmos_error_string(iretn).c_str());
                        fflush(stdout);
                    }
                    else
                    {
                        exec_thread = thread([=]
                                             { exec_channel->Loop(); });
                        secondsleep(3.);
                        printf("%f EXEC: Thread started\n", agent->uptime.split());
                        fflush(stdout);
                    }
                }

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
                    // uint8_t XBAND = agent->channel_number("XBAND");
                    // uint8_t TXSI2C = agent->channel_number("TXSI2C");
                    // uint8_t UHFDOWN = agent->channel_number("UHFDOWN");
                    // file_module->set_radios({/*XBAND, */ TXSI2C, UHFDOWN});
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

                printf("All threads started\n");
                fflush(stdout);

                return 0;
            }
        }
    }
}

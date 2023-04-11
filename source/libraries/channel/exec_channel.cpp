#include "rpi_channels.h"
// #include <libcamera/camera.h>
#include "support/datalib.h"

// Misc utility stuff
static int32_t get_last_offset(Agent *agent);
static void move_and_compress_exec(Agent *agent);
static void move_and_compress_soh(Agent *agent);

namespace Artemis
{
    namespace rpi
    {
        namespace Channel
        {
            ExecChannel::ExecChannel()
            {
            }

            int32_t ExecChannel::Init(Agent *agent)
            {

                // // Set the immediate, incoming, outgoing, and temp directories
                // immediate_dir = data_base_path(agent->getNode(), "immediate", "exec") + "/";
                // if (immediate_dir.empty())
                // {
                //     cout << "unable to create directory: <" << (agent->getNode() + "/immediate") + "/exec"
                //          << "> ... exiting." << endl;
                //     exit(1);
                // }

                // string outgoing_dir = data_base_path(agent->getNode(), "outgoing", "exec") + "/";
                // if (outgoing_dir.empty())
                // {
                //     cout << "unable to create directory: <" << (agent->getNode() + "/outgoing") + "/exec"
                //          << "> ... exiting." << endl;
                //     exit(1);
                // }

                // string temp_dir = data_base_path(agent->getNode(), "temp", "exec") + "/";
                // if (temp_dir.empty())
                // {
                //     cout << "unable to create directory: <" << (agent->getNode() + "/temp") + "/exec"
                //          << "> ... exiting." << endl;
                //     exit(1);
                // }

                // load_dictionary(eventdict, agent->cinfo, "events.dict");

                // // Reload existing queue
                // cmd_queue.restore_commands(temp_dir);

                // // Create default logstring
                // logstring = json_list_of_soh(agent->cinfo);
                // printf("===\nlogstring: %s\n===\n", logstring.c_str());
                // fflush(stdout);
                // json_table_of_list(logtable, logstring.c_str(), agent->cinfo);

                // agent->set_sohstring(logstring);

                this->agent = agent;

                return 0;
            }

            void ExecChannel::Loop()
            {
                agent->debug_log.Printf("Starting Exec Loop\n");

                int32_t iretn = 0;

                PacketComm packet;
                mychannel = agent->channel_number("EXEC");
                mydatasize = 0;
                if (mychannel >= 0)
                {
                    mydatasize = agent->channel_datasize(mychannel);
                }

                // vector<eventstruc> events;

                // // Start performing the body of the agent
                // string jjstring;
                // double llogmjd, dlogmjd, clogmjd;
                // llogmjd = currentmjd();
                // clogmjd = currentmjd();
                // double logdate_exec = 0.;
                // double logstride_exec = 0.;
                // double newlogstride_exec = 300. / 86400.;
                // double logdate_soh = 0.;
                // double logperiod = 0.;
                // double newlogperiod = 30. / 86400.;
                // double logstride_soh = 0.;
                // double newlogstride_soh = 300. / 86400.;
                // ElapsedTime postet;
                // ElapsedTime savet;
                // ElapsedTime runet;
                // // The executive thread of the rpi will be the logger for this realm
                // bool log_data_flag = true;
                // loghandle = DataLog(60.);

                // FILE *fp = fopen((get_cosmosnodes() + agent->nodeName + "/last_state").c_str(), "w+");
                // if (fp != nullptr)
                // {
                //     fprintf(fp, "%s", "this is the last state message");
                //     fclose(fp);
                // }

                while (agent->running())
                {
                    // Comm - Internal
                    if ((iretn = agent->channel_pull(mychannel, packet)) > 0)
                    {
                        for (size_t i = 0; i < packet.wrapped.size(); i++)
                        {
                            agent->debug_log.Printf("%01X", packet.wrapped[i]);
                        }
                        agent->debug_log.Printf("\t");

                        for (size_t i = 0; i < packet.data.size(); i++)
                        {
                            agent->debug_log.Printf("%c", packet.data[i]);
                        }
                        agent->debug_log.Printf("\n");
                    }

                    // agent->cinfo->node.utc = clogmjd = currentmjd();
                    // dlogmjd = (clogmjd - llogmjd) * 86400.;

                    // if (savet.split() > 60.)
                    // {
                    //     FILE *fp = fopen((get_cosmosnodes() + agent->nodeName + "/last_date").c_str(), "w");
                    //     if (fp != nullptr)
                    //     {
                    //         savet.reset();
                    //         calstruc date = mjd2cal(currentmjd());
                    //         fprintf(fp, "%02d%02d%02d%02d%04d.59\n", date.month, date.dom, date.hour, date.minute, date.year);
                    //         fclose(fp);
                    //     }
                    // }

                    // // Move any new outgoing files to ground
                    // log_relocate(get_cosmosnodes() + agent->nodeName + "/outgoing/exec", get_cosmosnodes() + "ground/outgoing/exec/");

                    // secondsleep(5. - runet.split());
                    // runet.reset();
                }
            }

            /////////////////////////
            // Agent requests
            /////////////////////////

            // Moving exec and soh logs cannot occur concurrently.
            // void ExecChannel::move_and_compress_exec()
            // {
            //     // exec_mutex.lock();
            //     cmd_queue.join_event_threads();
            //     log_move(agent->getNode(), "exec");
            //     // exec_mutex.unlock();
            // }
            // void ExecChannel::move_and_compress_soh()
            // {
            //     // soh_mutex.lock();
            //     log_move(agent->getNode(), "soh");
            //     // soh_mutex.unlock();
            // }

            // /////////////////////////
            // // Utility functions
            // /////////////////////////
            // int32_t ExecChannel::get_last_offset()
            // {
            //     int32_t offset = 0;
            //     FILE *fp = fopen((get_cosmosnodes() + agent->nodeName + "/last_offset").c_str(), "r");
            //     if (fp != nullptr)
            //     {
            //         fscanf(fp, "%d", &offset);
            //         fclose(fp);
            //     }
            //     return offset;
            // }

        }
    }
}

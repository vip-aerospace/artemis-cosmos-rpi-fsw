/**
 * @file agent_rpi.cpp
 * @brief The Raspberry Pi communications agent.
 * @ingroup agents
 */

#include "support/configCosmos.h"
#include "support/convertlib.h"
#include "agent/agentclass.h"

#include "support/beacon.h"
#include "support/packetcomm.h"
#include "support/packethandler.h"
#include "device/cpu/devicecpu.h"
#include "device/disk/devicedisk.h"
#include "channel/rpi_channels.h"

using namespace Artemis::RaspberryPi::Channels;

/**
 * @brief Type definition for pointer to a function.
 **
 * This typedef defines a type for a pointer to a function accepting a single
 * boolean argument. It is used to pass a function as a parameter to another
 * function.
 */
typedef void (*on_event_switch)(bool active);
void Setup();
void Loop();
void Shutdown();
void fire_event(const vector<bool> flags, bool &event_switch, on_event_switch);
void update_radio_availability_for_file_transfer(bool active);
int32_t forward_to_payload_channel(PacketComm &packet, string &response, Agent *agent);

int32_t init_agent_rpi();
static int32_t get_last_offset();

int32_t iretn = 0;
PacketHandler packethandler;

// For external linkage
bool start_teensy = true;
bool start_exec = true;
bool start_file = true;
bool start_payload = true;

namespace
{
    /** @brief The Raspberry Pi COSMOS agent. */
    Agent *agent;
    double initialmjd;

    /** @brief The Raspberry Pi CPU. */
    DeviceCpu deviceCpu;
    /** @brief The CPU's device index. */
    uint16_t cpu_device_index;
    /** @brief The Raspberry Pi storage. */
    DeviceDisk deviceDisk;
    /** @brief The storage device index. */
    uint16_t disk_device_index;
    /** @brief The storage device information struct. */
    vector<DeviceDisk::info> disk_information;

    /** @todo Unused variables */
    // string sohstring;
    // vector<string> sohlist;
    // static uint32_t verification = 0xf853;
    // Error::LogType debug_level = Error::LOG_NONE;

    // Flags, flipped on triggering conditions
    // Set these
    bool toteensy_on_flag = true;
    // Events, flipped on certain combinations of flags
    // Not directly set
    bool toteensy_on_event_switch = false;
}

/**
 * @brief The main agent function.
 * 
 * @param argc int: The number of arguments to the Raspberry Pi agent.
 * @param argv char[]: The values of the arguments passed in to the Raspberry Pi
 * agent.
 * @return int Always returns zero.
 * 
 * @todo The return value is always zero. The type of this function *might* have
 * to remain an int regardless.
 * 
 * @todo Break up into helper functions.
 */



int main(int argc, char *argv[])
{
    Setup(iretn, packethandler);
    Loop(iretn, packethandler);
    Shutdown();

    return 0;
}
void Setup(int32_t&iretn, PacketHandler &packethandler) {
    //int32_t iretn = 0;

    /////////////////////////////////////////////////////
    // Setup
    /////////////////////////////////////////////////////
    // Argument validation
    // Optional arg
    // agent_rpi [debug_level]
   /* if (argc > 1 && (argv[1][0] >= '0' && argv[1][0] <= '4'))
    {
        // debug_level = static_cast<Error::LogType>(argv[1][0] - '0');
    }
    if (argc == 4)
    {
        if (argv[2][0] == '0')
        {
            start_teensy = false;
        }
        if (argv[3][0] == '0')
        {
            start_file = false;
        }
    }*/

    // Agent initialization
    /**
     * Note that init_agent_rpi() always returns zero. It does not return 
     * anything if it fails, as the program is terminated right then and there.
     */
    iretn = init_agent_rpi();

    // Initialize packet handler
    //PacketHandler packethandler;
    packethandler.init(agent);
    packethandler.add_func((PacketComm::TypeId)200, forward_to_payload_channel);

    // Define agent requests
    agent->cinfo->agent0.aprd = 1.;
    agent->start_active_loop();
}

void Loop(int32_t &iretn, PacketHandler &packethandler) {
    ////////////////////////////////////////////////////
    // Main Loop
    ////////////////////////////////////////////////////
    agent->debug_log.Printf("Start main Agent loop\n");
    ElapsedTime savet;
    int32_t mychannel = 0;
    ElapsedTime tet;
    PacketComm packet;
    while (agent->running())
    {
        // Check events
        // fire_event({toteensy_on_flag}, toteensy_on_event_switch, &update_radio_availability_for_file_transfer);

        // if (toteensy_on_event_switch)
        // {
        // }

        // Update rpi cpu telemetry
        // if (tet.split() > 5.)
        // {
        //     agent->cinfo->node.utc = currentmjd();
        //     agent->cinfo->node.deci = decisec(agent->cinfo->node.utc);
        //     agent->cinfo->devspec.cpu[cpu_device_index].utc = currentmjd();
        //     agent->cinfo->devspec.cpu[cpu_device_index].uptime = deviceCpu.getUptime();
        //     agent->cinfo->devspec.cpu[cpu_device_index].load = static_cast<float>(deviceCpu.getLoad());
        //     agent->cinfo->devspec.cpu[cpu_device_index].gib = static_cast<float>(deviceCpu.getVirtualMemoryUsed() / 1073741824.);
        //     disk_information = deviceDisk.getInfo();
        //     for (uint16_t i = 0; i < disk_information.size(); ++i)
        //     {
        //         printf("Disk %u: mount=%s size=%lu used=%lu\n", i, disk_information[i].mount.c_str(), disk_information[i].size, disk_information[i].used);
        //         if (disk_information[i].mount == "/")
        //         {
        //             if (disk_information[i].size)
        //             {
        //                 agent->cinfo->devspec.cpu[cpu_device_index].storage = disk_information[i].used / static_cast<float>(disk_information[i].size);
        //             }
        //             else
        //             {
        //                 agent->cinfo->devspec.cpu[cpu_device_index].storage = 0.;
        //             }
        //         }
        //     }
        //     tet.reset();
        // }

        // Comm - Internal
        if ((iretn = agent->channel_pull(mychannel, packet)) > 0)
        {
            string response;
            packethandler.process(packet, response);
            // if (packet.header.radio > 0)
            // {
            //     // agent->push_response(packet.header.radio, mychannel, packet.header.orig, 0, response);
            // }
        }

        // Update saved time
        // if (savet.split() > 60.)
        // {
        //     printf("%f Save Time\n", agent->uptime.split());
        //     FILE *fp = fopen((get_cosmosnodes() + agent->nodeName + "/last_date").c_str(), "w+");
        //     if (fp)
        //     {
        //         savet.reset();
        //         calstruc date = mjd2cal(currentmjd());
        //         fprintf(fp, "%02d%02d%02d%02d%04d.59\n", date.month, date.dom, date.hour, date.minute, date.year);
        //         fclose(fp);
        //     }
        // }

        std::this_thread::yield();
        }
}

void Shutdown() {
    ////////////////////////////////////////////////
    // Shutdown
    ///////////////////////////////////////////////
    agent->shutdown();
    // Wait for all other running threads to finish.
    teensy_thread.join();
    file_thread.join();
}

/////////////////////////
// Agent requests
/////////////////////////

/////////////////////////
// Utility functions
/////////////////////////

/**
 * @brief Helper function to initialize the Raspberry Pi COSMOS agent.
 * 
 * @return int32_t Returns zero on successful initialization. Exits the program
 * with an error code if initialization fails.
 * 
 * @todo Note that the function only returns a value if it succeeds. If some
 * step fails in the initialization, the entire program terminates here, and 
 * nothing is returned. Consider changing the type of this helper function to
 * void.
 * 
 * @todo Consider breaking up this function into smaller, easier to understand,
 * functions.
 */
int32_t init_agent_rpi()
{
    int32_t iretn = 0;
    agent = new Agent("", "rpi", "rpi", 0., 10000, false, 0, NetworkType::UDP, 2);
    agent->set_debug_level(1);
    initialmjd = currentmjd();

    // Set channels
    agent->set_verification(0xf853);
    agent->channel_add("TOTEENSY", 50); // 50-8 50 is max size of radio, 8 is packet header size
    agent->channel_add("PAYLOAD");

    // Set time
    FILE *fp = fopen((get_cosmosnodes() + agent->cinfo->node.name + "/last_date").c_str(), "r");
    if (fp != nullptr)
    {
        calstruc date;
        fscanf(fp, "%02d%02d%02d%02d%04d%*c%02d\n", &date.month, &date.dom, &date.hour, &date.minute, &date.year, &date.second);
        fclose(fp);
        int32_t offset = get_last_offset();
        date.second += offset;
        initialmjd = currentmjd();
        double delta = cal2mjd(date) - currentmjd();
        if (delta > 3.5e-4)
        {
            delta = set_local_clock(cal2mjd(date));
            agent->debug_log.Printf("Initialized time from file: Delta %f Offset %d\n", delta, offset);
        }
        else
        {
            agent->debug_log.Printf("No change from file: Delta %f\n", delta);
        }
    }

    if ((iretn = agent->wait()) < 0)
    {
        agent->debug_log.Printf("%16.10f %s Failed to start Agent %s on Node %s: %s\n", currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), cosmos_error_string(iretn).c_str());
        exit(iretn);
    }
    else
    {
        agent->debug_log.Printf("%16.10f %s Started Agent %s on Node %s\n", currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str());
    }

    // Information for Mission Elapsed Time
    string idate = get_cosmosnodes() + agent->cinfo->node.name + "/initial_date";
    idate = "initial_date";
    fp = fopen(idate.c_str(), "r");
    calstruc date;
    if (fp != nullptr)
    {
        printf("Reading initial date\n");
        fflush(stdout);
        fscanf(fp, "%02d%02d%02d%02d%04d%*c%02d\n", &date.month, &date.dom, &date.hour, &date.minute, &date.year, &date.second);
        fclose(fp);
        agent->cinfo->node.utcstart = cal2mjd(date);
    }
    else
    {
        printf("Setting initial date\n");
        fflush(stdout);
        double mjd = currentmjd();
        agent->cinfo->node.utcstart = mjd;
        date = mjd2cal(mjd);
        fp = fopen(idate.c_str(), "w+");
        fprintf(fp, "%02d%02d%02d%02d%04d.59\n", date.month, date.dom, date.hour, date.minute, date.year);
        fclose(fp);
    }

    // double rmjd;
    // double epsilon;
    // double offset;
    // iretn = agent->get_agent_time(rmjd, epsilon, offset, "cpu");
    // if (iretn >= 0 && offset > 3.5e-4)
    // {
    //     double delta = set_local_clock(currentmjd() + offset);
    //     agent->debug_log.Printf("Initialized time from server: Delta %f Offset %f\n", delta, offset * 86400.);
    // }
    // else
    // {
    //     agent->debug_log.Printf("No change from server: Offset %f\n", offset * 86400.);
    // }

    // // OBC Devices
    // iretn = json_findpiece(agent->cinfo, "rpi_cpu");
    // if (iretn < 0)
    // {
    //     agent->debug_log.Printf("Error loading CPU information\n", cosmos_error_string(iretn).c_str());
    //     agent->shutdown();
    //     exit(iretn);
    // }
    // cpu_device_index = agent->cinfo->device[agent->cinfo->pieces[static_cast<uint16_t>(iretn)].cidx]->didx;

    // agent->cinfo->devspec.cpu[cpu_device_index].utc = currentmjd();
    // agent->cinfo->devspec.cpu[cpu_device_index].uptime = deviceCpu.getUptime();
    // agent->cinfo->devspec.cpu[cpu_device_index].boot_count = deviceCpu.getBootCount();
    // agent->cinfo->devspec.cpu[cpu_device_index].load = static_cast<float>(deviceCpu.getLoad());
    // agent->cinfo->devspec.cpu[cpu_device_index].gib = static_cast<float>(deviceCpu.getVirtualMemoryUsed() / 1073741824.);
    // agent->cinfo->devspec.cpu[cpu_device_index].maxgib = static_cast<float>(deviceCpu.getVirtualMemoryTotal() / 1073741824.);
    // agent->cinfo->devspec.cpu[cpu_device_index].maxload = deviceCpu.getCpuCount();
    // deviceCpu.numProcessors = agent->cinfo->devspec.cpu[cpu_device_index].maxload;

    // iretn = json_findpiece(agent->cinfo, "rpi_disk");
    // if (iretn < 0)
    // {
    //     agent->debug_log.Printf("Error loading Disk information\n", cosmos_error_string(iretn).c_str());
    //     agent->shutdown();
    //     exit(iretn);
    // }
    // disk_device_index = agent->cinfo->device[agent->cinfo->pieces[static_cast<uint16_t>(iretn)].cidx]->didx;

    // disk_information = deviceDisk.getInfo();
    // for (uint16_t i = 0; i < disk_information.size(); ++i)
    // {
    //     if (disk_information[i].mount == "/")
    //     {
    //         if (disk_information[i].size)
    //         {
    //             agent->cinfo->devspec.cpu[cpu_device_index].storage = disk_information[i].used / static_cast<float>(disk_information[i].size);
    //         }
    //         else
    //         {
    //             agent->cinfo->devspec.cpu[cpu_device_index].storage = 0.;
    //         }
    //         agent->cinfo->devspec.disk[disk_device_index].path = disk_information[i].mount.c_str();
    //     }
    // }

    // agent->set_sohstring(sohlist);
    json_updatecosmosstruc(agent->cinfo);

    init_rpi_channels(agent, start_file, start_teensy, start_payload);

    return 0;
}

static int32_t get_last_offset()
{
    int32_t offset = 0;
    FILE *fp = fopen((get_cosmosnodes() + agent->cinfo->node.name + "/last_offset").c_str(), "r");
    if (fp != nullptr)
    {
        fscanf(fp, "%d", &offset);
        fclose(fp);
    }
    return offset;
}

/**
 * @brief Event-triggered function to indicate a change in radio availability 
 * for file transfers.
 * 
 * This function is used to signal whether the radio is available for file 
 * transfers. Importantly, it is only run when triggered by an event. The 
 * triggering event for this function is whether the radio availability has 
 * changed (either from true to false, or false to true).
 * 
 * @param availability bool: Whether the radio is available for file transfer.
 */
void update_radio_availability_for_file_transfer(bool availability)
{
    PacketHandler::QueueTransferRadio(agent->channel_number("TOTEENSY"), availability, agent, rpi_node_id);
    return;
}

/**
 * @brief Helper function to trigger another function based on boolean flag 
 * state change
 * 
 * This function triggers another function if and only if the following 
 * conditions are met:
 * 
 * - All the passed-in flags are true, AND the event switch is false
 * OR
 * - At least one of the passed-in flags is false, AND the event switch is true
 * 
 * If either condition is met, the event switch is toggled to the opposite value
 * (false becomes true, or true becomes false), and it is passed as an argument 
 * to the specified function. Note that if the event switch is not toggled, the
 * event function is not triggered.
 *
 * @param flags vector<bool>: The flags to be checked against the event switch.
 * @param event_switch bool: The initial state of the event switch. This switch
 * could be changed by the time this function is done with it.
 * @param eventHandler function(bool): The function to be called if the event 
 * switch is toggled. The argument to the function is the post-toggle event 
 * switch boolean.
 */
void fire_event(const vector<bool> flags, bool &event_switch, void (*eventHandler)(bool))
{
    bool aggregateFlag = true;
    for (const bool flag : flags)
    {
        if (!flag)
        {
            aggregateFlag = false;
            break;
        }
    }
    if (event_switch != aggregateFlag)
    {
        event_switch = aggregateFlag;
        eventHandler(event_switch);
    }
}

/**
 * @brief Function to forward a packet to the payload channel.
 * 
 * This function is added as an externally-defined function of the PacketHandler
 * object. It gives the PacketHandler the ability to route packets to the 
 * payload channel.
 * 
 * @param packet PacketComm: The packet to be forwarded.
 * @param response string: The response from the packet handler.
 * @param agent Agent: The packet handler agent, which does the forwarding.
 * @return int32_t Size of packet in bytes if successfully forwarded, negative 
 * error value if unsuccessful .
 */
int32_t forward_to_payload_channel(PacketComm &packet, string &response, Agent *agent)
{
    return agent->channel_push(agent->channel_number("PAYLOAD"), packet);
}
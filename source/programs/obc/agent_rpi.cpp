/*! brief rpi communications Agent
 */

//! \ingroup agents

#include "support/configCosmos.h"
#include "support/convertlib.h"
#include "agent/agentclass.h"

#include "support/beacon.h"
#include "support/packetcomm.h"
#include "support/packethandler.h"
#include "device/cpu/devicecpu.h"
#include "device/disk/devicedisk.h"
#include "channel/rpi_channels.h"

using namespace Artemis::rpi::Channel;

// For external linkage
bool start_teensy = true;
bool start_exec = true;
bool start_file = true;

namespace
{
    uint16_t cpu_didx;
    uint16_t disk_didx;
    // string sohstring;
    Agent *agent;
    double initialmjd;
    // vector<string> sohlist;
    DeviceCpu deviceCpu;
    // static uint32_t verification = 0xf853;
    DeviceDisk deviceDisk;
    vector<DeviceDisk::info> dinfo;

    Error::LogType debug_level = Error::LOG_NONE;

    // Flags, flipped on triggering conditions
    // Set these
    bool toteensy_on_flag = false;
    // Events, flipped on certain combinations of flags
    // Not directly set
    bool toteensy_on_event_switch = false;
}

// Misc utility stuff
int32_t init_agent_rpi();
static int32_t get_last_offset();

int main(int argc, char *argv[])
{
    int32_t iretn = 0;

    // Optional arg
    // agent_rpi [debug_level]
    if (argc > 1 && (argv[1][0] >= '0' && argv[1][0] <= '4'))
    {
        debug_level = static_cast<Error::LogType>(argv[1][0] - '0');
    }
    if (argc == 5)
    {
        if (argv[2][0] == '0')
        {
            start_teensy = false;
        }
        if (argv[3][0] == '0')
        {
            start_exec = false;
        }
        if (argv[4][0] == '0')
        {
            start_file = false;
        }
    }

    iretn = init_agent_rpi();

    // Initialize packet handler
    PacketHandler packethandler;
    packethandler.init(agent);

    // Define agent requests

    agent->cinfo->agent[0].aprd = 1.;
    agent->start_active_loop();

    // Start performing the body of the agent
    agent->debug_error.Printf("Start main Agent loop\n");
    ElapsedTime savet;
    int32_t mychannel = 0;
    ElapsedTime tet;
    PacketComm packet;
    while (agent->running())
    {

        // Update rpi cpu telemetry
        if (tet.split() > 5.)
        {
            agent->cinfo->node.utc = currentmjd();
            agent->cinfo->node.deci = decisec(agent->cinfo->node.utc);
            agent->cinfo->devspec.cpu[cpu_didx].utc = currentmjd();
            agent->cinfo->devspec.cpu[cpu_didx].uptime = deviceCpu.getUptime();
            agent->cinfo->devspec.cpu[cpu_didx].load = static_cast<float>(deviceCpu.getLoad());
            agent->cinfo->devspec.cpu[cpu_didx].gib = static_cast<float>(deviceCpu.getVirtualMemoryUsed() / 1073741824.);
            dinfo = deviceDisk.getInfo();
            for (uint16_t i = 0; i < dinfo.size(); ++i)
            {
                printf("Disk %u: mount=%s size=%lu used=%lu\n", i, dinfo[i].mount.c_str(), dinfo[i].size, dinfo[i].used);
                if (dinfo[i].mount == "/")
                {
                    if (dinfo[i].size)
                    {
                        agent->cinfo->devspec.cpu[cpu_didx].storage = dinfo[i].used / static_cast<float>(dinfo[i].size);
                    }
                    else
                    {
                        agent->cinfo->devspec.cpu[cpu_didx].storage = 0.;
                    }
                }
            }
            tet.reset();
        }

        // Comm - Internal
        if ((iretn = agent->channel_pull(mychannel, packet)) > 0)
        {
        }

        // Update saved time
        if (savet.split() > 60.)
        {
            printf("%f Save Time\n", agent->uptime.split());
            FILE *fp = fopen((get_cosmosnodes() + agent->nodeName + "/last_date").c_str(), "w+");
            if (fp)
            {
                savet.reset();
                calstruc date = mjd2cal(currentmjd());
                fprintf(fp, "%02d%02d%02d%02d%04d.59\n", date.month, date.dom, date.hour, date.minute, date.year);
                fclose(fp);
            }
        }

        std::this_thread::yield();
    }

    agent->shutdown();

    // Join all running threads
    teensy_thread.join();
    file_thread.join();

    return 0;
}

/////////////////////////
// Agent requests
/////////////////////////

/////////////////////////
// Utility functions
/////////////////////////
int32_t init_agent_rpi()
{
    int32_t iretn = 0;
    agent = new Agent("artemis", "rpi", 0., 10000, false, 0, NetworkType::UDP, 2);
    agent->set_debug_level(debug_level);
    initialmjd = currentmjd();

    // Set channels
    agent->set_verification(0xf853);
    agent->channel_add("TOTEENSY", Support::Channel::PACKETCOMM_DATA_SIZE, 18000.);
    agent->channel_add("EXEC", Support::Channel::PACKETCOMM_DATA_SIZE, 18000.);

    // Set time
    FILE *fp = fopen((get_cosmosnodes() + agent->nodeName + "/last_date").c_str(), "r");
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
            agent->debug_error.Printf("Initialized time from file: Delta %f Offset %d\n", delta, offset);
        }
        else
        {
            agent->debug_error.Printf("No change from file: Delta %f\n", delta);
        }
    }

    if ((iretn = agent->wait()) < 0)
    {
        agent->debug_error.Printf("%16.10f %s Failed to start Agent %s on Node %s: %s\n", currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), cosmos_error_string(iretn).c_str());
        exit(iretn);
    }
    else
    {
        agent->debug_error.Printf("%16.10f %s Started Agent %s on Node %s\n", currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str());
    }

    // Information for Mission Elapsed Time
    string idate = get_cosmosnodes() + agent->nodeName + "/initial_date";
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

    double rmjd;
    double epsilon;
    double offset;
    iretn = agent->get_agent_time(rmjd, epsilon, offset, "cpu");
    if (iretn >= 0 && offset > 3.5e-4)
    {
        double delta = set_local_clock(currentmjd() + offset);
        agent->debug_error.Printf("Initialized time from server: Delta %f Offset %f\n", delta, offset * 86400.);
    }
    else
    {
        agent->debug_error.Printf("No change from server: Offset %f\n", offset * 86400.);
    }

    // OBC Devices
    iretn = json_findpiece(agent->cinfo, "rpi_cpu");
    if (iretn < 0)
    {
        agent->debug_error.Printf("Error loading CPU information\n", cosmos_error_string(iretn).c_str());
        agent->shutdown();
        exit(iretn);
    }
    cpu_didx = agent->cinfo->device[agent->cinfo->pieces[static_cast<uint16_t>(iretn)].cidx]->didx;

    agent->cinfo->devspec.cpu[cpu_didx].utc = currentmjd();
    agent->cinfo->devspec.cpu[cpu_didx].uptime = deviceCpu.getUptime();
    agent->cinfo->devspec.cpu[cpu_didx].boot_count = deviceCpu.getBootCount();
    agent->cinfo->devspec.cpu[cpu_didx].load = static_cast<float>(deviceCpu.getLoad());
    agent->cinfo->devspec.cpu[cpu_didx].gib = static_cast<float>(deviceCpu.getVirtualMemoryUsed() / 1073741824.);
    agent->cinfo->devspec.cpu[cpu_didx].maxgib = static_cast<float>(deviceCpu.getVirtualMemoryTotal() / 1073741824.);
    agent->cinfo->devspec.cpu[cpu_didx].maxload = deviceCpu.getCpuCount();
    deviceCpu.numProcessors = agent->cinfo->devspec.cpu[cpu_didx].maxload;

    iretn = json_findpiece(agent->cinfo, "rpi_disk");
    if (iretn < 0)
    {
        agent->debug_error.Printf("Error loading Disk information\n", cosmos_error_string(iretn).c_str());
        agent->shutdown();
        exit(iretn);
    }
    disk_didx = agent->cinfo->device[agent->cinfo->pieces[static_cast<uint16_t>(iretn)].cidx]->didx;

    dinfo = deviceDisk.getInfo();
    for (uint16_t i = 0; i < dinfo.size(); ++i)
    {
        if (dinfo[i].mount == "/")
        {
            if (dinfo[i].size)
            {
                agent->cinfo->devspec.cpu[cpu_didx].storage = dinfo[i].used / static_cast<float>(dinfo[i].size);
            }
            else
            {
                agent->cinfo->devspec.cpu[cpu_didx].storage = 0.;
            }
            agent->cinfo->devspec.disk[disk_didx].path = dinfo[i].mount.c_str();
        }
    }

    // agent->set_sohstring(sohlist);
    json_updatecosmosstruc(agent->cinfo);

    init_rpi_channels(agent, start_exec, start_file, start_teensy);

    return 0;
}

static int32_t get_last_offset()
{
    int32_t offset = 0;
    FILE *fp = fopen((get_cosmosnodes() + agent->nodeName + "/last_offset").c_str(), "r");
    if (fp != nullptr)
    {
        fscanf(fp, "%d", &offset);
        fclose(fp);
    }
    return offset;
}

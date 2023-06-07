#include "support/configCosmos.h"
#include "support/cosmos-errno.h"
#include "agent/agentclass.h"

static Agent *agent;

int main(int argc, char *argv[])
{
    int32_t iretn = 0;
    if (argc == 3)
    {
        agent = new Agent(argv[1], argv[2]);
    }
    else if (argc == 2)
    {
        agent = new Agent(argv[1]);
    }
    else
    {
        agent = new Agent();
    }

    // rpi
    iretn = json_createpiece(agent->cinfo, "rpi_cpu", DeviceType::CPU);
    iretn = json_createpiece(agent->cinfo, "rpi_disk", DeviceType::DISK);

    // teensy
    iretn = json_createpiece(agent->cinfo, "teensy_disk", DeviceType::DISK);

    json_dump_node(agent->cinfo);
}

#include "support/configCosmos.h"
#include "support/cosmos-errno.h"
#include "agent/agentclass.h"

static Agent *agent;

int main(int argc, char *argv[])
{
    int32_t iretn = 0;
    if (argc == 2)
    {
        agent = new Agent(argv[1]);
    }
    else
    {
        agent = new Agent();
    }

    // CPU
    for (uint16_t i = 0; i < agent->cinfo->devspec.cpu.size(); ++i)
    {
        printf("CPU %u %s: pidx=%u cidx=%u didx=%u uptime=%u bootcount=%u\n", i, agent->cinfo->devspec.cpu[i].name.c_str(), agent->cinfo->devspec.cpu[i].pidx, agent->cinfo->devspec.cpu[i].cidx, agent->cinfo->devspec.cpu[i].didx, agent->cinfo->devspec.cpu[i].uptime, agent->cinfo->devspec.cpu[i].boot_count);
    }
    printf("\n");

    // TSEN
    for (uint16_t i = 0; i < agent->cinfo->devspec.tsen.size(); ++i)
    {
        printf("TSEN %u %s: pidx=%u cidx=%u didx=%u temperature=%f\n", i, agent->cinfo->devspec.tsen[i].name.c_str(), agent->cinfo->devspec.tsen[i].pidx, agent->cinfo->devspec.tsen[i].cidx, agent->cinfo->devspec.tsen[i].didx, agent->cinfo->devspec.tsen[i].temp);
    }
    printf("\n");

    // EPS SWCH
    for (uint16_t i = 0; i < agent->cinfo->devspec.swch.size(); ++i)
    {
        printf("SWCH %u %s: pidx=%u cidx=%u didx=%u volt=%f amp=%f\n", i, agent->cinfo->devspec.swch[i].name.c_str(), agent->cinfo->devspec.swch[i].pidx, agent->cinfo->devspec.swch[i].cidx, agent->cinfo->devspec.swch[i].didx, agent->cinfo->devspec.swch[i].volt, agent->cinfo->devspec.swch[i].amp);
    }
    printf("\n");

    // EPS BATT
    for (uint16_t i = 0; i < agent->cinfo->devspec.batt.size(); ++i)
    {
        printf("BATT %u %s: pidx=%u cidx=%u didx=%u volt=%f amp=%f\n", i, agent->cinfo->devspec.batt[i].name.c_str(), agent->cinfo->devspec.batt[i].pidx, agent->cinfo->devspec.batt[i].cidx, agent->cinfo->devspec.batt[i].didx, agent->cinfo->devspec.batt[i].volt, agent->cinfo->devspec.batt[i].amp);
    }
    printf("\n");
}

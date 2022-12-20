#include "device/i2c/i2c.h"
#include "support/packetcomm.h"
#include "support/configCosmosKernel.h"
#include <string>
#include <vector>
#include <unistd.h>

using Cosmos::I2C;

int main(int argc, const char *argv[])
{
    int32_t iretn;
    PacketComm packet;

    cout << "testing I2C library" << endl;
    // 0x08 - motor controller board
    // 0x57 - CubeADCS
    I2C *i2c = new I2C("/dev/i2c-1", 0x08);
    iretn = i2c->connect();
    printf("connect: %d\n", iretn);

    iretn = i2c->send((uint8_t *)"dsdsa", 5);

    printf("send: %d\n", iretn);

    std::string msg;

    while (1)
    {
        iretn = i2c->receive(msg, 50);

        if (iretn <= 0 || msg.length() < sizeof(PacketComm::header) + sizeof(PacketComm::crc))
            continue;

        packet.wrapped.resize(0);
        uint8_t data_len = msg[0];
        for (size_t i = 0; i < sizeof(PacketComm::header) + data_len + sizeof(PacketComm::crc); i++)
        {
            packet.wrapped.push_back(msg[i]);
        }

        iretn = packet.Unwrap();

        if (iretn < 0)
            continue;

        for (size_t i = 0; i < packet.wrapped.size(); i++)
        {
            printf("%01X", packet.wrapped[i]);
        }
        printf("\t");

        for (size_t i = 0; i < packet.data.size(); i++)
        {
            printf("%c", packet.data[i]);
        }
        printf("\n");
    }
}

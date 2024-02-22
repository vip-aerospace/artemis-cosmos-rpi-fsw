/**
 * @file teensy_channel.cpp
 * @brief Definitions of the Teensy channel.
 * 
 * Defines the methods of the Teensy channel.
 */
#include "rpi_channels.h"

namespace Artemis
{
    namespace RaspberryPi
    {
        namespace Channels
        {
            TeensyChannel::TeensyChannel()
            {
            }

            /**
             * @brief Teensy channel initialization method.
             * 
             * This method describes the Teensy channel initialization. Like an 
             * Arduino script, this method runs once, when the channel is first 
             * being set up.
             * 
             * @param agent Agent: The agent that will contain this channel.
             * @return int32_t 0 on successful initialization, negative value if
             * unsuccessful.
             */
            int32_t TeensyChannel::Init(Agent *agent)
            {
                this->teensyAgent = agent;

                serial = new Serial("/dev/ttyS0", 9600);

                int32_t iretn = serial->get_error();
                if (iretn < 0)
                {
                    printf("Error setting up Teensy UART serial connection. iretn=%d\n", iretn);
                    return iretn;
                }

                serial->set_rtimeout(1.);
                serial->set_wtimeout(1.);
                serial->set_flowcontrol(0, 0);
                serial->drain();

                teensyChannelNumber = teensyAgent->channel_number("TOTEENSY");
                if (teensyChannelNumber < 0)
                {
                    iretn = teensyChannelNumber;
                    printf("Error setting up Teensy channel number. iretn=%d\n", iretn);
                    return iretn;
                }

                teensyChannelDataSize = teensyAgent->channel_datasize(teensyChannelNumber);
                teensyChannelDataSpeed = teensyAgent->channel_speed(teensyChannelNumber);

                return 0;
            }

            // Communicates with Teensy via PacketComm packets
            void TeensyChannel::Loop()
            {
                int32_t iretn = 0;

                PacketComm packet;

                teensyAgent->debug_log.Printf("Starting Teensy Loop\n");

                // ElapsedTime et;
                while (teensyAgent->running())
                {
                    struct sysinfo meminfoin;
                    sysinfo(&meminfoin);

                    // I2C Communication with Teensy
                    // if (i2c_recv(packet) >= 0)
                    // {
                    //     iretn = teensyAgent->channel_push(0, packet);
                    // }

                    if (serial->get_open())
                    {
                        packet.packetized.clear();
                        if ((iretn = serial->get_slip(packet.packetized)) > 0)
                        {
                            iretn = packet.RawUnPacketize();
                        }
                        if (iretn >= 0)
                        {
                            printf("%d\n", packet.header.type);
                            switch (packet.header.type)
                            {
                            case PacketComm::TypeId::CommandCameraCapture:
                            case PacketComm::TypeId::CommandObcHalt:
                                teensyAgent->channel_push("PAYLOAD", packet);
                                break;

                            default:
                                teensyAgent->channel_push(0, packet);
                                break;
                            }
                        }
                    }

                    // Comm - Internal
                    if ((iretn = teensyAgent->channel_pull(teensyChannelNumber, packet)) > 0)
                    {
                    }

                    std::this_thread::yield();
                }

                return;
            }

            int32_t TeensyChannel::i2c_recv(PacketComm &packet)
            {
                int32_t iretn = 0;
                std::string msg;

                // I2C From Teensy
                iretn = i2c->receive(msg, 50);

                if (iretn <= 0 || msg.length() < sizeof(PacketComm::header) + sizeof(PacketComm::crc))
                    return -1;

                packet.wrapped.resize(0);
                uint8_t data_len = msg[0];
                for (size_t i = 0; i < sizeof(PacketComm::header) + data_len + sizeof(PacketComm::crc); i++)
                {
                    packet.wrapped.push_back(msg[i]);
                }

                iretn = packet.Unwrap();

                if (iretn < 0)
                    return -1;
                // push to 0 channel
                return 0;
            }
        }
    }
}

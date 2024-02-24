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
            /** @brief Construct a new Teensy Channel:: Teensy Channel object */
            TeensyChannel::TeensyChannel() {}

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
             * 
             * @todo Validate that the argument is valid.
             */
            int32_t TeensyChannel::Init(Agent *agent)
            {
                this->channelAgent = agent;

                serial = new Serial("/dev/ttyS0", 9600);

                int32_t iretn = serial->get_error();
                if (iretn < 0)
                {
                    channelAgent->debug_log.Printf("Error setting up Teensy UART serial connection. iretn=%d\n", iretn);
                    return iretn;
                }

                serial->set_rtimeout(1.);
                serial->set_wtimeout(1.);
                serial->set_flowcontrol(0, 0);
                serial->drain();

                channelNumber = channelAgent->channel_number("TOTEENSY");
                if (channelNumber < 0)
                {
                    iretn = channelNumber;
                    channelAgent->debug_log.Printf("Error setting up Teensy channel number. iretn=%d\n", iretn);
                    return iretn;
                }

                channelDataSize = channelAgent->channel_datasize(channelNumber);
                channelDataSpeed = channelAgent->channel_speed(channelNumber);

                return 0;
            }

            /**
             * @brief Teensy channel worker method.
             * 
             * This method describes the Teensy channel functionality. Like an
             * Arduino script, this method runs in an infinite loop. This method
             * runs in its own thread.
             * 
             * @todo This includes communication with the Teensy over I2C. Check
             * that I2C will ever be used for this purpose. If not, remove this
             * code.
             */
            void TeensyChannel::Loop()
            {
                channelAgent->debug_log.Printf("Starting Teensy Loop\n");

                // ElapsedTime et;
                while (channelAgent->running())
                {
                    struct sysinfo meminfoin;
                    sysinfo(&meminfoin);

                    receiveFromTeensySerial();
                    sendToTeensySerial();

                    // receiveFromTeensyI2C();
                    // sendToTeensyI2C();                  

                    std::this_thread::yield();
                }

                return;
            }

            /**
             * @brief Helper function to receive a packet from the Teensy over 
             * UART serial.
             */
            void TeensyChannel::receiveFromTeensySerial() 
            {
                int32_t iretn;

                if (serial->get_open())
                {
                    incomingPacket.packetized.clear();
                    
                    if((iretn = serial->get_slip(incomingPacket.packetized)) <= 0)
                    {
                        if(iretn < 0)
                        {
                            channelAgent->debug_log.Printf("Error in getting incoming SLIP packet. iretn=%d\n", iretn);
                        }
                        return;
                    }

                    if((iretn = incomingPacket.RawUnPacketize()) < 0)
                    {
                        channelAgent->debug_log.Printf("Failed to un-packetize incoming UART serial packet. iretn=%d\n", iretn);
                        return;
                    }
                      
                    switch (incomingPacket.header.type)
                    {
                        case PacketComm::TypeId::CommandCameraCapture:
                        case PacketComm::TypeId::CommandObcHalt:
                            iretn = channelAgent->channel_push("PAYLOAD", incomingPacket);
                            if(iretn < 0)
                            {
                                channelAgent->debug_log.Printf("Failed to forward incoming packet to payload channel. iretn=%d\n", iretn);
                                return;
                            }
                            break;
                        default:
                            iretn = channelAgent->channel_push(0, incomingPacket);
                            if(iretn < 0)
                            {
                                channelAgent->debug_log.Printf("Failed to forward incoming packet to main channel. iretn=%d\n", iretn);
                                return;
                            }
                            break;
                    }
                }

                return;
            }

            /**
             * @brief Helper function to send a packet to the Teensy over UART
             * serial.
             */
            void TeensyChannel::sendToTeensySerial()
            {
                int32_t iretn = channelAgent->channel_pull(channelNumber, outgoingPacket);
                
                if (iretn < 0)
                {
                    channelAgent->debug_log.Printf("Error in checking Teensy channel for outgoing packet. iretn=%d\n", iretn);
                    return;
                }
                
                if(!outgoingPacket.RawPacketize())
                {
                    channelAgent->debug_log.Printf("Failed to SLIP packetize outgoing packet.");
                    return;
                }

                if((iretn = serial->put_slip(outgoingPacket.packetized)) <= 0)
                {
                    if(iretn < 0)
                    {
                        channelAgent->debug_log.Printf("Error in sending outgoing SLIP packet. iretn=%d\n", iretn);
                    }
                    return;
                }
                
                return;
            }

            /**
             * @brief Helper function to receive a packet from the Teensy over 
             * I2C.
             * 
             * @todo If I2C will be used, complete this function. If it is not 
             * to be used, delete this function.
             */
            void TeensyChannel::receiveFromTeensyI2C()
            {
                std::string msg;

                int32_t iretn = i2c->receive(msg, 50);
                if (iretn <= 0 || msg.length() < sizeof(PacketComm::header) + sizeof(PacketComm::crc))
                {
                    if (iretn < 0)
                    {
                        channelAgent->debug_log.Printf("Failed to receive incoming I2C packet. iretn=%d\n", iretn);
                    }
                    else if (msg.length() < sizeof(PacketComm::header) + sizeof(PacketComm::crc))
                    {
                        channelAgent->debug_log.Printf("Received incomplete incoming I2C packet. msg.length()=%d\n", msg.length());
                    }
                    return;
                }

                incomingPacket.wrapped.resize(0);
                uint8_t data_len = msg[0];
                for (size_t i = 0; i < sizeof(PacketComm::header) + data_len + sizeof(PacketComm::crc); i++)
                {
                    incomingPacket.wrapped.push_back(msg[i]);
                }

                if ((iretn = incomingPacket.Unwrap()) < 0)
                {
                    channelAgent->debug_log.Printf("Failed to unwrap incoming I2C packet. iretn=%d\n", iretn);
                    return;
                }

                if((iretn = channelAgent->channel_push(0, incomingPacket)) < 0)
                {
                    channelAgent->debug_log.Printf("Failed to forward incoming packet to main channel. iretn=%d\n", iretn);
                    return;
                }

                return;
            }

            /**
             * @brief Helper function to send a packet to the Teensy over I2C.
             * 
             * @todo If I2C will be used, complete this function. If it is not 
             * to be used, delete this function.
             */
            void TeensyChannel::sendToTeensyI2C() {}
        }
    }
}

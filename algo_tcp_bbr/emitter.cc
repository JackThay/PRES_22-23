#include "emitter.h"

Define_Module(Emitter);

void Emitter::initialize()
{
    m = par("m");
    timeout = par("timeout");
    lastAckTime = 0;
    lastSendTime = 0;
    cycleIndex = 0;
    pg = 1;
    spacing = 0;
    Packet *message = new Packet("CON");
    message->setBitLength(1000);
    send(message, "out");
}

void Emitter::handleMessage(cMessage *msg)
{
    double currentTime = simTime().dbl();

    if (strcmp("ACK",msg->getName()) == 0)
    {
        double measurementSample = currentTime - lastAckTime;
        lastAckTime = currentTime;
        spacings.push_back(measurementSample);

        if (spacings.size() > m)
        {
            spacings.pop_front();
        }
    }

    double nextSendTime = std::max(lastSendTime + spacing, currentTime);
    Packet *nextPacket = new Packet("CON");
    scheduleAt(nextSendTime, nextPacket);
    lastSendTime = nextSendTime;

    cycleIndex = (cycleIndex + 1) % 8;

    if (cycleIndex == 0)
    {
        pg = "ACK" ? 0.75 : 1.25;
    }
    else if (cycleIndex == 1)
    {
        pg = 0.75;
    }
    else
    {
        pg = 1;
    }

    spacing = *std::min_element(spacings.begin(), spacings.end()) / pg;

    if (strcmp("CON",msg->getName()) == 0)
    {
        Packet *nextPacket = new Packet("CON");
        nextPacket->setBitLength(1000);
        send(nextPacket, "out");
    }
}

#include "emitter.h"

Define_Module(Emitter);

void Emitter::initialize()
{
    incrementalWeight = par("incrementalWeight");
    decrementalWeight = par("decrementalWeight");
    fastDecrementalWeight = par("fastDecrementalWeight");
    initialSpacing = par("initialSpacing");
    rto = par("rto");

    // Schedule the first packet transmission
    scheduleAt(simTime() + initialSpacing, new cMessage("sendPacket"));
}

void Emitter::handleMessage(cMessage *msg)
{
    if (strcmp(msg->getName(), "sendPacket") == 0)
    {
        // Send the packet
        Packet *nextPacket = new Packet("CON");
        nextPacket->setBitLength(1000);
        send(nextPacket, "out");

        // Schedule the next packet transmission
        double spacing = initialSpacing;
        double lossSpacing = 0;

        // Wait for CoAP ACK or RTO expiration
        if (strcmp(msg->getName(), "ACK") == 0)
        {
            if (spacing >= lossSpacing)
            {
                spacing -= decrementalWeight * spacing;
            }
            else
            {
                spacing -= fastDecrementalWeight * spacing;
            }
        }
        else if(strcmp(msg->getName(), "rto_expired") == 0)
        {
            EV << "Timeout expired, CON packet is probably lost.\n";
            lossSpacing = spacing;
            spacing = incrementalWeight * spacing;
            scheduleAt(simTime()+ rto, new cMessage("rto_expired"));
        }

        spacing = std::max(spacing, simTime().dbl() - nextPacket->getSendingTime().dbl());
        scheduleAt(simTime() + spacing, new cMessage("sendPacket"));
        scheduleAt(simTime()+ rto, new cMessage("rto_expired"));
    }
    else
    {
        // Handle other messages
        // ...

    }
}

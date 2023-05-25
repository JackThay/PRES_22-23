#include "emitter_IDC.h"

Define_Module(Emitter_IDC);

void Emitter_IDC::initialize()
{
    packet_size = par("packet_size");
    clientID = rand(); // used to ID client if there is several
    currentID = rand(); // Initializing current ID with a random number
    incrementalWeight = 1.5;
    decrementalWeight = 0.01;
    fastDecrementalWeight = 0.5;
    initialSpacing = 0.5;
    rto = 1.0;
    EV << "Emitter using IDC-CoAP, number " << clientID <<" initialized" << std::endl; // output message

    // Schedule the first packet transmission
    scheduleAt(simTime() + initialSpacing, new cMessage("sendPacket"));
}

void Emitter_IDC::handleMessage(cMessage *msg)
{
    srand(time(0)); // Use current time as seed for the random number generator
    if (strcmp(msg->getName(), "sendPacket") == 0)
    {
        // Send the packet
        Packet *nextPacket = new Packet("CON");
        nextPacket->setBitLength(par("packet_size"));
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

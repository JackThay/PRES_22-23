#include "receiver.h"

Define_Module(Receiver); // Register the module with OMNeT++

// Constructor
Receiver::Receiver()
{
    finTraitement = NULL; // Initialize the flag pointer to null
}

// Destructor
Receiver::~Receiver()
{
    cancelAndDelete(finTraitement); // Cancel and delete the flag when the simulation ends
}

void Receiver::initialize()
{
    finTraitement = new cMessage("finTraitement"); // Create a new cMessage object to use as the flag
}

void Receiver::handleMessage(cMessage *msg)
{
    if (msg == finTraitement)
    {
        cPacket *ackPacket = new cPacket("ACK"); // creating a message ACK from the received CON
        ackPacket->setBitLength(320); // changing size of packet to typical size of an ACK packet
        send(ackPacket, SendOptions().transmissionId(useID), "out"); // setting transmissionId to ID copied from conPacket and send the message out
        EV << "Sending ACK packet, ID: " << ackPacket->getTransmissionId() << ", size : " << ackPacket->getBitLength() << "bit" << std::endl; // output a log message
    }
    else
    {
        if(!finTraitement -> isScheduled()) // If processing is not already in progress
        {
            cPacket *conPacket = check_and_cast<cPacket *> (msg); // receiving a packet
            if (strcmp("CON",msg->getName()) == 0) // checking if the incoming message is an CON
            {
                EV << "Receiving CON packet, ID: " << conPacket->getTransmissionId() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
                useID = conPacket->getTransmissionId();
                bubble("CON received!"); // displaying bubble
                delta = randomDouble(0.1,0.5); // Get the processing delay from the module parameter
                EV << "Current processing delay = " << delta << std::endl; // Print the processing delay to the simulation log
                scheduleAt(simTime() + delta, finTraitement); // Schedule the processing to be completed after the specified delay
                delete msg; // Delete the incoming message since it has been received and processed
            }
            else // obviously, if it's not a CON, this should not be happening!!
            {
                bubble("WTF?!");
                EV << "What the fuck was that?! That's not a CON!" << std::endl; // output a log message
                delete msg; // Delete the incoming message if processing is already in progress
            }

        }
        else // server is busy, cannot process incoming message
        {
            delete msg; // Delete the incoming message if processing is already in progress
        }
    }
}

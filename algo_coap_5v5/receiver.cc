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
        Packet *ackPacket = new Packet("ACK"); // creating a message ACK from the received CON
        ackPacket->setNid(useID); // re-using current ID number
        send (ackPacket,"out"); // Send the ACK message
        EV << "Sending ACK packet, ID: " << ackPacket->getNid() << std::endl; // output a log message
    }
    else
    {
        if(!finTraitement -> isScheduled()) // If processing is not already in progress
        {
            Packet *conPacket = check_and_cast<Packet *> (msg); // receiving CON packet
            useID = conPacket->getNid(); // registering ID from CON packet into use
            EV << "Receiving CON packet, ID: " << conPacket->getNid() << std::endl; // output a log message
            bubble("CON received!"); // displaying bubble
            delta = par("delai"); // Get the processing delay from the module parameter
            EV << "Current processing delay = " << delta << std::endl; // Print the processing delay to the simulation log
            scheduleAt(simTime() + delta, finTraitement); // Schedule the processing to be completed after the specified delay
            delete msg; // Delete the incoming message since it has been received and processed
        }
        else
        {
            delete msg; // Delete the incoming message if processing is already in progress
        }
    }
}

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
        // creating a message ACK from the received CON
        Packet *ackPacket = new Packet("ACK");
        // re-using current ID number
        ackPacket->setNid(useID);
        // re-using current sequence number
        ackPacket->setNseq(7777);
        // output a log message
        send (ackPacket,"out"); // Send the ACK message
        // output a log message
        EV << "Sending ACK packet, ID: " << ackPacket->getNid() << " SeqN: " << ackPacket->getNseq() << std::endl;
    }
    else
    {
        // If processing is not already in progress
        if(!finTraitement -> isScheduled())
        {
            // receiving CON packet
            Packet *conPacket = check_and_cast<Packet *> (msg);
            // registering ID from CON packet into use
            useID = conPacket->getNid();
            // registering sequence from CON packet into use
            useSeq = conPacket->getNseq();
            // output a log message
            EV << "Receiving CON packet, ID: " << conPacket->getNid() << " SeqN: " << conPacket->getNseq() << std::endl;
            // displaying bubble
            bubble("CON received!");
            // Get the processing delay from the module parameter
            delta = par("delai");
            // Print the processing delay to the simulation log
            EV << "Current processing delay = " << delta << std::endl;
            // Schedule the processing to be completed after the specified delay
            scheduleAt(simTime() + delta, finTraitement);
            // Delete the incoming message since it has been received and processed
            delete msg;
        }
        else
        {
            // Delete the incoming message if processing is already in progress
            delete msg;
        }
    }
}

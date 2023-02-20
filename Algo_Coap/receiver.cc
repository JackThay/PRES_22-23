#include "receiver.h"

Define_Module(Receiver); // Register the module with OMNeT++

Receiver::Receiver(){
    finTraitement = NULL; // Initialize the flag pointer to null
}

Receiver::~Receiver(){
    cancelAndDelete(finTraitement); // Cancel and delete the flag when the simulation ends
}

void Receiver::initialize()
{
    finTraitement = new cMessage("finTraitement"); // Create a new cMessage object to use as the flag
}

void Receiver::handleMessage(cMessage *msg)
{
    if (msg == finTraitement){
        cMessage *ack = new cMessage("ack"); // Create an ack message
        send (ack,"out"); // Send the ack message
    }else{
        if(!finTraitement -> isScheduled()){ // If processing is not already in progress
            EV << "Receive a message.\n"; // Print a message to the simulation log
            delta = par("delai"); // Get the processing delay from the module parameter
            EV << "Current processing delay = " << delta << std::endl; // Print the processing delay to the simulation log
            scheduleAt(simTime() + delta, finTraitement); // Schedule the processing to be completed after the specified delay
            delete msg; // Delete the incoming message since it has been received and processed
        }else{
            delete msg; // Delete the incoming message if processing is already in progress
        }
    }
}

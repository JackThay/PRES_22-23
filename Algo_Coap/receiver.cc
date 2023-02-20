#include "receiver.h"

Define_Module(Receiver);

Receiver::Receiver(){
    finTraitement = NULL;
}

Receiver::~Receiver(){
    cancelAndDelete(finTraitement);
}

void Receiver::initialize()
{
    finTraitement = new cMessage("finTraitement");
}

void Receiver::handleMessage(cMessage *msg)
{
    if (msg == finTraitement){
     cMessage *ack = new cMessage("ack");
     send (ack,"out");
     } else {
     EV << "Receive a message.\n";
     delta = par("delai");
     EV << "Current processing delay = " << delta << std::endl;
     scheduleAt(simTime() + delta, finTraitement);
     delete msg;
     }
}

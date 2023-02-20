#include "emitter.h"

Define_Module(Emitter);

Emitter::Emitter(){
    timeoutEvent = NULL;
}

Emitter::~Emitter(){
    cancelAndDelete(timeoutEvent);
}

void Emitter::initialize()
{
    timeout = par("timeout");
    timeoutEvent = new cMessage ("timeoutEvent");
    cMessage *data = new cMessage("data");
    EV << "Send the first packet. \n";
    scheduleAt(simTime(),data);
    t1 = simTime().dbl();
    scheduleAt(simTime() + timeout, timeoutEvent);
}

void Emitter::handleMessage(cMessage *msg)
{
    if(msg == timeoutEvent){
        EV << "Timeout expired, packet lost.\n";
        cMessage *data = new cMessage("data");
        EV << "Re-send data packet.\n";
        send(data, "out");
        scheduleAt(simTime()+ timeout, timeoutEvent);
    }else{
        EV << "Receive " << msg->getName() << std::endl;
        cancelEvent(timeoutEvent);
        t2 = simTime().dbl();
        rtt = t2 - t1;
        delete msg;
        EV << "Send a new data packet.\n";
        cMessage *data = new cMessage("data");
        send(data,"out");
        t1 = simTime().dbl();
        scheduleAt(simTime()+ timeout, timeoutEvent);
    }
}

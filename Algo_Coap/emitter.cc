#include "emitter.h"

// register the module with OMNeT++
Define_Module(Emitter);

// constructor
Emitter::Emitter(){
    timeoutEvent = NULL;
}

// destructor
Emitter::~Emitter(){
    cancelAndDelete(timeoutEvent);
}

// called at the beginning of the simulation
void Emitter::initialize()
{
    // get the timeout value from the NED file
    timeout = par("timeout");
    // create a message to send
    timeoutEvent = new cMessage ("timeoutEvent");
    // create a message to send
    cMessage *data = new cMessage("data");
    // output a log message
    EV << "Send the first packet. \n";
    // schedule the message to be sent immediately
    scheduleAt(simTime(),data);
    // record the time the packet was sent
    t1 = simTime().dbl();
    // schedule a timeout event
    scheduleAt(simTime() + timeout, timeoutEvent);
}

void Emitter::handleMessage(cMessage *msg)
{
    // if the message is the timeout event
    if(msg == timeoutEvent){
        // output a log message
        EV << "Timeout expired, packet lost.\n";
        // create a new message to send
        cMessage *data = new cMessage("data");
        // output a log message
        EV << "Re-send data packet.\n";
        // send the message out
        send(data, "out");
        // schedule a new timeout event
        scheduleAt(simTime()+ timeout, timeoutEvent);
    }
    // if the message is not the timeout event
    else{
        // output a log message
        EV << "Receive " << msg->getName() << std::endl;
        // cancel the timeout event
        cancelEvent(timeoutEvent);
        // record the time the packet was received
        t2 = simTime().dbl();
        // calculate the round trip time
        rtt = t2 - t1;
        // delete the message
        delete msg;
        // output a log message
        EV << "Send a new data packet.\n";
        // create a new message to send
        cMessage *data = new cMessage("data");
        // send the message out
        send(data,"out");
        // record the time the packet was sent
        t1 = simTime().dbl();
        // schedule a new timeout event
        scheduleAt(simTime()+ timeout, timeoutEvent);
    }
}

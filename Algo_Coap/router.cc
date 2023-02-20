#include "router.h"

Define_Module(Router);

void Router::initialize()
{
    dropping_probability = par("dropping_probability");
}

void Router::handleMessage(cMessage *msg)
{
    if (uniform(0,1) < dropping_probability){
    EV << "Drop packet.\n";
    bubble("packet lost");
    delete msg;
    }else{
        if (strcmp("data",msg->getName()) == 0){
            EV << "Forward data packet...\n";
            send (msg,"out_e");
        }else{
            EV << "Forward acknowledgment...\n";
            send(msg,"out_r");
        }
    }
}

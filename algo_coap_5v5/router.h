#ifndef __ALGO_COAP_5v5_ROUTER_H_ // include guard
#define __ALGO_COAP_5v5_ROUTER_H_

#include <omnetpp.h>
#include <stdio.h>
#include <string.h>
//#include <cdataratechannel.h>
#include "shared_function.h"
#include "Packet_m.h"


using namespace omnetpp;

class Router : public cSimpleModule // define a class named Router, which inherits from cSimpleModule
{
    private:
        double dropping_probability; // probability of dropping packets
        int queue_size; // size of the packet queue
        double timeBeforeSend; // Time to wait before sending next packet, should be packet size/bandwidth
        cQueue queue; // packet queue
        cMessage *nextInQueue;
    public:
        Router();                 // Constructor
        virtual ~Router();        // Destructor
    protected:
        virtual void initialize() override; // override the initialize method of cSimpleModule
        virtual void handleMessage(cMessage *msg) override; // override the handleMessage method of cSimpleModule
};

#endif

#ifndef __ALGO_COAP_5v5_ROUTER_H_ // include guard
#define __ALGO_COAP_5v5_ROUTER_H_

#include <omnetpp.h>
#include <stdio.h>
#include <string.h>
#include "shared_function.h"

using namespace omnetpp;

class Router : public cSimpleModule // define a class named Router, which inherits from cSimpleModule
{
    private:
        double dropping_probability; // probability of dropping packets
        int queue_size; // size of the packet queue
        int memory; // memorizing transmissionID from packet
        cQueue queue; // packet queue
        cMessage *finTraitement;
        cMessage *nextInQueue;
    public:
        Router();                 // Constructor
        virtual ~Router();        // Destructor
    protected:
        virtual void initialize() override; // override the initialize method of cSimpleModule
        virtual void handleMessage(cMessage *msg) override; // override the handleMessage method of cSimpleModule
};

#endif

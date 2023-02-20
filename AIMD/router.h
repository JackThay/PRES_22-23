#ifndef __AIMD_ROUTER_H_ // include guard
#define __AIMD_ROUTER_H_

#include <omnetpp.h>
#include <stdio.h>
#include <string.h>
#include <queue>

using namespace omnetpp;

class Router : public cSimpleModule // define a class named Router, which inherits from cSimpleModule
{
    private:
        double dropping_probability; // probability of dropping packets
        int queue_size; // size of the packet queue
        std::queue<cMessage *> queue; // packet queue
    protected:
        virtual void initialize() override; // override the initialize method of cSimpleModule
        virtual void handleMessage(cMessage *msg) override; // override the handleMessage method of cSimpleModule
};

#endif


#ifndef __ALGO_COAP_EMITTER_H_  // header guards, to avoid multiple inclusion of this header
#define __ALGO_COAP_EMITTER_H_

#include <omnetpp.h>  // include the OMNeT++ header file
#include <stdio.h>
#include <string.h> // needed for string
#include <cstdlib> // needed to generate random number
#include <iostream>
#include <time.h>
#include "Packet_m.h"

using namespace omnetpp;  // use the OMNeT++ namespace

class Emitter : public cSimpleModule  // define a new class called "Emitter" that inherits from the OMNeT++ class "cSimpleModule"
{
    private:
        cMessage *timeoutEvent;  // pointer to a message object that will be used to trigger a timeout event
        cMessage *initEvent;
        double timeout;  // the length of the timeout interval in seconds
        double rtt, t1, t2;  // variables used to compute the round-trip time of the network
        double ACK_TIMEOUT; // value for an ACK timeout
        double ACK_RANDOM_FACTOR; // value for random factor
        int MAX_RETRANSMIT; // value for max retransmission
        double initTimeout; // parameter for storing timeout
        int currentID; // value for message ID
        int retransmissionCounter; // counter for retransmission

    public:
        Emitter();  // default constructor for the Emitter class
        virtual ~Emitter();  // virtual destructor for the Emitter class

    protected:
        virtual void initialize() override;  // override the initialize() method of cSimpleModule
        virtual void handleMessage(cMessage *msg) override;  // override the handleMessage() method of cSimpleModule
};

#endif  // end of header guards

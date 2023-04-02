// Include guard to prevent multiple inclusions of this header file
#ifndef __ALGO_COAP_RECEIVER_H_
#define __ALGO_COAP_RECEIVER_H_

// Include the OMNeT++ library
#include <omnetpp.h>
#include "Packet_m.h"

// Use the OMNeT++ namespace
using namespace omnetpp;

// Define the Receiver class, which is a subclass of the cSimpleModule class
class Receiver : public cSimpleModule
{
    private:
        cMessage *finTraitement;    // Pointer to a message used for scheduling the end of processing
        simtime_t delta;            // The delay for processing a message
        int useID; // memorizing ID number from incoming CON packet

    public:
        Receiver();                 // Constructor
        virtual ~Receiver();        // Destructor

    protected:
        virtual void initialize() override;    // Called during module initialization
        virtual void handleMessage(cMessage *msg) override;    // Called when a message is received
};

// End of the include guard
#endif

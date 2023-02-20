#ifndef __AIMD_EMITTER_H_  // header guards, to avoid multiple inclusion of this header
#define __AIMD_EMITTER_H_

#include <omnetpp.h>  // include the OMNeT++ header file

using namespace omnetpp;  // use the OMNeT++ namespace

class Emitter : public cSimpleModule
{
  private:
    int cwnd; // congestion window size
    int ssthresh; // slow start threshold
    int numPacketsSent; // number of packets sent
    int numAcksReceived; // number of ACKs received
    int maxPacketsToSend; // maximum number of packets to send
    simtime_t timeout; // retransmission timeout

  public:
    Emitter(); // default constructor for the Emitter class
    virtual ~Emitter(); // virtual destructor for the Emitter class

  protected:
    virtual void initialize() override; // override the initialize() method of cSimpleModule
    virtual void handleMessage(cMessage *msg) override; // override the handleMessage() method of cSimpleModule
    virtual void sendPacket(); // method to send packet
};


#endif  // end of header guards

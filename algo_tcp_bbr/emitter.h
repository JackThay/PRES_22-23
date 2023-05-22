#include <omnetpp.h>
#include <deque>
#include <algorithm>
#include "shared_function.h"
#include "Packet_m.h"

using namespace omnetpp;

class Emitter : public cSimpleModule
{
  private:
    cMessage *timeoutEvent;  // pointer to a message object that will be used to trigger a timeout event
    cMessage *initEvent;
    cMessage *nextPacket;
    cMessage *getUploadSpeed;
    int clientID; // used to ID client if there's several
    int currentID; // value for message ID
    int packet_size; // packet size for the experiment
    int packet_sent; // number of packet sent in a second
    double rtt, t1, t2;  // variables used to compute the round-trip time of the network
    int m;
    double timeout;
    std::deque<double> spacings;
    std::deque<int> expectedAck;
    double lastAckTime;
    double lastSendTime;
    int cycleIndex;
    double pg;
    double spacing;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

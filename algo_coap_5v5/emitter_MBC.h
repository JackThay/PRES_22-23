#include <omnetpp.h>
#include <deque>
#include <algorithm>
#include "shared_function.h"
#include "Packet_m.h"

using namespace omnetpp;

class Emitter_MBC : public cSimpleModule
{
  private:
    cMessage *timeoutEvent;  // pointer to a message object that will be used to trigger a timeout event
    cMessage *nextPacket;
    cMessage *getUploadSpeed;
    int clientID; // used to ID client if there's several
    int currentID; // value for message ID
    int packet_size; // packet size for the experiment
    int packet_sent; // number of packet sent in a second
    double ACK_TIMEOUT; // value for an ACK timeout
    double ACK_RANDOM_FACTOR; // value for random factor
    double initTimeout; // parameter for storing timeout
    bool retransmission; // boolean to check if next transmission will be a retransmission or not
    int m;
    double timeout;
    std::deque<double> spacings;
    double lastAckTime;
    double lastSendTime;
    int cycleIndex;
    double pg;
    double spacing;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

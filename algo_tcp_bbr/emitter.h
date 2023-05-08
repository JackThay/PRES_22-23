#include <omnetpp.h>
#include <deque>
#include <algorithm>
#include "shared_function.h"
#include "Packet_m.h"

using namespace omnetpp;

class Emitter : public cSimpleModule
{
  private:
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

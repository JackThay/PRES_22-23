#include <omnetpp.h>
#include "shared_function.h"
#include "Packet_m.h"

using namespace omnetpp;

class Emitter : public cSimpleModule
{
  private:
    double incrementalWeight;
    double decrementalWeight;
    double fastDecrementalWeight;
    double initialSpacing;
    double rto;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

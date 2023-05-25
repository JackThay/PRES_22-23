#include <omnetpp.h>
#include "shared_function.h"
#include "Packet_m.h"

using namespace omnetpp;

class Emitter_IDC : public cSimpleModule
{
  private:
    int packet_size; // packet size for the experiment
    int clientID; // used to ID client if there's several
    int currentID; // value for message ID
    double incrementalWeight;
    double decrementalWeight;
    double fastDecrementalWeight;
    double initialSpacing;
    double rto;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

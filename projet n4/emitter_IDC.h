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
    int packet_sent; // number of packet sent
    int ack_received; // number of ack received
    cMessage *initEvent;
    cMessage *sendPacket;
    cMessage *getUploadSpeed;
    bool ACK;
    bool FirstCON;
    double spacing;
    double last_send_time;
    double current_last;
    double rtt, t1, t2;
    double loss_spacing;
    double incrementalWeight;
    double decrementalWeight;
    double fastDecrementalWeight;
    double initialSpacing;
    double rto;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

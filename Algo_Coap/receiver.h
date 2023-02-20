#ifndef __ALGO_COAP_RECEIVER_H_
#define __ALGO_COAP_RECEIVER_H_

#include <omnetpp.h>

using namespace omnetpp;


class Receiver : public cSimpleModule
{
    private:
        cMessage *finTraitement;
        simtime_t delta;
    public:
        Receiver();
        virtual ~Receiver();
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

#endif

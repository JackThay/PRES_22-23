#ifndef __ALGO_COAP_EMITTER_H_
#define __ALGO_COAP_EMITTER_H_

#include <omnetpp.h>

using namespace omnetpp;

class Emitter : public cSimpleModule
{
    private:
        cMessage *timeoutEvent;
        double timeout;
        double rtt, t1, t2;

    public:
        Emitter();
        virtual ~Emitter();

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

#endif

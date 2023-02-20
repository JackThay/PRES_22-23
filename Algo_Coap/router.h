#ifndef __ALGO_COAP_ROUTER_H_
#define __ALGO_COAP_ROUTER_H_

#include <omnetpp.h>

using namespace omnetpp;


class Router : public cSimpleModule
{
private:
    double dropping_probability;
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif

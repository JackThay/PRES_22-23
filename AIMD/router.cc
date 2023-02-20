#include "router.h"

Define_Module(Router);

void Router::initialize()
{
    // read parameters from NED file
    dropping_probability = par("dropping_probability");
    queue_size = par("queue_size");
    std::queue<cMessage*> queue;  // initialize the queue
}

void Router::handleMessage(cMessage *msg)
{
    // randomly drop packet based on configured dropping probability
    if (uniform(0,1) < dropping_probability){
        EV << "Drop packet.\n";
        bubble("packet lost");
        delete msg;
    } else {
        // forward data packet or acknowledgment depending on packet name
        if (strcmp("data",msg->getName()) == 0){
            EV << "Forward data packet...\n";
            if (queue.size() < queue_size) {
                queue.push(msg);
            } else {
                EV << "Queue full. Drop packet.\n";
                bubble("packet dropped by queue");
                delete msg;
            }
        } else {
            EV << "Forward acknowledgment...\n";
            send(msg,"out_r");
        }
    }

    // if there are packets in the queue, forward the first one
    if (!queue.empty()) {
        cMessage *queuedMsg = (cMessage*)queue.front();
        EV << "Forwarding queued packet...\n";
        send(queuedMsg, "out_e");
        queue.pop();
    }
}

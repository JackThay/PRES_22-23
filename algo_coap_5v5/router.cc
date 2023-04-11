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
        if (strcmp("CON",msg->getName()) == 0){
            EV << "Forward data packet...\n";
            if (queue.size() < queue_size) {
                queue.push(msg);
            } else {
                EV << "Queue full. Dropping packet.\n";
                bubble("Packet dropped by queue");
                delete msg;
            }
        } else {
            EV << "Forward acknowledgment...\n";
            cGate *arrivalGate = msg -> getArrivalGate();
            int arrivalGateIndex = arrivalGate -> getIndex();
            EV << "Packet arrived from gate " + std::to_string(arrivalGateIndex) + "\n";
            send(msg, "out_rPort", arrivalGateIndex);
        }
    }

    // if there are packets in the queue, forward the first one
    if (!queue.empty()) {
        cMessage *queuedMsg = (cMessage*)queue.front();
        EV << "Forwarding queued packet...\n";
        cGate *arrivalGate = queuedMsg -> getArrivalGate();
        int arrivalGateIndex = arrivalGate -> getIndex();
        EV << "Packet arrived from gate " + std::to_string(arrivalGateIndex) + "\n";
        send(queuedMsg, "out_ePort", arrivalGateIndex);
        queue.pop();
    }
}

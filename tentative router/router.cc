#include "router.h"

Define_Module(Router);

// Constructor
Router::Router()
{
    finTraitement = NULL; // Initialize the flag pointer to null
}

// Destructor
Router::~Router()
{
    cancelAndDelete(finTraitement); // Cancel and delete the flag when the simulation ends
}


void Router::initialize()
{
    // read parameters from NED file
    dropping_probability = par("dropping_probability");
    queue_size = par("queue_size");
    memory = 0;
    cQueue queue;  // initialize the queue
    finTraitement = new cMessage("finTraitement");
    nextInQueue = new cMessage("nextInQueue");
}

void Router::handleMessage(cMessage *msg)
{
    if (msg == nextInQueue)
    {
        if (queue.isEmpty())
        {
            delete msg;
        }
        else
        {
            cMessage *queuedMsg = (cMessage*)queue.front();
            EV << "Forwarding queued packet...\n";
            cGate *arrivalGate = queuedMsg -> getArrivalGate();
            int arrivalGateIndex = arrivalGate -> getIndex();
            EV << "Packet arrived from gate " + std::to_string(arrivalGateIndex) + "\n";
            queue.pop();
            send(queuedMsg, "out_ePort", arrivalGateIndex);
            scheduleAt(simTime(), nextInQueue);
        }
    }
    else
    {
        if(!nextInQueue -> isScheduled()) // If processing is not already in progress
        {
            if (uniform(0,1) < dropping_probability)
            {
                EV << "Drop packet.\n";
                bubble("packet lost");
                delete msg;
            }
            else
            {
                if (strcmp("CON",msg->getName()) == 0) // forward data packet or acknowledgment depending on packet name
                {
                    if (queue.isEmpty())
                    {
                        EV << "Forward acknowledgment...\n";
                        cGate *arrivalGate = msg -> getArrivalGate();
                        int arrivalGateIndex = arrivalGate -> getIndex();
                        EV << "Packet arrived from gate " + std::to_string(arrivalGateIndex) + "\n";
                        send(msg, "out_ePort", arrivalGateIndex);
                        scheduleAt(simTime() + 0.1, nextInQueue);
                    }
                    else if(!queue.isEmpty())
                    {
                        if (queue.getLength() < queue_size)
                        {
                            queue.insert(msg);
                            EV << "Queue size:  " << queue.getLength() << std::endl;
                        }
                        else
                        {
                            EV << "Queue full. Dropping packet.\n";
                            bubble("Packet dropped by queue");
                            delete msg;
                        }

                    }

                }
                else
                {
                    EV << "Forward acknowledgment...\n";
                    cGate *arrivalGate = msg -> getArrivalGate();
                    int arrivalGateIndex = arrivalGate -> getIndex();
                    EV << "Packet arrived from gate " + std::to_string(arrivalGateIndex) + "\n";
                    send(msg, "out_rPort", arrivalGateIndex);
                }
            }
        }
    }
    /*
    // randomly drop packet based on configured dropping probability
    if (uniform(0,1) < dropping_probability){
        EV << "Drop packet.\n";
        bubble("packet lost");
        delete msg;
    } else {
        cPacket *transitPacket = check_and_cast<cPacket *>(msg); // receiving a packet
        // forward data packet or acknowledgment depending on packet name
        if (strcmp("CON",msg->getName()) == 0){
            EV << "Forward data packet...\n";
            if (queue.getLength() < queue_size) {
                queue.insert(msg);
            } else {
                EV << "Queue full. Dropping packet.\n";
                bubble("Packet dropped by queue");
                delete msg;
            }
        } else {
            EV << "Forward acknowledgment...\n";
            memory = transitPacket->getTransmissionId();
            cGate *arrivalGate = msg -> getArrivalGate();
            int arrivalGateIndex = arrivalGate -> getIndex();
            EV << "Packet arrived from gate " + std::to_string(arrivalGateIndex) + "\n";
            send(msg, SendOptions().transmissionId(memory), "out_rPort", arrivalGateIndex);
        }
    }

    // if there are packets in the queue, forward the first one
    if (!queue.isEmpty()) {
            //if(!finTraitement -> isScheduled()){
                //scheduleAt(simTime() + rand(), finTraitement);
                cMessage *queuedMsg = (cMessage*)queue.front();
                cPacket *transitPacket = check_and_cast<cPacket *>(queuedMsg); // receiving a packet
                memory = transitPacket->getTransmissionId();
                EV << "Forwarding queued packet...\n";
                cGate *arrivalGate = queuedMsg -> getArrivalGate();
                int arrivalGateIndex = arrivalGate -> getIndex();
                EV << "Packet arrived from gate " + std::to_string(arrivalGateIndex) + "\n";
                queue.pop();
                send(queuedMsg, SendOptions().transmissionId(memory), "out_ePort", arrivalGateIndex);
            //}
        }*/
}

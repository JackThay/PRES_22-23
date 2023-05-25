#include "router.h"

Define_Module(Router);

// Constructor
Router::Router()
{
    nextInQueue = NULL; // Initialize the flag pointer to null
}

// Destructor
Router::~Router()
{
    cancelAndDelete(nextInQueue); // Cancel and delete the flag when the simulation ends
}


void Router::initialize()
{
    // read parameters from NED file
    dropping_probability = par("dropping_probability");
    queue_size = par("queue_size");
    timeBeforeSend = par("timeBeforeSend");
    cQueue queue;  // initialize the queue
    nextInQueue = new cMessage("nextInQueue");
}

void Router::handleMessage(cMessage *msg)
{
    if (msg == nextInQueue) //
    {
        if (queue.isEmpty())
        {
            EV << "Queue size: empty" << std::endl;
        }
        else
        {
            cMessage *queuedMsg = (cMessage*)queue.front();
            Packet *queuedPacket = check_and_cast<Packet *>(queuedMsg);
            EV << "Forwarding next " << queuedMsg->getName() << " packet ID: " << queuedPacket->getNid() << ", size : " << queuedPacket->getBitLength() << "bit" << std::endl; // output a log message
            cGate *arrivalGate = queuedMsg -> getArrivalGate();
            int arrivalGateIndex = arrivalGate -> getIndex();
            EV << "Packet arrived from gate " + std::to_string(arrivalGateIndex) + "\n";
            queue.pop();
            send(queuedMsg, "out_ePort", arrivalGateIndex);
            scheduleAt(simTime() + par("timeBeforeSend"), nextInQueue);
        }
    }
    else // if message is not a self message
    {
        Packet *transitPacket = check_and_cast<Packet *>(msg); // casting my packet
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
                        EV << "Forwarding " << msg->getName() << " packet ID: " << transitPacket->getNid() << ", size : " << transitPacket->getBitLength() << "bit" << std::endl; // output a log message
                        cGate *arrivalGate = msg -> getArrivalGate();
                        int arrivalGateIndex = arrivalGate -> getIndex();
                        //int arrivalGateDatarate = arrivalGate -> cDatarateChannel::getDatarate();
                        EV << "Packet arrived from gate " + std::to_string(arrivalGateIndex) + "\n";
                        send(msg, "out_ePort", arrivalGateIndex);
                        //EV << "Datarate: " << arrivalGateDatarate << std::endl;
                        scheduleAt(simTime() + /*transitPacket->getBitLength()/arrivalGateDatarate*/ par("timeBeforeSend"), nextInQueue);
                    }
                    else if(!queue.isEmpty())
                    {
                        if (queue.getLength() < queue_size) // Queue still has room
                        {
                            queue.insert(msg);
                            EV << "Router busy, queue has room " << msg->getName() << " packet ID: " << transitPacket->getNid() << ", size : " << transitPacket->getBitLength() << "bit, will be queued" << std::endl; // output a log message
                            EV << "Queue size: " << queue.getLength() << std::endl;
                        }
                        else // Queue is full
                        {
                            EV << "Router busy, queue is full " << msg->getName() << " packet ID: " << transitPacket->getNid() << ", size : " << transitPacket->getBitLength() << "bit, will be dropped" << std::endl; // output a log message
                            bubble("Packet dropped by queue");
                            delete msg;
                        }

                    }

                }
                else // when the packet is not a CON
                {
                    EV << "Forwarding " << msg->getName() << " packet ID: " << transitPacket->getNid() << ", size : " << transitPacket->getBitLength() << "bit" << std::endl; // output a log message
                    cGate *arrivalGate = msg -> getArrivalGate();
                    int arrivalGateIndex = arrivalGate -> getIndex();
                    EV << "Packet arrived from gate " + std::to_string(arrivalGateIndex) + "\n";
                    send(msg, "out_rPort", arrivalGateIndex);
                }
            }
        }
        else // if nextInQueue is not scheduled
        {
            if (strcmp("CON",msg->getName()) == 0) // forward data packet or acknowledgment depending on packet name
            {
                if (queue.getLength() < queue_size)
                {
                    queue.insert(msg);
                    EV << "Router busy, queue has room " << msg->getName() << " packet ID: " << transitPacket->getNid() << ", size : " << transitPacket->getBitLength() << "bit, will be queued" << std::endl; // output a log message
                    EV << "Queue size: " << queue.getLength() << std::endl;
                }
                else
                {
                    EV << "Router busy, queue is full " << msg->getName() << " packet ID: " << transitPacket->getNid() << ", size : " << transitPacket->getBitLength() << "bit, will be dropped" << std::endl; // output a log message
                    bubble("Packet dropped by queue");
                    delete msg;
                }
            }
            else // when the packet is not a CON
            {
                EV << "Forwarding " << msg->getName() << " packet ID: " << transitPacket->getNid() << ", size : " << transitPacket->getBitLength() << "bit" << std::endl; // output a log message
                cGate *arrivalGate = msg -> getArrivalGate();
                int arrivalGateIndex = arrivalGate -> getIndex();
                EV << "Packet arrived from gate " + std::to_string(arrivalGateIndex) + "\n";
                send(msg, "out_rPort", arrivalGateIndex);
            }

        }
    }
}

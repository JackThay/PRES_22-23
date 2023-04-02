#include "emitter.h"

// register the module with OMNeT++
Define_Module(Emitter);

// Constructor
Emitter::Emitter()
{
    timeoutEvent = NULL;
}

// Destructor
Emitter::~Emitter(){
    cancelAndDelete(timeoutEvent);
}

// called at the beginning of the simulation
void Emitter::initialize()
{
    // Use current time as seed for the random number generator
    srand(time(0));
    // Initializing current ID with a random number
    currentID = rand();
    // default value for ACK timeout
    ACK_TIMEOUT = 2;
    // default value for random factor
    ACK_RANDOM_FACTOR = 1.5;
    // default value for retransmission
    MAX_RETRANSMIT = 4;
    // initializing initial timeout
    initTimeout = 0;
    // initializing retransmission counter
    retransmissionCounter = 0;
    // get the timeout value from the NED file
    timeout = par("timeout");
    // create a message to send
    timeoutEvent = new cMessage ("timeoutEvent");
    // create a initial message to send
    initEvent = new cMessage("initEvent");
    // schedule the message to be sent immediately
    scheduleAt(simTime(),initEvent);
    // record the time the packet was sent
    //t1 = simTime().dbl();
    // schedule a timeout event
    //scheduleAt(simTime() + timeout, timeoutEvent);
}

void Emitter::handleMessage(cMessage *msg)
{
    // if the message is the timeout event
    if(msg == timeoutEvent)
    {
        // output a log message
        EV << "Timeout expired, CON packet is probably lost.\n";
        // incrementing retransmission counter
        retransmissionCounter = ++ retransmissionCounter;
        // if transmission is less or equal than 4
        if (retransmissionCounter<=MAX_RETRANSMIT)
        {
            // create a new message to re-send
            Packet *conPacket = new Packet("CON");
            // re-using current ID number
            conPacket->setNid(currentID);
            // output a log message
            EV << "Re-sending CON packet, ID: " << conPacket->getNid() << std::endl;
            // send the message out
            send(conPacket, "out");
            // doubling timeout
            initTimeout=initTimeout*2;
            // output a log message
            EV << "ACK Timeout is: " << initTimeout << std::endl;
            // schedule a new timeout event
            scheduleAt(simTime()+ initTimeout, timeoutEvent);
        }
        // if transmission is more than 4
        else
        {
            // output a log message
            EV << "Retransmission of CON packet, ID: " << currentID << " failed" << std::endl;
            // create a new message to re-send
            Packet *conPacket = new Packet("CON");
            // incrementing current ID number
            currentID = ++currentID;
            // using current ID number
            conPacket->setNid(currentID);
            // output a log message
            EV << "Sending next CON packet, ID: " << conPacket->getNid() << std::endl;
            // send the message out
            send(conPacket, "out");
            // resetting retransmission counter
            retransmissionCounter=0;
            //double upperBound=ACK_TIMEOUT*ACK_RANDOM_FACTOR;
            //double lowerBound=ACK_TIMEOUT;
            // initTimeout= rand() % (upperBound - lowerBound + 1.0) + lowerBound;
            initTimeout=2;
            // schedule a new timeout event
            scheduleAt(simTime()+ initTimeout, timeoutEvent);
        }

    }
    else if (msg == initEvent)
    {
        // canceling the timeout event
        cancelEvent(timeoutEvent);
        // output message
        EV << "Emitter initialized" << std::endl;
        // create a new packet to send
        Packet *conPacket = new Packet("CON");
        // using current ID number
        conPacket->setNid(currentID);
        // send the message out
        send(conPacket, "out");
        // output a log message
        EV << "Sending first CON packet, ID: " << conPacket->getNid() << std::endl;
        // schedule a new timeout event
        scheduleAt(simTime(), timeoutEvent);
    }
    // if the message is not the timeout event
    else
    {
        // receiving ACK Packet
        Packet *ackPacket = check_and_cast<Packet *>(msg);
        // if ID from ackPacket is the same with current ID
        if (currentID == ackPacket->getNid())
        {
            // displaying a bubble
            bubble("ID: OK");
            // output a log message
            EV << "Receiving " << msg->getName() << " packet, ID: " << ackPacket->getNid() << std::endl;
            // output confirmation message
            EV << "ID from " << msg->getName() << " packet: OK" << std::endl;
            // canceling the timeout event
            cancelEvent(timeoutEvent);
            // record the time the packet was received
            t2 = simTime().dbl();
            // calculate the round trip time
            rtt = t2 - t1;
            // delete the message
            delete msg;
            // create a new message to send
            Packet *conPacket = new Packet("CON");
            // incrementing current ID number
            currentID = ++currentID;
            // resetting retransmission counter
            retransmissionCounter = 0;
            // setting conPacket with the new current ID number
            conPacket->setNid(currentID);
            // output a log message
            EV << "Sending next CON packet, ID: " << conPacket->getNid() << std::endl;
            // send the message out
            send(conPacket,"out");
            // record the time the packet was sent
            t1 = simTime().dbl();
            //double upperBound=ACK_TIMEOUT*ACK_RANDOM_FACTOR;
            //double lowerBound=ACK_TIMEOUT;
            // initTimeout= rand() % (upperBound - lowerBound + 1.0) + lowerBound;
            initTimeout=2;
            // schedule a new timeout event
            scheduleAt(simTime()+ initTimeout, timeoutEvent);
        }
        // if ID from ackPacket is NOT the same with current ID
        else if (currentID != ackPacket->getNid())
        {
            // canceling the timeout event
            cancelEvent(timeoutEvent);
            // create a new message to re-send
            Packet *conPacket = new Packet("CON");
            // re-using current ID number
            conPacket->setNid(currentID);
            // output a log message
            EV << "Re-sending CON packet, ID: " << conPacket->getNid() << std::endl;
            // send the message out
            send(conPacket, "out");
            // doubling timeout
            initTimeout=initTimeout*2;
            // schedule a new timeout event
            scheduleAt(simTime()+ initTimeout, timeoutEvent);
        }
    }
}

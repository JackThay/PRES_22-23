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
    srand(time(0)); // Use current time as seed for the random number generator
    currentID = rand(); // Initializing current ID with a random number
    ACK_TIMEOUT = 2; // default value for ACK timeout
    ACK_RANDOM_FACTOR = 1.5; // default value for random factor
    MAX_RETRANSMIT = 4; // default value for retransmission
    initTimeout = 0; // initializing initial timeout
    retransmissionCounter = 0; // initializing retransmission counter
    initEvent = new cMessage("initEvent"); // create a initial message to send
    scheduleAt(simTime(),initEvent); // schedule the message to be sent immediately
}

void Emitter::handleMessage(cMessage *msg)
{
    if (msg == initEvent) // if the message is the initial event
    {
        EV << "Emitter initialized" << std::endl; // output message
        Packet *conPacket = new Packet("CON"); // create a new packet to send
        conPacket->setNid(currentID); // using current ID number
        send(conPacket, "out"); // send the message out
        t1 = simTime().dbl(); // record the time the packet was sent
        timeoutEvent = new cMessage("timeoutEvent"); // create a timeout
        EV << "Sending first CON packet, ID: " << conPacket->getNid() << std::endl; // output a log message
        std::setprecision(3); // limiting initTimeout to milliseconds
        initTimeout = randomDouble(ACK_TIMEOUT, ACK_TIMEOUT * ACK_RANDOM_FACTOR); // changing initial timeout
        EV << "ACK Timeout for CON packet ID: " << conPacket->getNid() << " is: " << initTimeout << std::endl; // output a log message
        scheduleAt(simTime() + initTimeout, timeoutEvent); // schedule a new timeout event
    }
    else if(msg == timeoutEvent) // if the message is the timeout event
    {
        EV << "Timeout expired, CON packet is probably lost.\n"; // output a log message
        retransmissionCounter = ++ retransmissionCounter; // incrementing retransmission counter
        if (retransmissionCounter<=MAX_RETRANSMIT) // if transmission is less or equal than 4
        {
            Packet *conPacket = new Packet("CON"); // create a new message to re-send
            conPacket->setNid(currentID); // re-using current ID number
            EV << "Re-sending CON packet, ID: " << conPacket->getNid() << std::endl; // output a log message
            send(conPacket, "out"); // send the message out
            initTimeout=initTimeout*2; // doubling timeout
            EV << "ACK Timeout for CON packet ID: " << conPacket->getNid() << " is: " << initTimeout << std::endl; // output a log message
            scheduleAt(simTime()+ initTimeout, timeoutEvent); // schedule a new timeout event
        }
        else // if transmission is more than 4
        {
            EV << "Retransmission of CON packet, ID: " << currentID << " failed" << std::endl; // output a log message
            Packet *conPacket = new Packet("CON"); // create a new message to re-send
            currentID = ++currentID; // incrementing current ID number
            conPacket->setNid(currentID); // using current ID number
            EV << "Sending next CON packet, ID: " << conPacket->getNid() << std::endl; // output a log message
            send(conPacket, "out"); // send the message out
            t1 = simTime().dbl(); // record the time the packet was sent
            retransmissionCounter=0; // resetting retransmission counter
            std::setprecision(3); // limiting initTimeout to milliseconds
            initTimeout = randomDouble(ACK_TIMEOUT, ACK_TIMEOUT * ACK_RANDOM_FACTOR); // changing initial timeout
            EV << "ACK Timeout for CON packet ID: " << conPacket->getNid() << " is: " << initTimeout << std::endl; // output a log message
            scheduleAt(simTime()+ initTimeout, timeoutEvent); // schedule a new timeout event
        }
    }
    else // if the message is not the timeout event
    {
        Packet *ackPacket = check_and_cast<Packet *>(msg); // receiving ACK Packet
        if (currentID == ackPacket->getNid()) // if ID from ackPacket is the same with current ID
        {
            bubble("ID: OK"); // displaying a bubble
            EV << "Receiving " << msg->getName() << " packet, ID: " << ackPacket->getNid() << std::endl; // output a log message
            EV << "ID from " << msg->getName() << " packet: OK" << std::endl; // output confirmation message
            cancelEvent(timeoutEvent); // canceling the timeout event
            t2 = simTime().dbl(); // record the time the packet was received
            rtt = t2 - t1; // calculate the round trip time
            EV << "RTT for packet ID: " << ackPacket->getNid() << " was: " << rtt << "s" << std::endl; // output a log message
            delete msg; // delete the message
            Packet *conPacket = new Packet("CON"); // create a new message to send
            currentID = ++currentID; // incrementing current ID number
            retransmissionCounter = 0; // resetting retransmission counter
            conPacket->setNid(currentID); // setting conPacket with the new current ID number
            EV << "Sending next CON packet, ID: " << conPacket->getNid() << std::endl; // output a log message
            send(conPacket,"out"); // send the message out
            t1 = simTime().dbl(); // record the time the packet was sent
            std::setprecision(3); // limiting initTimeout to milliseconds
            initTimeout = randomDouble(ACK_TIMEOUT, ACK_TIMEOUT * ACK_RANDOM_FACTOR); // changing initial timeout
            EV << "ACK Timeout for CON packet ID: " << conPacket->getNid() << " is: " << initTimeout << std::endl; // output a log message
            scheduleAt(simTime()+ initTimeout, timeoutEvent); // schedule a new timeout event
        }
        else if (currentID != ackPacket->getNid()) // if ID from ackPacket is NOT the same with current ID
        {
            cancelEvent(timeoutEvent); // canceling the timeout event
            Packet *conPacket = new Packet("CON"); // create a new message to re-send
            conPacket->setNid(currentID); // re-using current ID number
            EV << "Re-sending CON packet, ID: " << conPacket->getNid() << std::endl; // output a log message
            send(conPacket, "out"); // send the message out
            initTimeout=initTimeout*2; // doubling timeout
            EV << "ACK Timeout for CON packet ID: " << conPacket->getNid() << " is: " << initTimeout << std::endl; // output a log message
            scheduleAt(simTime()+ initTimeout, timeoutEvent); // schedule a new timeout event
        }
    }
}

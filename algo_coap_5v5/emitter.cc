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
    int packet_size = par("packet_size");
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
        cPacket *conPacket = new cPacket("CON"); // create a new packet to send
        conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
        send(conPacket, SendOptions().transmissionId(currentID), "out"); // setting transmissionId to currentID and send the message out
        t1 = simTime().dbl(); // record the time the packet was sent
        timeoutEvent = new cMessage("timeoutEvent"); // create a timeout
        EV << "Sending first CON packet, ID: " << conPacket->getTransmissionId() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
        std::setprecision(3); // limiting initTimeout to milliseconds
        initTimeout = randomDouble(ACK_TIMEOUT, ACK_TIMEOUT * ACK_RANDOM_FACTOR); // changing initial timeout
        EV << "ACK Timeout for CON packet ID: " << conPacket->getTransmissionId() << " is: " << initTimeout << "s" << std::endl; // output a log message
        scheduleAt(simTime() + initTimeout, timeoutEvent); // schedule a new timeout event
    }
    else if(msg == timeoutEvent) // if the message is the timeout event
    {
        EV << "Timeout expired, CON packet is probably lost.\n"; // output a log message
        retransmissionCounter = ++ retransmissionCounter; // incrementing retransmission counter
        if (retransmissionCounter<=MAX_RETRANSMIT) // if transmission is less or equal than 4
        {
            cPacket *conPacket = new cPacket("CON");; // create a new message to re-send
            conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
            EV << "Re-sending CON packet, ID: " << conPacket->getTransmissionId() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
            send(conPacket, SendOptions().transmissionId(currentID), "out"); // setting transmissionId to currentID and send the message out
            initTimeout=initTimeout*2; // doubling timeout
            EV << "ACK Timeout for CON packet ID: " << conPacket->getTransmissionId() << " is: " << initTimeout << "s" << std::endl; // output a log message
            scheduleAt(simTime()+ initTimeout, timeoutEvent); // schedule a new timeout event
        }
        else // if transmission is more than 4
        {
            EV << "Retransmission of CON packet, ID: " << currentID << " failed" << std::endl; // output a log message
            cPacket *conPacket = new cPacket("CON"); // create a new message to re-send
            conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
            currentID = ++currentID; // incrementing current ID number
            send(conPacket, SendOptions().transmissionId(currentID), "out"); // setting transmissionId to currentID and send the message out
            EV << "Sending next CON packet, ID: " << conPacket->getTransmissionId() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
            t1 = simTime().dbl(); // record the time the packet was sent
            retransmissionCounter=0; // resetting retransmission counter
            std::setprecision(3); // limiting initTimeout to milliseconds
            initTimeout = randomDouble(ACK_TIMEOUT, ACK_TIMEOUT * ACK_RANDOM_FACTOR); // changing initial timeout
            EV << "ACK Timeout for CON packet ID: " << conPacket->getTransmissionId() << " is: " << initTimeout << "s" << std::endl; // output a log message
            scheduleAt(simTime()+ initTimeout, timeoutEvent); // schedule a new timeout event
        }
    }
    else // if the message is not the timeout event
    {
        cPacket *ackPacket = check_and_cast<cPacket *>(msg); // receiving ACK Packet
        if (strcmp("ACK",msg->getName()) == 0) // checking if the incoming message is an ACK
        {
            EV << "Receiving " << msg->getName() << " packet, ID: " << ackPacket->getTransmissionId() << std::endl; // output a log message
            if (currentID == ackPacket->getTransmissionId()) // if ID from ackPacket is the same with current ID
            {
                bubble("ID: OK"); // displaying a bubble
                EV << "ID from " << msg->getName() << " packet: OK" << std::endl; // output confirmation message
                cancelEvent(timeoutEvent); // canceling the timeout event
                t2 = simTime().dbl(); // record the time the packet was received
                rtt = t2 - t1; // calculate the round trip time
                EV << "RTT for packet ID: " << ackPacket->getTransmissionId() << " was: " << rtt << "s" << std::endl; // output a log message
                download_speed = getDownloadSpeed(rtt, par("packet_size"));
                EV << "Download speed for packet ID: " << ackPacket->getTransmissionId() << " was: " << download_speed << "b/s" << std::endl; // output a log message
                delete msg; // delete the message
                cPacket *conPacket = new cPacket("CON"); // create a new message to send
                conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
                currentID = ++currentID; // incrementing current ID number
                retransmissionCounter = 0; // resetting retransmission counter
                send(conPacket, SendOptions().transmissionId(currentID), "out"); // setting transmissionId to currentID and send the message out
                EV << "Sending next CON packet, ID: " << conPacket->getTransmissionId() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
                t1 = simTime().dbl(); // record the time the packet was sent
                std::setprecision(3); // limiting initTimeout to milliseconds
                initTimeout = randomDouble(ACK_TIMEOUT, ACK_TIMEOUT * ACK_RANDOM_FACTOR); // changing initial timeout
                EV << "ACK Timeout for CON packet ID: " << conPacket->getTransmissionId() << " is: " << initTimeout << "s" << std::endl; // output a log message
                scheduleAt(simTime()+ initTimeout, timeoutEvent); // schedule a new timeout event
            }
            else if (currentID != ackPacket->getTransmissionId()) // if ID from ackPacket is NOT the same with current ID
            {
                cancelEvent(timeoutEvent); // canceling the timeout event
                cPacket *conPacket = new cPacket("CON"); // create a new message to re-send
                conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
                send(conPacket, SendOptions().transmissionId(currentID), "out"); // send the message out
                EV << "Re-sending CON packet, ID: " << conPacket->getTransmissionId() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
                initTimeout=initTimeout*2; // doubling timeout
                EV << "ACK Timeout for CON packet ID: " << conPacket->getTransmissionId() << " is: " << initTimeout << "s" << std::endl; // output a log message
                scheduleAt(simTime()+ initTimeout, timeoutEvent); // schedule a new timeout event
            }

        }
        else // obviously, if it's not an ACK, this should not be happening!!
        {
            bubble("WTF?!");
            EV << "What the fuck was that?! That's not a ACK!" << std::endl; // output a log message
            delete msg; // Delete the incoming message if processing is already in progress
        }

    }
}

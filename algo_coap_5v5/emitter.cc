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
    packet_size = par("packet_size");
    clientID = rand(); // used to ID client if there is several
    currentID = rand(); // Initializing current ID with a random number
    packet_sent = 0;
    ACK_TIMEOUT = 2; // default value for ACK timeout
    ACK_RANDOM_FACTOR = 1.5; // default value for random factor
    MAX_RETRANSMIT = 4; // default value for retransmission
    initTimeout = 0; // initializing initial timeout
    retransmissionCounter = 0; // initializing retransmission counter
    initEvent = new cMessage("initEvent"); // create a initial message to send
    getUploadSpeed = new cMessage("getUploadSpeed");
    scheduleAt(simTime(),initEvent); // schedule the message to be sent immediately
    scheduleAt(simTime()+1,getUploadSpeed);
}


void Emitter::handleMessage(cMessage *msg)
{
    if (msg == initEvent) // if the message is the initial event
    {
        srand(time(0)); // Use current time as seed for the random number generator
        EV << "Emitter number " << clientID <<" initialized" << std::endl; // output message
        delete msg; // delete the message
        Packet *conPacket = new Packet("CON"); // create a new packet to send
        conPacket->setNid(currentID); // setting NID to currend ID
        conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
        send(conPacket, "out"); // setting transmissionId to currentID and send the message out
        packet_sent = ++packet_sent; // used to count the number of packet sent in a second
        t1 = simTime().dbl(); // record the time the packet was sent
        timeoutEvent = new cMessage("timeoutEvent"); // create a timeout
        EV << "Sending first CON packet, ID: " << conPacket->getNid() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
        initTimeout = randomDouble(ACK_TIMEOUT, ACK_TIMEOUT * ACK_RANDOM_FACTOR); // changing initial timeout
        EV << "ACK Timeout for CON packet ID: " << conPacket->getNid() << " is: " << initTimeout << "s" << std::endl; // output a log message
        scheduleAt(simTime()+ initTimeout, timeoutEvent); // schedule a new timeout event
    }
    else if(msg == getUploadSpeed)
    {
        delete msg; // delete the message
        int upload_speed = packet_size * packet_sent;
        EV << "This is a time counter: " << simTime().dbl()<< std::endl;
        EV << "Upload speed: " << upload_speed << "bits/s"<< std::endl;
        packet_sent = 0; // resetting number of packet sent
        getUploadSpeed = new cMessage("getUploadSpeed");
        scheduleAt(simTime()+1,getUploadSpeed);
    }
    else if(msg == timeoutEvent) // if the message is the timeout event
    {
        EV << "Timeout expired, CON packet is probably lost.\n"; // output a log message
        retransmissionCounter = ++retransmissionCounter; // incrementing retransmission counter
        if (retransmissionCounter <= MAX_RETRANSMIT) // if transmission is less or equal than 4
        {
            Packet *conPacket = new Packet("CON"); // create a new Packet to re-send
            conPacket->setNid(currentID); // setting NID to current ID
            conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
            send(conPacket, "out"); // setting transmissionId to currentID and send the message out
            packet_sent = ++packet_sent; // used to count the number of packet sent in a second
            EV << "Re-sending CON packet, ID: " << conPacket->getNid() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
            initTimeout=initTimeout*2; // doubling timeout
            EV << "ACK Timeout for CON packet ID: " << conPacket->getNid() << " is: " << initTimeout << "s" << std::endl; // output a log message
            scheduleAt(simTime()+ initTimeout, timeoutEvent); // schedule a new timeout event
        }
        else // if transmission is more than 4
        {
            EV << "Retransmission of CON packet, ID: " << currentID << " failed" << std::endl; // output a log message
            currentID = ++currentID; // incrementing current ID number
            Packet *conPacket = new Packet("CON"); // create next Packet to send
            conPacket->setNid(currentID); // setting NID to new current ID
            conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
            send(conPacket, "out"); // setting transmissionId to currentID and send the message out
            packet_sent = ++packet_sent; // used to count the number of packet sent in a second
            EV << "Sending next CON packet, ID: " << conPacket->getNid() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
            t1 = simTime().dbl(); // record the time the packet was sent
            retransmissionCounter = 0; // resetting retransmission counter
            initTimeout = randomDouble(ACK_TIMEOUT, ACK_TIMEOUT * ACK_RANDOM_FACTOR); // changing initial timeout
            EV << "ACK Timeout for CON packet ID: " << conPacket->getNid() << " is: " << initTimeout << "s" << std::endl; // output a log message
            scheduleAt(simTime()+ initTimeout, timeoutEvent); // schedule a new timeout event
        }
    }
    else // if the message is not the timeout event
    {
        Packet *ackPacket = check_and_cast<Packet *>(msg); // receiving ACK Packet
        if (strcmp("ACK",msg->getName()) == 0) // checking if the incoming message is an ACK
        {
            EV << "Receiving " << msg->getName() << " packet, ID: " << ackPacket->getNid() << std::endl; // output a log message
            if (currentID == ackPacket->getNid()) // if ID from ackPacket is the same with current ID
            {
                bubble("ID: OK"); // displaying a bubble
                EV << "ID from " << msg->getName() << " packet: OK" << std::endl; // output confirmation message
                cancelEvent(timeoutEvent); // canceling the timeout event
                t2 = simTime().dbl(); // record the time the packet was received
                rtt = t2 - t1; // calculate the round trip time
                currentID = ++currentID; // incrementing current ID number
                EV << "RTT for packet ID: " << ackPacket->getNid() << " was: " << rtt << "s" << std::endl; // output a log message
                delete msg; // delete the message
                Packet *conPacket = new Packet("CON"); // create next Packet to send
                conPacket->setNid(currentID); // setting NID to new current ID
                conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
                retransmissionCounter = 0; // resetting retransmission counter
                send(conPacket, "out"); // setting transmissionId to currentID and send the message out
                packet_sent = ++packet_sent; // used to count the number of packet sent in a second
                EV << "Sending next CON packet, ID: " << conPacket->getNid() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
                t1 = simTime().dbl(); // record the time the packet was sent
                initTimeout = randomDouble(ACK_TIMEOUT, ACK_TIMEOUT * ACK_RANDOM_FACTOR); // changing initial timeout
                EV << "ACK Timeout for CON packet ID: " << conPacket->getNid() << " is: " << initTimeout << "s" << std::endl; // output a log message
                scheduleAt(simTime()+ initTimeout, timeoutEvent); // schedule a new timeout event
            }
            else if (currentID != ackPacket->getNid()) // if ID from ackPacket is NOT the same with current ID
            {
                cancelEvent(timeoutEvent); // canceling the timeout event
                Packet *conPacket = new Packet("CON"); // create a new Packet to re-send
                conPacket->setNid(currentID); // setting NID to current ID
                conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
                send(conPacket, "out"); // send the message out
                packet_sent = ++packet_sent; // used to count the number of packet sent in a second
                EV << "Re-sending CON packet, ID: " << conPacket->getNid() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
                initTimeout=initTimeout*2; // doubling timeout
                EV << "ACK Timeout for CON packet ID: " << conPacket->getNid() << " is: " << initTimeout << "s" << std::endl; // output a log message
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

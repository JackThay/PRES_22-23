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
    packet_sent = 0; // counter for the number of packet sent and its ACK
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
        EV << "Emitter using CoAP, number " << clientID <<" initialized" << std::endl; // output message
        delete msg; // delete the message
        Packet *conPacket = new Packet("CON"); // create a new packet to send
        conPacket->setNid(currentID); // setting NID to current ID
        conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
        send(conPacket, "out"); // setting transmissionId to currentID and send the message out
        t1 = simTime().dbl(); // record the time the packet was sent
        timeoutEvent = new cMessage("timeoutEvent"); // create a timeout
        EV << "Emitter number: " << clientID << " => Sending first CON packet, ID: " << conPacket->getNid() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
        initTimeout = randomDouble(ACK_TIMEOUT, ACK_TIMEOUT * ACK_RANDOM_FACTOR); // changing initial timeout
        EV << "Emitter number: " << clientID << " => ACK Timeout for CON packet ID: " << conPacket->getNid() << " is: " << initTimeout << "s" << std::endl; // output a log message
        scheduleAt(simTime()+ initTimeout, timeoutEvent); // schedule a new timeout event
    }
    else if(msg == getUploadSpeed)
    {
        delete msg; // delete the message
        int upload_speed = packet_size * packet_sent;
        packet_sent = 0; // resetting number of packet sent
        getUploadSpeed = new cMessage("getUploadSpeed");
        scheduleAt(simTime()+1,getUploadSpeed);
    }
    else if(msg == timeoutEvent) // if the message is the timeout event
    {
        retransmissionCounter = ++retransmissionCounter; // incrementing retransmission counter
        if (retransmissionCounter <= MAX_RETRANSMIT) // if transmission is less or equal than 4
        {
            Packet *conPacket = new Packet("CON"); // create a new Packet to re-send
            conPacket->setNid(currentID); // setting NID to current ID
            conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
            send(conPacket, "out"); // setting transmissionId to currentID and send the message out
            initTimeout=initTimeout*2; // doubling timeout
            scheduleAt(simTime()+ initTimeout, timeoutEvent); // schedule a new timeout event
        }
        else // if transmission is more than 4
        {
            currentID = ++currentID; // incrementing current ID number
            Packet *conPacket = new Packet("CON"); // create next Packet to send
            conPacket->setNid(currentID); // setting NID to new current ID
            conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
            send(conPacket, "out"); // setting transmissionId to currentID and send the message out
            t1 = simTime().dbl(); // record the time the packet was sent
            retransmissionCounter = 0; // resetting retransmission counter
            initTimeout = randomDouble(ACK_TIMEOUT, ACK_TIMEOUT * ACK_RANDOM_FACTOR); // changing initial timeout
            scheduleAt(simTime()+ initTimeout, timeoutEvent); // schedule a new timeout event
        }
    }
    else // if the message is not the timeout event
    {
        Packet *ackPacket = check_and_cast<Packet *>(msg); // receiving ACK Packet
        if (strcmp("ACK",msg->getName()) == 0) // checking if the incoming message is an ACK
        {
            if (currentID == ackPacket->getNid()) // if ID from ackPacket is the same with current ID
            {
                bubble("ID: OK"); // displaying a bubble
                cancelEvent(timeoutEvent); // canceling the timeout event
                packet_sent = ++packet_sent; // used to count the number of ACK received in a second
                t2 = simTime().dbl(); // record the time the packet was received
                rtt = t2 - t1; // calculate the round trip time
                currentID = ++currentID; // incrementing current ID number
                delete msg; // delete the message
                Packet *conPacket = new Packet("CON"); // create next Packet to send
                conPacket->setNid(currentID); // setting NID to new current ID
                conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
                retransmissionCounter = 0; // resetting retransmission counter
                send(conPacket, "out"); // setting transmissionId to currentID and send the message out
                t1 = simTime().dbl(); // record the time the packet was sent
                initTimeout = randomDouble(ACK_TIMEOUT, ACK_TIMEOUT * ACK_RANDOM_FACTOR); // changing initial timeout
                scheduleAt(simTime()+ initTimeout, timeoutEvent); // schedule a new timeout event
            }
            else if (currentID != ackPacket->getNid()) // if ID from ackPacket is NOT the same with current ID
            {
                bubble("ID: ERROR"); // displaying a bubble
                cancelEvent(timeoutEvent); // canceling the timeout event
                Packet *conPacket = new Packet("CON"); // create a new Packet to re-send
                conPacket->setNid(currentID); // setting NID to current ID
                conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
                send(conPacket, "out"); // send the message out
                initTimeout=initTimeout*2; // doubling timeout
                scheduleAt(simTime()+ initTimeout, timeoutEvent); // schedule a new timeout event
            }

        }
        else // obviously, if it's not an ACK, this should not be happening!!
        {
            bubble("ERROR");
            EV << "Message received was not an ACK" << std::endl; // output a log message
            delete msg; // Delete the incoming message if processing is already in progress
        }

    }
}

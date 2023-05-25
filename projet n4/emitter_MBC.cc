#include "emitter_MBC.h"

Define_Module(Emitter_MBC);

// Constructor
Emitter_MBC::Emitter_MBC()
{
    nextPacket = NULL;
}

// Destructor
Emitter_MBC::~Emitter_MBC(){
    cancelAndDelete(nextPacket);
}

void Emitter_MBC::initialize()
{
    packet_size = par("packet_size");
    clientID = rand(); // used to ID client if there is several
    currentID = rand(); // Initializing current ID with a random number
    m = 8;
    timeout = 1;
    lastAckTime = 0;
    lastSendTime = 0;
    cycleIndex = 0;
    pg = 1;
    ACK_TIMEOUT = 2; // default value for ACK timeout
    ACK_RANDOM_FACTOR = 1.5; // default value for random factor
    initTimeout = randomDouble(ACK_TIMEOUT, ACK_TIMEOUT * ACK_RANDOM_FACTOR); // initializing initial timeout
    spacing = 0.7; // Seriously, I don't know what value we're supposed to write down here
    packet_sent= 1; // 1 because the 1st packet is sent immediately
    ack_received = 0;
    retransmission = true; // only an ACK can change this value to false

    // Here start the code we usually use from "initEvent" of regular CoAP
    EV << "Emitter using MBC-CoAP, number " << clientID <<" initialized" << std::endl; // output message
    Packet *conPacket = new Packet("CON"); // create a new packet to send
    conPacket->setNid(currentID); // setting NID to current ID
    conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
    send(conPacket, "out");
    EV << "Emitter number: " << clientID << " => Sending first CON packet, ID: " << conPacket->getNid() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
    EV << "Emitter number: " << clientID << " => Spacing for CON packet ID: " << conPacket->getNid() << " is: " << spacing << "s (user-defined)" << std::endl; // output a log message
    getUploadSpeed = new cMessage("getUploadSpeed");
    scheduleAt(simTime()+1,getUploadSpeed);
}

void Emitter_MBC::handleMessage(cMessage *msg)
{
    srand(time(0)); // Use current time as seed for the random number generator
    double currentTime = simTime().dbl();
    if(strcmp("getUploadSpeed",msg->getName()) == 0)
    {
        delete msg; // delete the message
        EV << "Emitter number: " << clientID << " => This is a time counter: " << simTime().dbl()<< "s" << std::endl;
        EV << "Emitter number: " << clientID << " => Total packet sent: " << packet_sent << std::endl;
        EV << "Emitter number: " << clientID << " => Total ACK received: " << ack_received << std::endl;
        getUploadSpeed = new cMessage("getUploadSpeed");
        scheduleAt(simTime()+1,getUploadSpeed);
    }
    if (strcmp("ACK",msg->getName()) == 0)
    {
        Packet *ackPacket = check_and_cast<Packet *>(msg); // receiving ACK Packet
        EV << "Emitter number: " << clientID << " => Receiving " << msg->getName() << " packet, ID: " << ackPacket->getNid() << std::endl; // output a log message
        if (currentID == ackPacket->getNid()) // checking ACK
        {
            bubble("ID: OK"); // displaying a bubble
            EV << "Emitter number: " << clientID << " => ID from " << msg->getName() << " packet: OK" << std::endl; // output confirmation message
            ack_received = ++ack_received;
            retransmission = false;
        }
        else
        {
            bubble("ID: ERROR"); // displaying a bubble
            EV << "Emitter number: " << clientID << " => ID from " << msg->getName() << " packet: ERROR" << std::endl; // output confirmation message
        }
        double measurementSample = currentTime - lastAckTime;
        lastAckTime = currentTime;
        spacings.push_back(measurementSample);
        if (spacings.size() > m)
        {
            spacings.pop_front();
        }
    }

    double nextSendTime = std::max(lastSendTime + spacing, currentTime);
    nextPacket = new Packet("nextPacket");
    scheduleAt(nextSendTime, nextPacket);
    lastSendTime = nextSendTime;

    cycleIndex = (cycleIndex + 1) % 8;

    if (cycleIndex == 0)
    {
        pg = !"ACK" ? 0.75 : 1.25;
    }
    else if (cycleIndex == 1)
    {
        pg = 0.75;
    }
    else
    {
        pg = 1;
    }

    spacing = *std::min_element(spacings.begin(), spacings.end()) / pg;

    if (strcmp("nextPacket",msg->getName()) == 0) // when self message is "nextPacket"
    {
        //delete msg; // somehow, deleting the message here will crash the simulation
        if (retransmission == false)
            currentID = ++currentID; // incrementing current ID number if retransmission is false
        Packet *conPacket = new Packet("CON"); // create a new packet to send
        conPacket->setNid(currentID); // setting NID to current ID
        conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
        send(conPacket, "out");
        packet_sent = ++packet_sent; // used to count the number of packet sent in a second
        retransmission = true; // only an ACK can change this value to false
        EV << "Emitter number: " << clientID << " => sending next CON packet, ID: " << conPacket->getNid() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
    }
}

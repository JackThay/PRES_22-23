#include "emitter.h"

Define_Module(Emitter);

void Emitter::initialize()
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
    spacing = 0;
    packet_sent= 1; // 1 because the 1st packet is sent immediately

    // Here start the code we usually use from "initEvent" of regular CoAP
    EV << "Emitter number " << clientID <<" initialized" << std::endl; // output message
    Packet *conPacket = new Packet("CON"); // create a new packet to send
    conPacket->setNid(currentID); // setting NID to current ID
    conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
    send(conPacket, "out");
    expectedAck.push_back(currentID);
    EV << "Sending first CON packet, ID: " << conPacket->getNid() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
    EV << "ACK expected: ";
    for (auto itr = expectedAck.begin(); itr != expectedAck.end(); itr++)
    {
        EV << *itr << " ";
    }
    EV << std::endl;
    getUploadSpeed = new cMessage("getUploadSpeed");
    scheduleAt(simTime()+1,getUploadSpeed);
}

void Emitter::handleMessage(cMessage *msg)
{
    srand(time(0)); // Use current time as seed for the random number generator
    double currentTime = simTime().dbl();
    if(strcmp("getUploadSpeed",msg->getName()) == 0)
    {
        delete msg; // delete the message
        int upload_speed = packet_size * packet_sent;
        EV << "This is a time counter: " << simTime().dbl()<< std::endl;
        EV << "Upload speed: " << upload_speed << "bits/s"<< std::endl;
        packet_sent = 0; // resetting number of packet sent
        getUploadSpeed = new cMessage("getUploadSpeed");
        scheduleAt(simTime()+1,getUploadSpeed);
    }
    if (strcmp("ACK",msg->getName()) == 0)
    {
        Packet *ackPacket = check_and_cast<Packet *>(msg); // receiving ACK Packet
        EV << "Receiving " << msg->getName() << " packet, ID: " << ackPacket->getNid() << std::endl; // output a log message
        if (find(expectedAck.begin(), expectedAck.end(), ackPacket->getNid()) != end(expectedAck)) // searching ACK ID in the list of expected ACK
        {
            EV << "ACK for CON packet, ID: "<< ackPacket->getNid() << " received!" << std::endl; // output a log message
            expectedAck.erase(find(expectedAck.begin(), expectedAck.end(), ackPacket->getNid())); // removing ACK ID from list of expected ACK
        }
        else
        {
            EV << "ACK packet, ID: "<< ackPacket->getNid() << " not expected!" << std::endl; // output a log message
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
    nextPacket = new cMessage("nextPacket");
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
        delete msg; // delete the message
        currentID = ++currentID; // incrementing current ID number
        Packet *conPacket = new Packet("CON"); // create a new packet to send
        conPacket->setNid(currentID); // setting NID to current ID
        conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
        send(conPacket, "out");
        packet_sent = ++packet_sent; // used to count the number of packet sent in a second
        expectedAck.push_back(currentID);
        EV << "Sending next CON packet, ID: " << conPacket->getNid() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
        EV << "ACK expected: ";
        for (auto itr = expectedAck.begin(); itr != expectedAck.end(); itr++)
        {
            EV << *itr << " ";
        }
        EV << std::endl;
    }
}

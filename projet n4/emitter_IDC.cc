#include "emitter_IDC.h"

Define_Module(Emitter_IDC);

void Emitter_IDC::initialize()
{
    packet_size = par("packet_size");
    clientID = rand(); // used to ID client if there is several
    currentID = rand(); // Initializing current ID with a random number
    incrementalWeight = 1.5;
    decrementalWeight = 0.01;
    fastDecrementalWeight = 0.5;
    initialSpacing = 0.5;
    current_last = 0;
    packet_sent = 0;
    ack_received = 0;
    rto = 1.0;
    ACK = false;
    FirstCON = false;
    spacing = 0.7;
    loss_spacing = 0;
    sendPacket = new cMessage("sendPacket");
    getUploadSpeed = new cMessage("getUploadSpeed"); // create a timeout
    initEvent = new cMessage("initEvent"); // create a initial message to send
    scheduleAt(simTime(), initEvent); // schedule the message to be sent immediately
    scheduleAt(simTime()+1,getUploadSpeed);
}

void Emitter_IDC::handleMessage(cMessage *msg)
{
    if (msg == initEvent) // if the message is the initial event
    {
        srand(time(0)); // Use current time as seed for the random number generator
        EV << "Emitter using IDC-CoAP, number " << clientID <<" initialized" << std::endl; // output message
        Packet *conPacket = new Packet("CON"); // create a new packet to send
        conPacket->setNid(currentID); // setting NID to current ID
        conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
        send(conPacket, "out"); // send the CON Packet out
        packet_sent = ++packet_sent;
        t1 = simTime().dbl(); // record the time the packet was sent
        last_send_time = simTime().dbl(); // recording time of last sending
        EV << "Emitter number: " << clientID << " => Sending first CON packet, ID: " << conPacket->getNid() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
        EV << "Emitter number: " << clientID << " => Spacing for CON packet ID: " << conPacket->getNid() << " is: " << spacing << "s (user-defined)" << std::endl; // output a log message
        sendPacket = new cMessage("sendPacket");
        scheduleAt(last_send_time + spacing, sendPacket);
    }
    else if(msg == getUploadSpeed)
    {
        delete msg; // delete the message
        EV << "Emitter number: " << clientID << " => This is a time counter: " << simTime().dbl()<< "s" << std::endl;
        EV << "Emitter number: " << clientID << " => Total packet sent: " << packet_sent << std::endl;
        EV << "Emitter number: " << clientID << " => Total ACK received: " << ack_received << std::endl;
        getUploadSpeed = new cMessage("getUploadSpeed");
        scheduleAt(simTime()+1,getUploadSpeed);
    }
    else if (msg == sendPacket)
    {
        if (ACK==true)
        {
            EV << "Emitter number: " << clientID << " => ACK was received for CON packet, ID: " << currentID << ", switching to transmission of next packet" << std::endl; // output a log message
            currentID = ++currentID;
            ack_received = ++ack_received;
            if (spacing >= loss_spacing)
            {
                spacing = spacing - (decrementalWeight * spacing);
                EV << "Spacing after dw: "<< spacing << std::endl;
            }
            else
            {
                spacing = spacing - (fastDecrementalWeight * spacing);
            }
        }
        else
        {
            EV << "Emitter number: " << clientID << " => ACK was not received for CON packet, ID: " << currentID << ", switching to retransmission of packet" << std::endl; // output a log message
            loss_spacing = spacing;
            spacing = incrementalWeight * spacing;
        }
        spacing = std::max(spacing, current_last);
        Packet *conPacket = new Packet("CON"); // create a new packet to send
        conPacket->setNid(currentID); // setting NID to current ID
        conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
        send(conPacket, "out"); // send the CON Packet out
        packet_sent = ++packet_sent;
        EV << "Emitter number: " << clientID << " => Sending CON packet, ID: " << conPacket->getNid() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
        EV << "Emitter number: " << clientID << " => Spacing for CON packet ID: " << conPacket->getNid() << " is: " << spacing << "s" << std::endl; // output a log message
        ACK = false;
        last_send_time = simTime().dbl(); // recording time of last sending
        sendPacket = new cMessage("sendPacket");
        scheduleAt(last_send_time + spacing, sendPacket);
    }
    else
    {
        Packet *ackPacket = check_and_cast<Packet *>(msg); // receiving ACK Packet
        if (strcmp("ACK",msg->getName()) == 0) // checking if the incoming message is an ACK
        {
            EV << "Emitter number: " << clientID << " => Receiving " << msg->getName() << " packet, ID: " << ackPacket->getNid() << std::endl; // output a log message
            if (currentID == ackPacket->getNid()) // if ID from ackPacket is the same with current ID
            {
                bubble("ID: OK"); // displaying a bubble
                EV << "Emitter number: " << clientID << " => ID from " << msg->getName() << " packet: OK" << std::endl; // output confirmation message
                ACK = true;
            }
            else
            {
                bubble("ID: ERROR"); // displaying a bubble
            }
            current_last = simTime().dbl() - last_send_time;
        }
        else
        {
            EV << "Emitter number: " << clientID << " => Packet received was not an ACK" << std::endl; // output a log message
        }
    }
}

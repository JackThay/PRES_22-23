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
    currentID = rand(); // Initializing current ID with a random number
    ACK_TIMEOUT = 2; // default value for ACK timeout
    ACK_RANDOM_FACTOR = 1.5; // default value for random factor
    MAX_RETRANSMIT = 4; // default value for retransmission
    spacing = 0; // space between last transmission and next transmission/retransmission
    loss_spacing = 0; // space between last transmission an last timeout
    dw = 0.01; // decremental weight
    fw = 0.5; // fast decremental weight
    iw 1.5; // incremental weight
    initTimeout = 0; // initializing initial timeout
    retransmissionCounter = 0; // initializing retransmission counter
    initEvent = new cMessage("initEvent"); // create a initial message to send
    scheduleAt(simTime(),initEvent); // schedule the message to be sent immediately
}


void Emitter::handleMessage(cMessage *msg)
{
    if (msg == initEvent) // if the message is the initial event
    {
        srand(time(0)); // Use current time as seed for the random number generator
        EV << "Emitter initialized" << std::endl; // output message
        Packet *conPacket = new Packet("CON"); // create a new packet to send
        conPacket->setNid(currentID); // setting NID to currend ID
        conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
        send(conPacket, "out"); // setting transmissionId to currentID and send the message out
        t1 = simTime().dbl(); // record the time the packet was sent
        start_spacing = t1; // record the start of spacing
        timeoutEvent = new cMessage("timeoutEvent"); // create a timeout
        EV << "Sending first CON packet, ID: " << conPacket->getNid() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
        std::setprecision(3); // limiting initTimeout to milliseconds
        initTimeout = randomDouble(ACK_TIMEOUT, ACK_TIMEOUT * ACK_RANDOM_FACTOR); // changing initial timeout
        EV << "ACK Timeout for CON packet ID: " << conPacket->getNid() << " is: " << initTimeout << "s" << std::endl; // output a log message
        scheduleAt(simTime()+ initTimeout, timeoutEvent); // schedule a new timeout event
    }
    else if(msg == timeoutEvent) // if the message is the timeout event
    {
        EV << "Timeout expired, CON packet is probably lost.\n"; // output a log message
        retransmissionCounter = ++retransmissionCounter; // incrementing retransmission counter
        if (retransmissionCounter <= MAX_RETRANSMIT) // if transmission is less or equal than 4
        {
            end_spacing = simTime().dbl(); // recording the end of spacing
            spacing = end_spacing - start_spacing; // calculating actual spacing
            loss_spacing = spacing; // recording spacing for lost transmission
            spacing = iw * spacing; // recalculating new spacing
            Packet *conPacket = new Packet("CON"); // create a new Packet to re-send
            conPacket->setNid(currentID); // setting NID to current ID
            conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
            send(conPacket, "out"); // setting transmissionId to currentID and send the message out
            start_spacing = simTime().dbl(); // recording new start of pacing
            EV << "Re-sending CON packet, ID: " << conPacket->getNid() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
            EV << "ACK Timeout for CON packet ID: " << conPacket->getNid() << " is: " << initTimeout << "s" << std::endl; // output a log message
            scheduleAt(simTime()+ spacing, timeoutEvent); // schedule a new timeout event based on spacing
        }
        else // if transmission is more than 4
        {
            EV << "Retransmission of CON packet, ID: " << currentID << " failed" << std::endl; // output a log message
            end_spacing = simTime().dbl(); // recording the end of spacing
            spacing = end_spacing - start_spacing; // calculating actual spacing
            loss_spacing = spacing; // recording spacing for lost transmission
            spacing = iw * spacing; // recalculating new spacing
            currentID = ++currentID; // incrementing current ID number
            Packet *conPacket = new Packet("CON"); // create next Packet to send
            conPacket->setNid(currentID); // setting NID to new current ID
            conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
            send(conPacket, "out"); // setting transmissionId to currentID and send the message out
            start_spacing = simTime().dbl(); // recording new start of pacing
            EV << "Sending next CON packet, ID: " << conPacket->getNid() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
            t1 = simTime().dbl(); // record the time the packet was sent
            retransmissionCounter = 0; // resetting retransmission counter
            EV << "ACK Timeout for CON packet ID: " << conPacket->getNid() << " is: " << initTimeout << "s" << std::endl; // output a log message
            scheduleAt(simTime()+ spacing, timeoutEvent); // schedule a new timeout event
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
                end_spacing = simTime().dbl(); // recording the end of spacing
                spacing = end_spacing - start_spacing; // calculating actual spacing
                if (spacing >= loss_spacing)
                    spacing = spacing - (dw * spacing);
                else
                    spacing = spacing - (fw * spacing);
                bubble("ID: OK"); // displaying a bubble
                EV << "ID from " << msg->getName() << " packet: OK" << std::endl; // output confirmation message
                cancelEvent(timeoutEvent); // canceling the timeout event
                t2 = simTime().dbl(); // record the time the packet was received
                rtt = t2 - t1; // calculate the round trip time
                currentID = ++currentID; // incrementing current ID number
                EV << "RTT for packet ID: " << ackPacket->getNid() << " was: " << rtt << "s" << std::endl; // output a log message
                download_speed = getDownloadSpeed(rtt, par("packet_size"));
                EV << "Download speed for packet ID: " << ackPacket->getNid() << " was: " << download_speed << "b/s" << std::endl; // output a log message
                delete msg; // delete the message
                Packet *conPacket = new Packet("CON"); // create next Packet to send
                conPacket->setNid(currentID); // setting NID to new current ID
                conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
                retransmissionCounter = 0; // resetting retransmission counter
                send(conPacket, "out"); // setting transmissionId to currentID and send the message out
                EV << "Sending next CON packet, ID: " << conPacket->getNid() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
                t1 = simTime().dbl(); // record the time the packet was sent
                start_spacing = simTime().dbl(); // recording new start of pacing
                std::setprecision(3); // limiting initTimeout to milliseconds
                EV << "ACK Timeout for CON packet ID: " << conPacket->getNid() << " is: " << initTimeout << "s" << std::endl; // output a log message
                scheduleAt(simTime()+ spacing, timeoutEvent); // schedule a new timeout event
            }
            else if (currentID != ackPacket->getNid()) // if ID from ackPacket is NOT the same with current ID
            {
                cancelEvent(timeoutEvent); // canceling the timeout event
                end_spacing = simTime().dbl(); // recording the end of spacing
                spacing = end_spacing - start_spacing; // calculating actual spacing
                loss_spacing = spacing; // recording spacing for lost transmission
                spacing = iw * spacing; // recalculating new spacing
                Packet *conPacket = new Packet("CON"); // create a new Packet to re-send
                conPacket->setNid(currentID); // setting NID to current ID
                conPacket->setBitLength(par("packet_size")); // changing size of packet to chosen size
                send(conPacket, "out"); // send the message out
                start_spacing = simTime().dbl(); // recording new start of pacing
                EV << "Re-sending CON packet, ID: " << conPacket->getNid() << ", size : " << conPacket->getBitLength() << "bit" << std::endl; // output a log message
                EV << "ACK Timeout for CON packet ID: " << conPacket->getNid() << " is: " << initTimeout << "s" << std::endl; // output a log message
                scheduleAt(simTime()+ spacing, timeoutEvent); // schedule a new timeout event
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

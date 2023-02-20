#include "emitter.h"

Define_Module(Emitter);  // register the module with OMNeT++

Emitter::Emitter() // constructor for the Emitter module
{
}

Emitter::~Emitter() // destructor for the Emitter module
{
}

void Emitter::initialize()  // function to initialize the Emitter module
{
    // initialize congestion control parameters
    cwnd = 1; // congestion window size
    ssthresh = 10; // slow start threshold
    numPacketsSent = 0; // number of packets sent so far
    numAcksReceived = 0; // number of acknowledgements received so far
    maxPacketsToSend = 20; // maximum number of packets to send
    timeout = 0.1; // retransmission timeout
    // schedule the start of packet transmission
    scheduleAt(simTime(), new cMessage("start"));
}

void Emitter::handleMessage(cMessage *msg) // function to handle incoming messages
{
    if (msg->isSelfMessage()) { // if the message is a self-message
        sendPacket(); // send a packet
        return; // exit the function
    }

    numAcksReceived++; // increment the number of acknowledgements received
    EV << "Number of ack received " << numAcksReceived << endl;

    if (cwnd < ssthresh) { // if the congestion window is less than the slow start threshold
        cwnd++; // increase the congestion window size by 1
    } else { // otherwise
        cwnd = cwnd + 1/cwnd; // increase the congestion window size by 1 divided by the current congestion window size
    }
    // if all packets have been sent and all acknowledgements have been received
    if (numPacketsSent == maxPacketsToSend && numAcksReceived == maxPacketsToSend) {
        endSimulation(); // end the simulation
    }
}

void Emitter::sendPacket()  // function to send a packet
{
    cPacket *pkt = new cPacket("data"); // create a new packet with the name "data"
    send(pkt, "out"); // send the packet out of the module's output gate
    numPacketsSent++; // increment the number of packets sent
    EV << "Number of packet sent " << numPacketsSent << endl;

    scheduleAt(simTime() + timeout, new cMessage("retransmission")); // schedule a retransmission timer
}

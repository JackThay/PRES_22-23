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
    // Initializing current sequence with a random number
    currentSeq = rand();
    // get the timeout value from the NED file
    timeout = par("timeout");
    // create a message to send
    timeoutEvent = new cMessage ("timeoutEvent");
    // create a message to send
    Packet *initPacket = new Packet("INIT");
    // setting sequence number for INIT packet
    initPacket->setNseq(currentSeq); // should be 0 if nothing went wrong
    // output a log message
    EV << "Sending out INIT packet, seqN: " << initPacket->getNseq() << std::endl;
    // schedule the message to be sent immediately
    scheduleAt(simTime(),initPacket);
    // record the time the packet was sent
    t1 = simTime().dbl();
    // schedule a timeout event
    scheduleAt(simTime() + timeout, timeoutEvent);
}

void Emitter::handleMessage(cMessage *msg)
{
    // if the message is the timeout event
    if(msg == timeoutEvent)
    {
        // output a log message
        EV << "Timeout expired, CON packet is probably lost.\n";
        // create a new message to re-send
        Packet *conPacket = new Packet("CON");
        // re-using current sequence number
        conPacket->setNseq(currentSeq);
        // output a log message
        EV << "Re-sending CON packet, seqN: " << conPacket->getNseq() << std::endl;
        // send the message out
        send(conPacket, "out");
        // schedule a new timeout event
        scheduleAt(simTime()+ timeout, timeoutEvent);
    }
    // if the message is not the timeout event
    else
    {
        // receiving ACK Packet
        Packet *ackPacket = check_and_cast<Packet *>(msg);
        // if seqN from ackPacket is the same with current seqN
        if (currentSeq == ackPacket->getNseq())
        {
            // output a log message
            EV << "Receiving " << msg->getName() << " packet, seqN: " << ackPacket->getNseq() << std::endl;
            // output confirmation message
            EV << "SeqN from " << msg->getName() << " packet: OK" << std::endl;
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
            // incrementing current sequence number
            currentSeq = ++currentSeq;
            // setting conPacket with the new current sequence number
            conPacket->setNseq(currentSeq);
            // output a log message
            EV << "Sending a new CON packet, seqN: " << conPacket->getNseq() << std::endl;
            // send the message out
            send(conPacket,"out");
            // record the time the packet was sent
            t1 = simTime().dbl();
            // schedule a new timeout event
            scheduleAt(simTime()+ timeout, timeoutEvent);
        }
        // if seqN from ackPacket is NOT the same with current seqN
        else
        {
            // output a log message
            EV << "Receiving " << msg->getName() << " packet, seqN: " << ackPacket->getNseq() << std::endl;
            // output confirmation message
            EV << "SeqN from " << msg->getName() << " packet: WRONG, previous CON packet is probably lost" << std::endl;
            // canceling the timeout event
            cancelEvent(timeoutEvent);
            // create a new message to re-send
            Packet *conPacket = new Packet("CON");
            // re-using current sequence number
            conPacket->setNseq(currentSeq);
            // output a log message
            EV << "Re-sending CON packet, seqN: " << conPacket->getNseq() << std::endl;
            // send the message out
            send(conPacket, "out");
            // schedule a new timeout event
            scheduleAt(simTime()+ timeout, timeoutEvent);
        }
    }
}

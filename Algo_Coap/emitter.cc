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
    // Initializing current ID with a random number
    currentID = rand();
    // Initializing current sequence number to 0
    currentSeq = 0;
    // get the timeout value from the NED file
    timeout = par("timeout");
    // create a message to send
    timeoutEvent = new cMessage ("timeoutEvent");
    // create a message to send
    Packet *initPacket = new Packet("INIT");
    // setting ID number for INIT packet
    initPacket->setNid(currentID);
    // setting sequence number for INIT packet
    initPacket->setNseq(currentSeq);
    // output a log message
    EV << "Sending out INIT packet, ID: " << initPacket->getNid() << " SeqN: " << initPacket->getNseq() << std::endl;
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
        // re-using current ID number
        conPacket->setNid(currentID);
        // incrementing sequence number
        currentSeq = ++currentSeq;
        // using new sequence number
        conPacket->setNseq(currentSeq);
        // output a log message
        EV << "Re-sending CON packet, ID: " << conPacket->getNid() << " SeqN: " << conPacket->getNseq() << std::endl;
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
        // if ID from ackPacket is the same with current ID
        if (currentID == ackPacket->getNid())
        {
            // output a log message
            EV << "Receiving " << msg->getName() << " packet, ID: " << ackPacket->getNid() << " SeqN: " << ackPacket->getNseq() << std::endl;
            // output confirmation message
            EV << "ID from " << msg->getName() << " packet: OK" << std::endl;
            // if sequence from ackPacket is the same with current sequence
            if (currentSeq == ackPacket->getNseq())
            {
                // output confirmation message
                EV << "SeqN from " << msg->getName() << " packet: OK" << std::endl;
                // displaying bubble
                bubble("ID and SeqN: OK!");
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
                // incrementing current ID number
                currentID = ++currentID;
                // resetting sequence number
                currentSeq = 0;
                // setting conPacket with the new current ID number
                conPacket->setNid(currentID);
                // setting conPacket with the new current sequence number
                conPacket->setNseq(currentSeq);
                // output a log message
                EV << "Sending a new CON packet, ID: " << conPacket->getNid() << " SeqN: " << conPacket->getNseq() << std::endl;
                // send the message out
                send(conPacket,"out");
                // record the time the packet was sent
                t1 = simTime().dbl();
                // schedule a new timeout event
                scheduleAt(simTime()+ timeout, timeoutEvent);
            }

            else
            {
                EV << "SeqN from " << msg->getName() << " packet: WRONG, previous CON packet is probably lost" << std::endl;
                // displaying bubble
                bubble("SeqN: WRONG!");
                // canceling the timeout event
                cancelEvent(timeoutEvent);
                // create a new message to re-send
                Packet *conPacket = new Packet("CON");
                // re-using current ID number
                conPacket->setNid(currentID);
                // incrementing sequence number
                currentSeq = ++currentSeq;
                // using new sequence number
                conPacket->setNseq(currentSeq);
                // output a log message
                EV << "Re-sending CON packet, ID: " << conPacket->getNid() << " SeqN: " << conPacket->getNseq() << std::endl;
                // send the message out
                send(conPacket, "out");
                // schedule a new timeout event
                scheduleAt(simTime()+ timeout, timeoutEvent);
            }
        }
        // if ID from ackPacket is NOT the same with current ID
        else
        {
            // output a log message
            EV << "Receiving " << msg->getName() << " packet, ID: " << ackPacket->getNid() << std::endl;
            // output confirmation message
            EV << "ID from " << msg->getName() << " packet: WRONG, previous CON packet is probably lost" << std::endl;
            // displaying bubble
            bubble("ID : WRONG!");
            // canceling the timeout event
            cancelEvent(timeoutEvent);
            // create a new message to re-send
            // create a new message to re-send
            Packet *conPacket = new Packet("CON");
            // re-using current ID number
            conPacket->setNid(currentID);
            // incrementing sequence number
            currentSeq = ++currentSeq;
            // using new sequence number
            conPacket->setNid(currentSeq);
            // output a log message
            EV << "Re-sending CON packet, ID: " << conPacket->getNid() << " SeqN: " << conPacket->getNseq() << std::endl;
            // send the message out
            send(conPacket, "out");
            // schedule a new timeout event
            scheduleAt(simTime()+ timeout, timeoutEvent);
        }
    }
}

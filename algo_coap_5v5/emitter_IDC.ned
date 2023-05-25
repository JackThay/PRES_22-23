simple Emitter_IDC
{
    // Declaration of parameters
    parameters:
        int packet_size @unit(bps); // packet size for the experiment
        // Declaration of display string for visualization
        @display("i=device/pocketpc");
    // Declaration of input and output gates
    gates:
        input in;
        output out;
}

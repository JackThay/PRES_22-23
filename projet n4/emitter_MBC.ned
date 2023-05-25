simple Emitter_MBC
{
    // Declaration of parameters
    parameters:
        int packet_size @unit(bps); // packet size for the experiment
        int m = default(8); // Size of the sliding window
        double timeout = default(1s) @unit(s); // Timeout value
        // Declaration of display string for visualization
        @display("i=device/pocketpc");
    // Declaration of input and output gates
    gates:
        input in;
        output out;
}

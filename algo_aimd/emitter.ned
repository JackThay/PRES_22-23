simple Emitter
{
    // Declaration of parameters
    parameters:
        int packet_size; // packet size for the experiment
        double incrementalWeight;
        double decrementalWeight;
        double fastDecrementalWeight;
        double initialSpacing;
        double rto @unit(s);
        // Declaration of display string for visualization
        @display("i=device/pocketpc");
    // Declaration of input and output gates
    gates:
        input in;
        output out;
}

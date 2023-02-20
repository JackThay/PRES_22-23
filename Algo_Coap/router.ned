simple Router
{
    // Define the parameters that can be set in the NED file
    parameters:
        double dropping_probability; // The probability of dropping packets that exceed the queue size
        int queue_size; // The maximum size of the queue
    // Define the module's appearance in the NED file
    @display("i=abstract/router");
    // Define the module's gates
    gates:
        input in_e; // The input gate for emitter packets
        output out_e; // The output gate for emitter packets
        input in_r; // The input gate for receiver packets
        output out_r; // The output gate for receiver packets
}

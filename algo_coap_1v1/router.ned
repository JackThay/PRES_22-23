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
        input in_ePort[]; 
        output out_ePort[];
        input in_rPort[]; 
        output out_rPort[];

}

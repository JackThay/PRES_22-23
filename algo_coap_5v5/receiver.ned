simple Receiver
{
    parameters:
        // Declare a parameter named "delai" with the default value of 0.
        volatile double delai = default(0);
    // Specify the display string for the module.
    @display("i=device/pc");
    // Declare the input and output gates for the module.
    gates:
        input in;
        output out;
}

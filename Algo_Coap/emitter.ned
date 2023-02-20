simple Emitter
{
    parameters:
        double timeout @unit(s);
    @display("i=device/pc");
    gates:
        input in;
        output out;
}

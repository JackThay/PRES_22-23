simple Router
{
    parameters:
        double dropping_probability @unit(s);
    @display("i=abstract/router");
    gates:
        input in_e;
        output out_e;
        input in_r;
        output out_r;
}

import ned.DelayChannel;

// Defining the network Reseau
network Reseau
{
    // Defining the submodules of the network
    submodules:
        // Defining an Emitter module with display position (43,48)
        emitter: Emitter {
            @display("p=43,48");
        }
        // Defining a Receiver module with display position (410,48)
        receiver: Receiver {
            @display("p=410,48");
        }
        // Defining a Router module with display position (241,214)
        router: Router {
            @display("p=241,214");
        }
    // Defining the connections between the submodules
    connections:
        emitter.out --> DelayChannel {  delay = 0.1s; } --> router.in_e;
        router.out_e --> DelayChannel {  delay = 0.1s; } --> receiver.in;
        receiver.out --> DelayChannel {  delay = 0.1s; } --> router.in_r;
        router.out_r --> DelayChannel {  delay = 0.1s; } --> emitter.in;
}

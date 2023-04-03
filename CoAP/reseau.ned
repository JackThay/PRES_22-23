import ned.DelayChannel;

// Defining the network Reseau
network Reseau
{
    // Defining the submodules of the network
    submodules:
        // Defining an Emitter module with display position (43,48)
        IoT: Emitter {
            @display("p=43,48;i=device/pocketpc");
        }
        // Defining a Receiver module with display position (410,48)
        Server: Receiver {
            @display("p=410,48;i=device/server2");
        }
        // Defining a Router module with display position (241,214)
        Router: Router {
            @display("p=241,214");
        }
    // Defining the connections between the submodules
    connections:
        IoT.out --> DelayChannel {  delay = 0.1s; } --> Router.in_e;
        Router.out_e --> DelayChannel {  delay = 0.1s; } --> Server.in;
        Server.out --> DelayChannel {  delay = 0.1s; } --> Router.in_r;
        Router.out_r --> DelayChannel {  delay = 0.1s; } --> IoT.in;
}

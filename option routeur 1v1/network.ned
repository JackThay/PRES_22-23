import ned.DatarateChannel;
import ned.DelayChannel;

// Defining the network Reseau
network Network
{
    // Declaration of parameters
    parameters:
        int bandwidth @unit(bps); // packet size for the experiment
    // Defining the submodules of the network
    submodules:
        // Defining an Emitter module with display position (43,48)
        IoT0: Emitter {
            @display("p=43,48;i=device/pocketpc");
        }
        // Defining a Receiver module with display position (410,48)
        Server0: Receiver {
            @display("p=410,48;i=device/server2");
        }
        // Defining a Router module with display position (241,214)
        Router: Router {
            @display("p=241,214");
            gates:
                in_ePort[1];
                out_ePort[1];
                in_rPort[1];
                out_rPort[1];
        }
    // Defining the connections between the submodules
    connections:
        IoT0.out --> DatarateChannel {  delay = default(0.1s); datarate = parent.bandwidth; } --> Router.in_ePort[0];
        Router.out_ePort[0] --> DatarateChannel {  delay = default(0.1s); datarate = parent.bandwidth; } --> Server0.in;
        Server0.out --> DatarateChannel {  delay = default(0.1s); datarate = parent.bandwidth; } --> Router.in_rPort[0];
        Router.out_rPort[0] --> DatarateChannel {  delay = default(0.1s); datarate = parent.bandwidth; } --> IoT0.in;
}

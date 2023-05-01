import ned.DatarateChannel;
import ned.DelayChannel;

// Defining the network Reseau
network Network
{
    // Declaration of parameters
    parameters:
        int bandwidth; // packet size for the experiment
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
                in_ePort[5];
                out_ePort[5];
                in_rPort[5];
                out_rPort[5];
        }
        IoT1: Emitter {
            @display("p=43,142");
        }
        IoT2: Emitter {
            @display("p=45,240");
        }
        IoT3: Emitter {
            @display("p=46,327");
        }
        IoT4: Emitter {
            @display("p=46,412");
        }
        Server1: Receiver {
            @display("p=408,140");
        }
        Server2: Receiver {
            @display("p=406,238");
        }
        Server3: Receiver {
            @display("p=406,327");
        }
        Server4: Receiver {
            @display("p=405,424");
        }
    // Defining the connections between the submodules
    connections:
        IoT0.out --> DatarateChannel {  delay = default(0.1s);datarate = default(10000bps); } --> Router.in_ePort[0];
        IoT1.out --> DatarateChannel {  delay = default(0.1s);datarate = default(10000bps); } --> Router.in_ePort[1];
        IoT2.out --> DatarateChannel {  delay = default(0.1s);datarate = default(10000bps); } --> Router.in_ePort[2];
        IoT3.out --> DatarateChannel {  delay = default(0.1s);datarate = default(10000bps); } --> Router.in_ePort[3];
        IoT4.out --> DatarateChannel {  delay = default(0.1s);datarate = default(10000bps); } --> Router.in_ePort[4];
        Router.out_ePort[0] --> DatarateChannel {  delay = default(0.1s);datarate = default(10000bps); } --> Server0.in;
        Router.out_ePort[1] --> DatarateChannel {  delay = default(0.1s);datarate = default(10000bps); } --> Server1.in;
        Router.out_ePort[2] --> DatarateChannel {  delay = default(0.1s);datarate = default(10000bps); } --> Server2.in;
        Router.out_ePort[3] --> DatarateChannel {  delay = default(0.1s);datarate = default(10000bps); } --> Server3.in;
        Router.out_ePort[4] --> DatarateChannel {  delay = default(0.1s);datarate = default(10000bps); } --> Server4.in;
        Server0.out --> DatarateChannel {  delay = default(0.1s);datarate = default(10000bps); } --> Router.in_rPort[0];
        Server1.out --> DatarateChannel {  delay = default(0.1s);datarate = default(10000bps); } --> Router.in_rPort[1];
        Server2.out --> DatarateChannel {  delay = default(0.1s);datarate = default(10000bps); } --> Router.in_rPort[2];
        Server3.out --> DatarateChannel {  delay = default(0.1s);datarate = default(10000bps); } --> Router.in_rPort[3];
        Server4.out --> DatarateChannel {  delay = default(0.1s);datarate = default(10000bps); } --> Router.in_rPort[4];
        Router.out_rPort[0] --> DatarateChannel {  delay = default(0.1s);datarate = default(10000bps); } --> IoT0.in;
        Router.out_rPort[1] --> DatarateChannel {  delay = default(0.1s);datarate = default(10000bps); } --> IoT1.in;
        Router.out_rPort[2] --> DatarateChannel {  delay = default(0.1s);datarate = default(10000bps); } --> IoT2.in;
        Router.out_rPort[3] --> DatarateChannel {  delay = default(0.1s);datarate = default(10000bps); } --> IoT3.in;
        Router.out_rPort[4] --> DatarateChannel {  delay = default(0.1s);datarate = default(10000bps); } --> IoT4.in;
}

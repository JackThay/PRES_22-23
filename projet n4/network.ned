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
        // IoT0 is used for testing the congestion control algorithms, other IoT will be using regular CoAP
        // This is to ensure every network environnement stays the same between each simulations
        // change IoT0 according to the test you're planning:
        // Regular CoAP -> Emitter_COAP
        // IDC-CoAP -> Emitter_IDC
        // MBC-CoAP -> Emitter_MBC
        // Generic -> Emitter (generic emitter with no console message)
        
        // Defining an Emitter module with display position (43,48)
        IoT0: Emitter_COAP { // <== This is where you change the emitter
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
        IoT0.out --> DatarateChannel {  delay = default(0.1s);datarate = parent.bandwidth; } --> Router.in_ePort[0];
        IoT1.out --> DatarateChannel {  delay = default(0.1s);datarate = parent.bandwidth; } --> Router.in_ePort[1];
        IoT2.out --> DatarateChannel {  delay = default(0.1s);datarate = parent.bandwidth; } --> Router.in_ePort[2];
        IoT3.out --> DatarateChannel {  delay = default(0.1s);datarate = parent.bandwidth; } --> Router.in_ePort[3];
        IoT4.out --> DatarateChannel {  delay = default(0.1s);datarate = parent.bandwidth; } --> Router.in_ePort[4];
        Router.out_ePort[0] --> DatarateChannel {  delay = default(0.1s);datarate = parent.bandwidth; } --> Server0.in;
        Router.out_ePort[1] --> DatarateChannel {  delay = default(0.1s);datarate = parent.bandwidth; } --> Server1.in;
        Router.out_ePort[2] --> DatarateChannel {  delay = default(0.1s);datarate = parent.bandwidth; } --> Server2.in;
        Router.out_ePort[3] --> DatarateChannel {  delay = default(0.1s);datarate = parent.bandwidth; } --> Server3.in;
        Router.out_ePort[4] --> DatarateChannel {  delay = default(0.1s);datarate = parent.bandwidth; } --> Server4.in;
        Server0.out --> DatarateChannel {  delay = default(0.1s);datarate = parent.bandwidth; } --> Router.in_rPort[0];
        Server1.out --> DatarateChannel {  delay = default(0.1s);datarate = parent.bandwidth; } --> Router.in_rPort[1];
        Server2.out --> DatarateChannel {  delay = default(0.1s);datarate = parent.bandwidth; } --> Router.in_rPort[2];
        Server3.out --> DatarateChannel {  delay = default(0.1s);datarate = parent.bandwidth; } --> Router.in_rPort[3];
        Server4.out --> DatarateChannel {  delay = default(0.1s);datarate = parent.bandwidth; } --> Router.in_rPort[4];
        Router.out_rPort[0] --> DatarateChannel {  delay = default(0.1s);datarate = parent.bandwidth; } --> IoT0.in;
        Router.out_rPort[1] --> DatarateChannel {  delay = default(0.1s);datarate = parent.bandwidth; } --> IoT1.in;
        Router.out_rPort[2] --> DatarateChannel {  delay = default(0.1s);datarate = parent.bandwidth; } --> IoT2.in;
        Router.out_rPort[3] --> DatarateChannel {  delay = default(0.1s);datarate = parent.bandwidth; } --> IoT3.in;
        Router.out_rPort[4] --> DatarateChannel {  delay = default(0.1s);datarate = parent.bandwidth; } --> IoT4.in;
}

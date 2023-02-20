import ned.DelayChannel;


network Reseau
{
    submodules:
        emitter: Emitter {
            @display("p=43,48");
        }
        receiver: Receiver {
            @display("p=410,48");
        }
        router: Router {
            @display("p=241,214");
        }
    connections:
        emitter.out --> DelayChannel {  delay = 0.1s; } --> router.in_e;
        router.out_e --> DelayChannel {  delay = 0.1s; } --> receiver.in;
        receiver.out --> DelayChannel {  delay = 0.1s; } --> router.in_r;
        router.out_r --> DelayChannel {  delay = 0.1s; } --> emitter.in;
}

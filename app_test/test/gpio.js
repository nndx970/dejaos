import { gpio } from "dxlib";

gpio.initGpio();
gpio.initGpio();
gpio.initGpio();
gpio.initGpio();

let status = 1;
setInterval(() => {
    gpio.setRelayStatus(status);
    status = status === 1 ? 0 : 1;
}, 1000);

console.log("test");

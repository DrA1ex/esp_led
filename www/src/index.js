import {DEFAULT_ADDRESS} from "./constants.js";
import {Application} from "./application.js";

if ("serviceWorker" in navigator) {
    try {
        const registration = await navigator.serviceWorker.register("./service_worker.js");
        console.log("ServiceWorker registration successful with scope: ", registration.scope);
    } catch (e) {
        console.log("ServiceWorker registration failed: ", e);
    }
}

const params = new Proxy(new URLSearchParams(window.location.search), {
    get: (searchParams, prop) => searchParams.get(prop),
});

const protocol = window.location.protocol === "https:" ? "wss" : "ws";
const host = params.host ?? window.location.hostname;
const gateway = `${protocol}://${host !== "localhost" ? host : DEFAULT_ADDRESS}/ws`;

const StatusElement = document.getElementById("status");

const app = new Application(gateway);
app.subscribe(this, app.Event.Connected, () => {
    StatusElement.style.visibility = "collapse";
});

app.subscribe(this, app.Event.Disconnected, () => {
    StatusElement.innerText = "NOT CONNECTED";
    StatusElement.style.visibility = "visible";
});

await app.begin(document.body);
import {DEFAULT_ADDRESS} from "./constants.js";
import {Application} from "./application.js";

function urlCombine(base, path, ...parts) {
    const result = new URL(path, base).href;
    if (!parts.length) return result;

    return urlCombine(result, parts.shift(), ...parts);
}

function updateManifest(json) {
    for (const [key, value] of Object.entries(json)) {
        if (key === "src") json[key] = urlCombine(window.location.origin, window.location.pathname, value);
        else if (value instanceof Array) value.forEach(v => updateManifest(v));
        else if (value instanceof Object) updateManifest(value);
    }
}

try {
    const response = await fetch("./site.webmanifest");
    if (!response.ok) console.error("Unable to load manifest:", response.statusText);

    const json = await response.json();
    updateManifest(json);
    json.start_url = window.location.href;

    const link = document.createElement("link");
    link.rel = "manifest";
    link.href = "data:application/manifest+json;charset=utf-8," + encodeURIComponent(JSON.stringify(json, null, 2));

    document.head.appendChild(link);
} catch (e) {
    console.error("Unable to load manifest", e);
}

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
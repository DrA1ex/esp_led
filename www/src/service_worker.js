const CACHE_KEY = "cache";

const URL_TO_CACHE = [
    "./",
    "./index.js",
    "./style.css",
];

self.addEventListener("install", (event) => {
    event.waitUntil(_install());
});

self.addEventListener("fetch", function (event) {
    if (event.request.method !== "GET") return;

    event.respondWith(_fetch(event));
});

async function _install() {
    const cache = await caches.open(CACHE_KEY);
    return cache.addAll(URL_TO_CACHE);
}

async function _fetch(event) {
    const cache = await caches.open(CACHE_KEY);

    const networkResponse = _cacheFetch(cache, event.request);
    const cachedResponse = await cache.match(event.request);

    if (cachedResponse?.ok) {
        event.waitUntil(networkResponse);
        console.log("Get from cache:", cachedResponse.url);
        return cachedResponse;
    } else {
        return await networkResponse;
    }
}

async function _cacheFetch(cache, request) {
    const networkResponse = await fetch(request);

    if (networkResponse.ok) {
        console.log("Update cache:", request.url);
        await cache.put(request, networkResponse.clone());
    } else {
        console.log("Failed to update cache:", networkResponse.statusText);
    }

    return networkResponse;
}
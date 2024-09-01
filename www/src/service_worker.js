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

    const networkResponse = await _cacheFetch(cache, event.request);
    if (networkResponse?.ok) return networkResponse;

    const cachedResponse = await cache.match(event.request);
    if (cachedResponse?.ok) {
        console.log("Get from cache:", cachedResponse.url);
        return cachedResponse;
    }

    return networkResponse;
}

async function _cacheFetch(cache, request) {
    const networkResponse = await fetch(request).catch((e) => {
        console.error("Network request failed");
        return {ok: false, error: e};
    });

    if (!networkResponse.ok) return networkResponse;

    const cacheMatch = await cache.match(request);
    if (cacheMatch) {
        const cachedLastModified = cacheMatch.headers.get('Last-Modified');
        const networkLastModified = networkResponse.headers.get('Last-Modified');

        if (networkLastModified && (!cachedLastModified || new Date(networkLastModified) > new Date(cachedLastModified))) {
            console.log("Update cache:", request.url);
            await cache.put(request, networkResponse.clone());
        }
    } else {
        console.log("Caching new response:", request.url);
        await cache.put(request, networkResponse.clone());
    }

    return networkResponse;
}
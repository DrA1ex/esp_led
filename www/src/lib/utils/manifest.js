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

export async function makeManifest(url) {
    try {
        const response = await fetch(url);
        if (!response.ok) return console.error("Unable to load manifest:", response.statusText);

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
}
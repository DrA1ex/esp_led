export function parseHexColor(color) {
    const r = parseInt(color.substring(1, 3), 16);
    const g = parseInt(color.substring(3, 5), 16);
    const b = parseInt(color.substring(5, 7), 16);

    return 0xff | (r << 24) | (g << 16) | (b << 8);
}

export function rgbToHex(color) {
    const r = (color >> 24) & 0xff;
    const g = (color >> 16) & 0xff;
    const b = (color >> 8) & 0xff;

    return `#${[r, g, b].map(c => c.toString(16).padStart(2, "0")).join("")}`;
}
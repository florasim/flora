"use strict";

var WeakMap = require("weak-map");

var hashes = new WeakMap();

module.exports = hash;
function hash(value) {
    if (value && typeof value.hash === "function") {
        return value.hash();
    } else if (value && typeof value === "object") {
        var hash = hashes.get(value);
        if (!hash) {
            hash = (Math.random() * 0xFFFFFFFF) >>> 0;
            hashes.set(value, hash);
        }
        return hash;
    } else {
        return value;
    }
}


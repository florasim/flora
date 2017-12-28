"use strict";

var clearObject = require("./object");

module.exports = popClear;
function popClear(object) {
    if (Array.isArray(object)) {
        object.length = 0;
    } else if (typeof object.clear === "function") {
        object.clear();
    } else if (object && typeof object === "object") {
        clearObject(object);
    } else {
        throw new TypeError("Can't clear " + object);
    }
}


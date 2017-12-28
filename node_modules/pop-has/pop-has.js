"use strict";

var objectHas = require("./has");

module.exports = has;
function has(object, soughtValue, equals) {
    if (object && typeof object.has === "function") {
        return object.has(soughtValue, equals);
    } else {
        return objectHas(object, soughtValue, equals);
    }
}

"use strict";

var swap = require("./swap");

module.exports = polymorphicSwap;
function polymorphicSwap(array, start, minusLength, plus) {
    if (typeof array.swap === "function") {
        array.swap(start, minusLength, plus);
    } else {
        swap(array, start, minusLength, plus);
    }
}


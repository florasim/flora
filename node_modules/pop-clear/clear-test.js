"use strict";

module.exports = clearObject;
function clearObject(object) {
    for (var name in object) {
        delete object[name];
    }
}


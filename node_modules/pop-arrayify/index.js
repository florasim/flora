"use strict";

module.exports = arrayify;
function arrayify(object) {
    var result, index, length;
    if (!object) {
        return [];
    } else if (Array.isArray(object)) {
        return object.slice();
    } else if (typeof object.toArray === "function") {
        return object.toArray();
    } else if (typeof object.forEach === "function") {
        result = [];
        object.forEach(result.push, result);
        return result;
    } else if (typeof object.length === "number") {
        result = [];
        for (index = 0, length = object.length; index < length; index++) {
            result[index] = object[index];
        }
        return result;
    } else {
        throw new Error("Can't convert to an array " + object);
    }
}

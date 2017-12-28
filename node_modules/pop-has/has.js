"use strict";

var equalsOperator = require("pop-equals");
var hasOwnProperty = Object.prototype.hasOwnProperty;

module.exports = has;
function has(object, soughtValue, equals) {
    equals = equals || equalsOperator;
    for (var key in object) {
        if (hasOwnProperty.call(object, key)) {
            var value = object[key];
            if (equals(soughtValue, value)) {
                return true;
            }
        }
    }
    return false;
}

'use strict';

var unzip = require('./unzip');

module.exports = zip;
function zip() {
    // Unrolled arguments to array to avoid V8 deoptimization of functions that
    // pass the arguments object to other functions.
    var args = [];
    for (var index = 0, length = arguments.length; index < length; index++) {
        args[index] = arguments[index];
    }
    return unzip(args);
}

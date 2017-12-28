'use strict';

var unzip = require('./unzip');

// Polymorphic unzip uses collection.toArray() (for non-array collection
// implementations) to convert the table or any of its rows into array before
// passing them along to the non-polymorphic unzip.

module.exports = popUnzip;
function popUnzip(table) {
    if (typeof table.unzip === 'function') {
        return table.unzip();
    }
    // Ensure that the table we pass to the non-polymorphic unzip is an array
    // of arrays.
    // However, only construct a new table if necessary.
    var arrayTable;
    if (!Array.isArray(table)) {
        table = arrayTable = table.toArray();
    }
    for (var index = 0, length = table.length; index < length; index++) {
        var row = table[index];
        if (!Array.isArray(row)) {
            // Construct a copy of the table in which to replace non-array
            // values.
            if (!arrayTable) {
                // Table is known to be an array because we would have replaced
                // it already otherwise.
                arrayTable = table.slice();
            }
            arrayTable[index] = row.toArray();
        }
    }
    return unzip(arrayTable || table);
}

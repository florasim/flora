'use strict';

// Unzip is also known as a matrix transpose, operating exclusively on arrays.

module.exports = unzip;
function unzip(table) {
    var transpose = [];
    var rows = table.length;
    var row, columns, length;
    var index, jndex;

    // Mathematically, the degenerate case is an empty array where each inner
    // value would be of infinite length.
    if (!rows) {
        // Within this array, the nothingness is infinite.
        return [];
    }

    columns = table[0].length;
    length = Infinity;

    // Find the shortest row, this will be the length of the transpose.
    for (index = 0; index < rows; index++) {
        row = table[index];
        if (row.length < length) {
            length = row.length;
        }
    }

    // Populate the transpose.
    for (index = 0; index < length; index++) {
        row = transpose[index] = [];
        for (jndex = 0; jndex < rows; jndex++) {
            row[jndex] = table[jndex][index];
        }
    }

    return transpose;
}

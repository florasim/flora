'use strict';

var zip = require("../pop-zip");

function Fake(array) {
    this.array = array;
}

Fake.prototype.toArray = function () {
    return this.array;
};

it("accepts a fake row", function () {
    expect(zip(
        ['a', 'b', 'c'],
        new Fake([1, 2, 3]),
        ['x', 'y', 'z']
    )).toEqual([
        ['a', 1, 'x'],
        ['b', 2, 'y'],
        ['c', 3, 'z']
    ]);
});


'use strict';

var unzip = require('../pop-unzip');

function Fake(array) {
    this.array = array;
}

Fake.prototype.toArray = function () {
    return this.array;
};

it('accepts a fake table', function () {
    expect(unzip(new Fake([
        ['a', 'b', 'c'],
        [1, 2, 3],
        ['x', 'y', 'z']
    ]))).toEqual([
        ['a', 1, 'x'],
        ['b', 2, 'y'],
        ['c', 3, 'z']
    ]);
});

it('accepts a fake row', function () {
    expect(unzip([
        ['a', 'b', 'c'],
        new Fake([1, 2, 3]),
        ['x', 'y', 'z']
    ])).toEqual([
        ['a', 1, 'x'],
        ['b', 2, 'y'],
        ['c', 3, 'z']
    ]);
});

it('accepts fake tables and rows', function () {
    expect(unzip(new Fake([
        ['a', 'b', 'c'],
        new Fake([1, 2, 3]),
        ['x', 'y', 'z']
    ]))).toEqual([
        ['a', 1, 'x'],
        ['b', 2, 'y'],
        ['c', 3, 'z']
    ]);
});

it('accepts exclusively fake tables and rows', function () {
    expect(unzip(new Fake([
        new Fake(['a', 'b', 'c']),
        new Fake([1, 2, 3]),
        new Fake(['x', 'y', 'z'])
    ]))).toEqual([
        ['a', 1, 'x'],
        ['b', 2, 'y'],
        ['c', 3, 'z']
    ]);
});

it('accepts objects that implement unzip', function () {
    expect(unzip({
        unzip: function () {
            return 'bogus';
        }
    })).toBe('bogus');
});

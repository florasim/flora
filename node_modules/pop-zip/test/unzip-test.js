'use strict';

var unzip = require('../unzip');
var cases = require('./cases');

it('unzips (illustrative)', function () {
    expect(unzip([
        ['a', 'b', 'c'],
        [1, 2, 3],
        ['x', 'y', 'z']
    ])).toEqual([
        ['a', 1, 'x'],
        ['b', 2, 'y'],
        ['c', 3, 'z']
    ]);
});


Object.keys(cases).forEach(function (name) {
    var test = cases[name];
    it(name, function () {
        expect(unzip(test.input)).toEqual(test.output);
    });
});

'use strict';

var zip = require('../zip');
var cases = require('./cases');

it('zips (illustrative)', function () {
    expect(zip(
        ['a', 'b', 'c'],
        [1, 2, 3],
        ['x', 'y', 'z']
    )).toEqual([
        ['a', 1, 'x'],
        ['b', 2, 'y'],
        ['c', 3, 'z']
    ]);
});

Object.keys(cases).forEach(function (name) {
    var test = cases[name];
    it(name, function () {
        expect(zip.apply(this, test.input)).toEqual(test.output);
    });
});

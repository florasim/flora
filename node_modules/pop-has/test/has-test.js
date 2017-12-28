"use strict";

var has = require("../has");

it("checks array values", function () {
    expect(has([1, 2, 3], 2)).toBe(true);
    expect(has([[1], [2], [3]], [2])).toBe(true);
});

it("can use alternate equality", function () {
    function is(x, y) {
        // The latter term is for NaN is NaN
        return x === y || (x !== x && y !== y);
    }
    expect(has([{}], {}, is)).toBe(false);
    expect(has([1, 2, NaN], NaN, is)).toBe(true);
});

it("checks object values", function () {
    expect(has({a: 10}, 10)).toBe(true);
    expect(has({a: [10]}, [10])).toBe(true);
    expect(has([], 10)).toBe(false);
});


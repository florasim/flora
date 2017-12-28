"use strict";

var clear = require("../index");

it("clears an object", function () {
    var object = {a: 10, b: 20, c: 30};
    clear(object);
    expect(object).toEqual({});
});

it("clears an array", function () {
    var array = [1, 2, 3];
    clear(array);
    expect(array).toEqual([]);
});

it("clears an instance", function () {
    var called;
    var spy = function () {called = true;};
    var object = {clear: spy};
    clear(object);
    expect(called).toBe(true);
});


"use strict";

var arrayify = require("../index");

it("supports arrays", function () {
    var array = [1, 2, 3];
    var arrayed = arrayify(array);
    expect(arrayed).not.toBe(array);
    expect(arrayed).toEqual(array);
});

it("supports arrays with holes", function () {
    var array = [1,, 2,, 3];
    var arrayed = arrayify(array);
    expect(arrayed).not.toBe(array);
    expect(arrayed).toEqual(array);
});

it("supports objects with toArray", function () {
    expect(arrayify({
        toArray: function () {
            return [1, 2, 3];
        }
    })).toEqual([1, 2, 3]);
});

it("supports objects with forEach methods", function () {
    expect(arrayify({
        forEach: function (callback, thisp) {
            callback.call(thisp, 1);
            callback.call(thisp, 2);
            callback.call(thisp, 3);
        }
    })).toEqual([1, 2, 3]);
});

it("supports array-like objects", function () {
    expect(arrayify({
        length: 3,
        0: 1,
        1: 2,
        2: 3
    })).toEqual([1, 2, 3]);
});

it("coerces null to an empty array", function () {
    expect(arrayify(null)).toEqual([]);
});

it("does not support anything else", function () {
    expect(function () {
        arrayify({});
    }).toThrow();
});

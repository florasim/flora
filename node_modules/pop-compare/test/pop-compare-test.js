"use strict";

// Just in case this was shimmed
Array.prototype.compare = null;

var compare = require("../pop-compare");
var start = new Date();

it("compares numbers", function () {
    expect(compare(0, 10)).toBe(-10);
    expect(compare(5, -5)).toBe(10);
    expect(compare(NaN, NaN)).toBe(0);
    expect(compare(-Infinity, Infinity)).toBe(-Infinity);
    expect(compare(Infinity, -Infinity)).toBe(Infinity);
});

it("compares boxed numbers", function () {
    expect(compare(new Number(0), 10)).toBe(-10);
    expect(compare(new Number(0), new Number(10))).toBe(-10);
    expect(compare(0, new Number(10))).toBe(-10);
});

it("compares strings", function () {
    expect(compare("abc", "abc")).toBe(0);
    expect(compare("a", "b")).toBe(-Infinity);
    expect(compare("b", "a")).toBe(Infinity);
});

it("compares string is less than prefixed string", function () {
    expect(compare("ab", "abc")).toBe(-Infinity);
    expect(compare("abc", "ab")).toBe(Infinity);
});

it("compares dates", function () {
    expect(compare(start, start)).toBe(0);
    expect(compare(start, new Date()) <= 0).toBe(true);
    expect(compare(start, Date.now()) <= 0).toBe(true);
});

describe("arrays", function () {

    it("compares equivalent arrays", function () {
        expect(compare([], [])).toBe(0);
        expect(compare([1], [])).toBe(Infinity);
        expect(compare([], [1])).toBe(-Infinity);
    });

    it("compares arrays with holes", function () {
        expect(compare([,], [,])).toBe(0);
        expect(compare([,], [,,])).toBe(-1);
        expect(compare([,,,], [,,])).toBe(1);
        expect(compare([0,], [,])).toBe(Infinity);
        expect(compare([,], [0,])).toBe(-Infinity);
    });

    describe("compares nested arrays", function () {
        it("compares respective values", function () {
            expect(compare([[]], [[]])).toBe(0);
            expect(compare([[1]], [[0]])).toBe(1);
            expect(compare([[1]], [[1]])).toBe(0);
            expect(compare([[0]], [[1]])).toBe(-1);
        });

        it("compares values to holes", function () {
            expect(compare([[1]], [[]])).toBe(Infinity);
            expect(compare([[]], [[1]])).toBe(-Infinity);
        });
    });

    it("compares arrays with reference cycles", function () {
        var array = [];
        array.push(array);
        expect(compare(array, array)).toBe(0);
    });

    it("can not compare equivalent arrays with reference cycles", function () {
        expect(function () {
            var a = []; a.push(a);
            var b = []; b.push(b);
            expect(compare(a, b)).toBe(0);
        }).toThrow();
    });

});

it("incompatible types are incomparable", function () {
    expect(compare([], {})).toBe(0);
    expect(compare("", {})).toBe(0);
});

describe("polymorphism", function () {

    it("uses left hand side", function () {
        var fake = {
            compare: function (other, subCompare) {
                expect(other).toBe(undefined);
                expect(subCompare).toBe(compare);
                return -1;
            }
        };
        expect(compare(fake)).toBe(-1);
    });

    it("uses left hand side", function () {
        var fake = {
            compare: function (other, subCompare) {
                expect(other).toBe(null);
                expect(subCompare).toBe(compare);
                return -1;
            }
        };
        expect(compare(null, fake)).toBe(1);
    });

});

it("allows the nested comparison to be overridden", function () {
    function dontcare() {
        return 0;
    }
    expect(compare([10], [20])).toBe(-10);
    expect(compare([10], [20], dontcare)).toBe(0);
});


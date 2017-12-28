
var O = require("../index");
var swap = require("pop-swap");

it("observes the length of an array", function () {
    var array = [];
    var observer = O.observePropertyChange(array, "length", function (length) {
        expect(length).toBe(1);
    });
    array.push(10);
    observer.cancel();
});

it("observes property of an object", function () {
    var object = {weight: 10};
    var observer = O.observePropertyChange(object, "weight", function (weight) {
        expect(weight).toBe(20);
    });
    object.weight = 20;
    observer.cancel();
});

it("observes values at indexes", function () {
    var array = [];
    var change;
    var handler = {
        handlePropertyChange: function (plus, minus, index, object) {
            change = {
                plus: plus,
                minus: minus,
                index: index,
                object: object
            };
        }
    };
    var observer0 = O.observePropertyChange(array, 0, handler);
    var observer1 = O.observePropertyChange(array, 1, handler);
    var observer2 = O.observePropertyChange(array, 2, handler);

    array.set(0, 10);
    expect(change).toEqual({plus: 10, minus: undefined, index: 0, object: array});

    array.set(0, 20);
    expect(change).toEqual({plus: 20, minus: 10, index: 0, object: array});

    array.set(1, 20);
    expect(change).toEqual({plus: 20, minus: undefined, index: 1, object: array});
});

it("mirrors arrays", function () {
    var array = [];
    var mirror = [];
    var observer = O.observeRangeChange(array, function (plus, minus, index) {
        swap(mirror, index, minus.length, plus);
    });
    array.push(1, 2, 3);
    array.shift();
    array.pop();
    expect(mirror).toEqual([2]);
    observer.cancel();
});

it("tracks an array with a plain object", function () {
    var array = [];
    var object = {};
    var observer = O.observeMapChange(array, function (plus, minus, index, type) {
        if (type === "delete") {
            delete object[index];
        } else { // type === "create" || type === "update"
            object[index] = plus;
        }
    });
    array.push(1, 2, 3);
    expect(object).toEqual({0: 1, 1: 2, 2: 3});

    array.splice(1, 1);
    expect(object).toEqual({0: 1, 1: 3});
});

it("observes a property of a property", function () {
    var a = {b: {c: 10}};
    var value;
    var observer = O.observePropertyChange(a, "b", function (b) {
        value = b.c;
        return O.observePropertyChange(b, "c", function (c) {
            value = c;
        });
    });

    a.b = {c: 20};
    expect(value).toBe(20);

    a.b.c = 30;
    expect(value).toBe(30);

    observer.cancel();
});



var swap = require("../swap");

describe("swap", function () {
    var array, otherArray;
    beforeEach(function () {
        array = [1, 2, 3];
    });

    it("grows", function () {
        swap(array, 3, 0, [4, 5, 6]);
        expect(array).toEqual([1, 2, 3, 4, 5, 6]);
    });

    it("grows with some removed", function () {
        swap(array, 1, 2, [4, 5, 6]);
        expect(array).toEqual([1, 4, 5, 6]);
    });

    it("grows from beyond length", function () {
        swap(array, 4, 0, [1]);
        expect(array).toEqual([1, 2, 3, , 1]);
    });

    it("grows from beyond length truncating removal", function () {
        swap(array, 4, 1, [1]);
        expect(array).toEqual([1, 2, 3, , 1]);
    });

    it("shrinks", function () {
        swap(array, 1, 1);
        expect(array).toEqual([1, 3]);
    });

    it("shrinks with some added", function () {
        swap(array, 1, 2, [4]);
        expect(array).toEqual([1, 4]);
    });

    it("copies a hole", function () {
        swap(array, 1, 1, [,]);
        expect(array).toEqual([1, , 3]);
    });

    it("copies holes", function () {
        swap(array, 1, 1, [,,]);
        expect(array).toEqual([1, , , 3]);
    });

    it("sets within", function () {
        array.set(1, 4);
        expect(array).toEqual([1, 4, 3]);
    });

    it("sets without", function () {
        array.set(4, 4);
        expect(array).toEqual([1, 2, 3, , 4]);
    });

    it("can replace content with content of another arraylike", function () {
        otherArray = { __proto__ : Array.prototype };
        otherArray[0] = 4;
        otherArray[1] = 5;
        otherArray.length = 2;
        swap(array, 0, array.length, otherArray);
        expect(array).toEqual([4, 5]);
    });

    it("should ignore non array like plus value", function () {
        swap(array, 0, array.length, 4);
        expect(array).toEqual([]);

    });

    it("should ignore extra arguments", function () {
        swap(array, 0, array.length, 4, 5, 6);
        expect(array).toEqual([]);
    });

    it("works with large arrays", function () {
        otherArray = new Array(200000);
        expect(function () {
            swap(array, 0, array.length, otherArray);
        }).not.toThrow();
        expect(array.length).toEqual(200000);
    });

});

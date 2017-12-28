
var MiniMap = require("../mini-map");

it("works", function () {
    var map = new MiniMap();
    var key = {};
    expect(map.has(key)).toBe(false);
    map.set(key, 10);
    expect(map.get(key)).toBe(10);
    expect(map.has(key)).toBe(true);
    map.delete(key);
    expect(map.has(key)).toBe(false);
});

describe("clear", function () {
    it("clears a map", function () {
        var map = new MiniMap();
        map.set(10, 10);
        map.set(20, 20);
        map.clear();
        expect(map.has(10)).toBe(false);
        expect(map.has(20)).toBe(false);
    });
});

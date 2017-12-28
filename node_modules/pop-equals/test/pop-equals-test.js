
var equals = require("../pop-equals");

describe("equals", function () {

    var fakeNumber = {
        valueOf: function () {
            return 10;
        }
    };
    var equatable = {
        value: 10,
        clone: function () {
            return this;
        },
        equals: function (n) {
            return n === 10 || !!n && n.value === 10;
        }
    };

    var equivalenceClasses = [
        {
            'unboxed number': 10,
            'boxed number': new Number(10),
            'faked number': fakeNumber,
            'equatable': equatable
        },
        {
            'array': [10],
            'other array': [10]
        },
        {
            'nested array': [[10, 20], [30, 40]]
        },
        {
            'object': {a: 10},
            'other object': {a: 10}
        },
        {
            'now': new Date()
        },
        {
            'NaN': NaN
        },
        {
            'undefined': undefined
        },
        {
            'null': null
        }
    ];

    // positives:
    // everything should be equal to every other thing in
    // its equivalence class
    equivalenceClasses.forEach(function (equivalenceClass) {
        Object.forEach(equivalenceClass, function (a, ai) {
            equivalenceClass[ai + " clone"] = Object.clone(a);
        });
        // within each pair of class, test exhaustive combinations to cover
        // the commutative property
        Object.forEach(equivalenceClass, function (a, ai) {
            describe(ai, function () {
                Object.forEach(equivalenceClass, function (b, bi) {
                    it("equals " + bi, function () {
                        expect(equals(a, b)).toBe(true);
                        //expect(a).toEqual(b);
                    });
                });
            });
        });
    });

    // negatives
    // everything from one equivalence class should not equal
    // any other thing from a different equivalence class
    equivalenceClasses.forEach(function (aClass, aClassIndex) {
        equivalenceClasses.forEach(function (bClass, bClassIndex) {
            // only compare each respective class against another once (>),
            // and not for equivalence classes to themselves (==).
            // This cuts the bottom right triangle below the diagonal out
            // of the test matrix of equivalence classes.
            if (aClassIndex >= bClassIndex)
                return;
            // but within each pair of classes, test exhaustive
            // combinations to cover the commutative property
            Object.forEach(aClass, function (a, ai) {
                Object.forEach(bClass, function (b, bi) {
                    it(ai + " not equals " + bi, function () {
                        expect(equals(a, b)).toBe(false);
                    });
                });
            });
        });
    });

    it("recognizes deep structural similarity", function () {
        var a = [];
        a.push(a);
        expect(equals(a, [[a]])).toBe(true);
    });

    it("recognizes deep structural dissimilarity", function () {
        function Foo() {}
        var foo = new Foo();
        expect(equals(
            [foo],
            [[[foo]]]
        )).toBe(false);
    });

    it("recognizes arrays", function () {
        expect(equals([], [])).toBe(true);
    });

    it("recognizes similar sparse arrays", function () {
        expect(equals([,,], [,,])).toBe(true);
    });

    it("recognizes dissimilar sparse arrays", function () {
        expect(equals([,,,], [,,])).toBe(false);
    });

});


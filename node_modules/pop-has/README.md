
# Has

This JavaScript package exports a has operator that accepts JavaScript objects,
arrays, and collections and returns whether the object contains an equivalent
value.

```
$ npm install --save pop-has
```

```js
var has = require("pop-has/object-has");
expect(has([1, 2, 3], 2)).toBe(true);
expect(has({a: 10}), 10).toBe(true);
expect(has(new Set([10]), 10)).toBe(true);
epxect(has([[1], [2], [3]], [2])).toBe(true);
```

The has operator accepts an object, a sought value, and an optional alternate
equality operator.
By default, `has` uses `pop-equals` to determine whether values are equivalent.
Thus, you can alternately use an identity function or whatever you find
suitable for determining equivalence.

```js
function is(x, y) {
    // The latter term is for NaN is NaN
    return x === y || (x !== x && y !== y);
}
expect(has([{}], {}, is)).toBe(false);
expect(has([1, 2, NaN], NaN, is)).toBe(true);
```

This operator can be safely used on large sparse arrays since it uses a for/in
loop with ownership checks on arrays and objects by default.
Thus, it is also suitable for searching values of the owned properties of an
object.

## Polymorphic operator

A well-planned system of objects is beautiful: a system where every meaningful
method for an object has been anticipated in the design.
Inevitably, another layer of architecture introduces a new concept and with it
the temptation to monkey-patch, dunk-punch, or otherwise cover-up the omission.
But reaching backward in time, up through the layers of architecture doesn't
always compose well, when different levels introduce concepts of the same name
but distinct behavior.

A polymorphic operator is a function that accepts as its first argument an
object and varies its behavior depending on its type.
Such an operator has the benefit of covering for the types from higher layers of
architecture, but defers to the eponymous method name of types yet to be
defined.

The has operator delegates to the has method of any object that implements it.
That method will receive the sought value and the appropriate equals operator.

## License and Copyright

Copyright (c) 2015 Kristopher Michael Kowal and contributors.
All rights reserved.
MIT License


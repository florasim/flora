
# Compare

This JavaScript package exports a comparison operator that accepts arbitrary
objects and performs a deep comparison operation, traversing into arrays as well
as delegating to the compare method of either argument if one exists, favoring
the left side.

A comparator returns a number that has the same relationship to zero as the left
value has to the right.
For numbers, subtracting the right from the left will give you such a
difference value, describing both the direction and magnitude of the difference.
A comparator for which the magnitude is not meaningful should return positive or
negative infinity.

As a core principle, every value is equal to itself, including NaN, making this
suitable for ordering arbitrary values in collections.

Boxed values, like `new Number(10)` will always be unboxed before comparing.

This comparison operator does not handle equivalent object graphs with reference
cycles at this juncture.

```js
var compare = require("pop-compare");
expect(compare(-1, 1)).toBe(-2);
expect(compare("abc", "abc")).toBe(0);
expect(compare(NaN, NaN)).toBe(0);
expect(compare("a", "b")).toBe(-Infinity);
```

The compare function accepts an alternate comparator for nested comparison as
its optional third argument.

```js
function dontcare() { return 0; }
expect(compare([10], [20], dontcare)).toBe(0);
```

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

The comparison operator delegates to the compare method of either the left or
right value, favoring the left, if either implements `compare`.

```js
var fake = { compare: function () { return -1; } };
expect(compare(fake, null)).toBe(-1);
```

The result will be inverted if the right is compared to the left.

```js
var fake = { compare: function () { return -1; } };
expect(compare(null, fake)).toBe(1);
```

## License and Copyright

Copyright (c) 2014 Montage Studio Inc. and contributors.
All rights reserved.
BSD 3-Clause license


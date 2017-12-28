
# Equals

This JavaScript package exports an equality operator that accepts arbitrary
objects and performs deep equality checks on objects and arrays, as well as
delegating to the equals method of other objects.

As a core principle, every value is equal to itself, including NaN, making this
suitable for verifying equivalent keys or values for storage or retrieval in
collections.

A boxed value, like `new Number(10)` will always be equal to its unboxed
equivalent, `10` in this case.

This operator can be safely used on object graphs that contain reference cycles.
By default, `equals` uses a `MiniMap` to recall objects it has already seen, but
this `memo` can be overridden.

```js
var cycle = {};
cycle.cycle = cycle;
equals(cycle, cycle, null, new Map());
```

Equals accepts:

-   left side value
-   right side value
-   equals, optional alternate equality checker for children
-   memo, optional alternate map for memoizing already visited values. Map,
    WeakMap, or any other memo implementing `set` or `has`. Notably, these maps
    can be reused if the equivalent objects remain equivalent between calls.

```js
equals(10, 10)
equals({a: 10, b: 20}}, {b: 20, a: 10})
equals([], [])
equals([[1, 2, 3]], [[1, 2, 3]])
```

Objects, so long as they are direct decendants of the Object prototype, are
equivalent if they have the same keys and same respective values, but order is
not significant.
Values from the left are only ever passed as left side arguments, and right only
as right side arguments.

An array, regardless of whether it is on the left or right, is equivalent to any
object with the same length and owned properties, so sparse arrays with the same
length and owning the same entries are equivalent.

```js
equals([,[,,],], [,[,,],]);
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

The equals operator delegates to the equals method of either the left or right
value, favoring the left, if either implements `equals`.
It passes the other argument, and forwards the alternate equals and memo.
Equals methods are expected to accept and use these arguments, or at the very
least forward them in any recursive equality checks.

## License and Copyright

Copyright (c) 2014 Montage Studio Inc. and contributors.
All rights reserved.
BSD 3-Clause license


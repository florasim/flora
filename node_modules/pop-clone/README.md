
# Clone

This JavaScript package exports a deep clone operator that accepts arbitrary
object graphs that may include reference cycles.
The clone operator delegates to the clone method of any object that implements
it.

```
$ npm install --save pop-clone
```

The clone method accepts a value or graph of objects and returns a
referentially equivalent graph of objects.

-   value
-   depth, Infinity by default
-   memo, an optional map, implementing `has` and `get`, suitable for mapping
    objects to objects.

```js
var clone = require("pop-clone");
var object = {};
object.object = object;
var mirror = clone(object);
```

Clone will replicate:

-   values
-   arrays
-   objects that inherit directly from `Object.prototype`.
-   objects that implement `clone(depth, memo)`.

Other values will throw a "Can't clone" error.

When the clone is finished, the memo will contain the reflection of every object
in the given graph.

```js
var memo = new Map();
var graph = {child: {}};
var cloned = clone(graph, null, memo);
expect(memo.get(graph.child)).toBe(cloned.child);
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

Objects that implement `clone` will receive the remaining depth and a memo.
If no memo is provided, the memo will be a MiniMap from the `mini-map` package.
The mini map performs well enough for small collections, but consider providing
a more sophisticated map, `Map` or `WeakMap`, for a large object graph.

```js
var object = {
    child: {
        clone: function (depth, memo) {
            expect(memo.has(object)).toBe(true);
            expect(depth).toBe(1);
            return "hello";
        }
    }
};
var cloned = clone(object, 2);
expect(cloned.child).toBe("hello");
```

## Copyright and License

This project has been factored out of Montage Studio's Collections and as such
retains its copyright and license.

Copyright (c) 2014 Montage Studio, Inc. and contributors.
BSD 3-Clause License


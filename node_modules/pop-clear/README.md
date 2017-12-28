
# Clear

This JavaScript package exports a clear operator that accepts an array, object,
or any other object that implements the clear method.
For objects, the clear method deletes all enumerable properties.

```
$ npm install --save pop-clear
```

```js
var clear = require("pop-clear");

var array = [1, 2, 3];
clear(array);
expect(array).toEqual([]);

var object = {a: 10, b: 20};
clear(object);
expect(object).toEqual({});

var instance = {
    clear: function () {
        throw new TypeError("Can't clear");
    }
};
expect(function () {
    clear(instance);
}).toThrow();
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

The clear operator delegates to the clear method of the given object if it is
implemented.
This is particularly useful for cases where the object may be an array or an
observable array, for which the `clear` method has been overridden to dispatch
ranged content changes.

## License and Copyright

Copyright (c) 2015 Kristopher Michael Kowal and contributors.
All rights reserved.
MIT License


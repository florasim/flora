
# Swap

At first blush, every method of an array that modifies its content can be
implemented in terms of `splice`.

```
pop(): splice(length - 1, 1)[0]
push(...values): splice(length, 0, ...values)
shift(): splice(0, 1)[0]
unshift(...values): splice(0, 0, ...values)
clear(): splice(0, length)
```

Splice always returns an array of the values removed, which is not useful for
modeling all of these methods.

Splice, at first blush, is also the appropriate operator to apply operational
transforms. An operational transform either adds values at a position, or
removes values from a position.
For this purpose, splice has some disadvantages.

1.  For sparse arrays, splice cannot be used to add new values beyond the end of
    the array; splice shifts the change to the end of the arrray.
2.  Becuase splice is variadic, accepting any number of arguments, those
    arguments get projected onto the stack.
    For a sufficiently large change, splice will throw a stack overflow
    RangeError.

## Swap rather than Splice

Swap has no return value and accepts an optional array of values to add.
It is well-suited as the underlying operator for operational transforms since it
wastes nothing for addition only and removal only, can add sparce arrays beyond
the end of a sparse array, and does not deal with the arguments object.

```js
var swap = require("pop-swap/swap");
swap(array, index, length, values?);
```

Swap accepts an array, an index, the number of values to to remove after that
index, then the values to add after that index.
The added values are optional.
The array or array of values to add may be sparse.
Swap returns nothing.

It is also suitable for implementing the remaining array methods.
Of course, this is only really useful if you need to implement all of those
array methods in a way that channels through swap.
This is the story behind [observable arrays][Collections] in the Montage
Collections package.

[Collections]: https://github.com/montagejs/collections

```
splice(index, minus, ...values):
    index = Math.min(index, this.length);
    var result = this.slice(index, index + minus);
    swap(index, minus, values);
    return result;
pop():
    var last = this[this.length - 1];
    swap(this.length - 1, 1);
    return last;
push(...values)
    swap(this.length, 0, values);
shift():
    var first = this[0];
    swap(0, 1);
    return first;
unshift(...values):
    swap(0, 0, values);
clear();
    swap(0, this.length);
```

## Polymoprhic operators

A well planned system of objects is beautiful: a system where every meaningful
method for an object has been anticipated in the design.
Inevitably, another layer of architecture introduces a new concept and with it
the temptation to monkey-patch, dunk-punch, or otherwise cover up the omission.
But reaching backward in time, up through the layers of architectures doesn't
always componse well, when different leaves introduce concepts of the same name
but distinct behavior.

A polymorphic operator is a function that accepts as its first argument an
object and varies its behavior depending on its type.
Such an operator has the benefit of covering for the types from higher layers of
architecture, but defers to the eponymous method name of types yet to be
defined.

```js
var swap = require("pop-swap");
var array = ['Hello', 'World!'];
swap(array, 0, 1, ['Farewell']);

var proxy = {
    array: ['Hello', 'World!'],
    swap: function (start, minusLength, plus) {
        swap(this.array, start, minusLength, plus);
    }
};
swap(proxy, 0, 1, ['Farewell']);
```


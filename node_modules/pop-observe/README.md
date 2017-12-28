
# Observable content changes

This JavaScript package, suitable for browsers and Node.js, provides a system
for synchronously observing content changes to arrays, objects, and other
instances.
These observers have a common, composable style, expose their internal state for
debugging, and reuse state tracking objects to reduce garbage collection.

-   Changes can be captured before or after they are made.
-   The last argument of a change notification is the object observed, so a
    single handler can service multiple objects.
-   Handler methods can return a child observer object, which will be implicitly
    cancelled before the next change, so observers can be stacked.
-   Does not alter the Array base type, but promotes array instances to an
    ObservableArray when they are observed.

## Installation

```
npm install --save pop-observe@2
```

## Examples

Observing the length of an array.

```js
var array = [];
var observer = O.observePropertyChange(array, "length", function (length) {
    expect(length).toBe(1);
});
array.push(10);
observer.cancel();
```

Observing a property of an ordinary object.

```js
var object = {weight: 10};
var observer = O.observePropertyChange(object, "weight", function (weight) {
    expect(weight).toBe(20);
});
object.weight = 20;
observer.cancel();
```

Observing values at indexes.

```js
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
```

Mirroring arrays.

```js
var O = require("pop-observe");
var swap = require("pop-swap");
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
```

Tracking an array with a plain object.

```js
var O = require("pop-observe");
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
```

Observing a property of a property.
Note that the cancel method gets rid of observers on a.b and b.c.
Note that the b.c observer gets canceled every time b changes.

```js
var O = require("pop-observe");
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
```

## Change notification arguments

-   Property change observers dispatch (plus, minus, name, object) change
    notifications when the value of a specific, named property changes.
    Each observer sees the old value (minus), new value (plus), property name
    (name), and the object (object) such that a single property change handler
    can service multiple observers.
    Observing a property of an ordinary object replaces that property with a
    getter and setter.
-   Range change observers dispatch (plus, minus, index, object) change
    notifications when ordered values are removed (captured in a minus array),
    then added (captured in the plus array), at a particular index.
    Each handler also receives the object observed, so a single handler can
    service multiple observers.
-   Map change observers dispatch (plus, minus, key, object) change
    notifications when the value for a specific key in a map has changed.
  
## Behavior on Arrays

-   This library does not alter plain JavaScript arrays or the Array prototype.
-   Observing an array transforms that array into an ObservableArray either by
    subverting its prototype or adding properties directly to the instance.
    Observable arrays have additional [swap][] and set methods.
-   Observing any property change on an array transforms that array into an
    observable array and property changes are dispatched for the "length" or any
    value by its index.
-   Observing range changes on an array transforms that array into an observable
    array and all of its methods produce these change notifications.
-   Observing any map change on an array transforms the array into an observable
    array and map changes are dispatched for changes to the value at the given
    index.

[swap]: https://github.com/kriskowal/pop-swap

## Custom types

Arbitrary constructors can mix in or inherit the ObservableObject type to
support the observable interface directly and do not need to provide any further
support.

```js
var inherits = require("util").inherits;
var ObservableObject = require("pop-observe/observable-object");
function Custom() {}
inherits(Custom, ObservableObject);
```

Arbitrary constructors can mix in or inherit the ObservableRangeChange type and
must explicitly dispatch change notifications when range change observers are
active.

```js
var inherits = require("util").inherits;
var ObservableRange = require("pop-observe/observable-range");
function Custom() {}
inherits(Custom, ObservableRange);
Customer.prototype.unshift = function unshift(value) {
    if (this.dispatchesRangeChanges) {
        this.dispatchRangeWillChange([value], [], 0);
    }
    // actual work
    if (this.dispatchesRangeChanges) {
        this.dispatchRangeChange([value], [], 0);
    }
};
```

This library does not provide any map implementations but provides the
ObservableMap for any to inherit or mix in.

```js
var inherits = require("util").inherits;
var ObservableMap = require("pop-observe/observable-map");
function Custom() {}
inherits(Custom, ObservableMap);
Customer.prototype.delete = function delete(key) {
    var old = this.get(key);
    if (!old) {
        return;
    }
    if (this.dispatchesMapChanges) {
        this.dispatchMapWillChange("delete", key, undefined, old);
    }
    // actual work
    if (this.dispatchesMapChanges) {
        this.dispatchMapChange("delete", key, undefined, old);
    }
};
```

All of thse can be mixed by copying the properties from their prototypes.

```js
var ObservableObject = require("pop-observe/observable-object");
// Your favorite property copying idiom here
var owns = Object.prototype.hasOwnProperty;
for (var name in ObservableObject.prototype) {
    if (owns.call(ObservableObject.prototype, name)) {
        Customer.prototype[name] = ObservableObject.prototype[name];
    }
}
```

## Interface

Each type of observer provides before and after methods for observation and
manual dispatch.
For properties, manual dispatch is necessary when a property is hidden behind a
getter and a setter *if* the value as returned by the getter changes without the
setter ever being invoked.
Arrays require manual dispatch only if the value at a given index changes
without invoking an array mutation method.
For this reason, observable arrays have a `set(index, value)` method.
All ranged and map collections must implement manual dispatch when their
`dispatchesRangeChanges` or `dispatchesMapChanges` properties are true.

Object property change observers

-   observePropertyChange(object, handler, note, capture) -> Observer
-   observePropertyWillChange(object, handler, note) -> Observer
-   dispatchPropertyChange(object, name, plus, minus, capture)
-   dispatchPropertyWillChange(object, name, plus, minus)

-   makePropertyObservable(object, name)
-   preventPropertyObserver(object, name)

Range change observers

-   observeRangeChange(object, handler, note, capture) -> Observer
-   observeRangeWillChange(object, handler, note) -> Observer
-   dispatchRangeChange(object, plus, minus, index, capture)
-   dispatchRangeWillChange(object, plus, minus, index)

Map change observers

-   observeMapChange(object, handler, note, capture) -> Observer
-   observeMapWillChange(object, handler, note) -> Observer
-   dispatchMapChange(object, type, key, plus, minus, capture)
-   dispatchMapWillChange(object, type, key, plus, minus)

Observer objects in general

-   Observer.prototype.cancel();

## Handlers

Handlers may be raw functions, or objects with one or more handler methods.
Observers for different kinds of changes and before and after changes call
different methods of the handler object based on their availability at the time
that the observer is created.
For example, `observePropertyWillChange(array, "length", handler)` will create a
property observer that will delegate to the
`handler.handleLengthPropertyWillChange(plus, minus, key, object)` method, or
just that generic `handler.handlePropertyWillChange(plus, minus, key, object)`
method if the specific method does not exist.

-   observable object, property observers (plus, minus, key, object)
    -   after change
        -   specific: handleProperty*Name*Change
        -   general: handlePropertyChange
    -   before change
        -   specific: handleProperty*Name*WillChange
        -   general: handlePropertyWillChange

Range changes do not operate on a given property name, but the
`observeRangeChange(handler, name, note, capture)` method allows you to give the
range change a name, for example, the name of the array observed on a given
object.

```js
var handler = {
    handleValuesRangeChange: function (plus, minus, index, object) {
        // ...
    }
};
var observer = repetition.values.observeRangeChange(handler, "values");
// ...
repetition.values.push(10);
// ...
observer.cancel();
```

-   observable range (plus, minus, index, object)
    -   after change
        -   specific: handle*Name*RangeChange
        -   general: handleRangeChange
    -   before change
        -   specific: handle*Name*RangeWillChange
        -   general: handleRangeWillChange

Likewise, `observeMapChange(handler, name, note, capture)` accepts a name for a
specific handler method.

-   observable map (plus, minus, key, object)
    -   after change
        -   specific: handle*Name*MapChange
        -   general: handleMapChange
    -   before change
        -   specific: handle*Name*MapWillChange
        -   general: handleMapWillChange

## Observers

Observers are re-usable objects that capture the state of the observer.
Most importantly, they provide the `cancel` method, which disables the observer
and returns it to a free list for the observer methods to re-use.
They are suitable for run-time inspection of the state of the observer.

They also carry an informational `note` property, if the caller of the observe
method provided one.
This is intended for use by third parties to provide additional debugging
information about an observer, for example, where the observer came from.

-   PropertyChangeObserver
    -   object
    -   propertyName
    -   observers
    -   handler
    -   handlerMethodName
    -   childObserver
    -   note
    -   capture
    -   value
-   RangeChangeObserver
    -   object
    -   name
    -   observers
    -   handler
    -   handlerMethodName
    -   childObserver
    -   note
    -   capture
-   MapChangeObserver
    -   object
    -   name
    -   observers
    -   handler
    -   handlerMethodName
    -   childObserver
    -   note
    -   capture

## Implementing observability

The `pop-observe/observable-object`, `pop-observe/observable-range`, and
`pop-observe/observable-map` modules export mixable or prototypically
inheritable constructors.
Objects that inherit the observable interface must then dispatch change and will
change notifications if they are being observed, in all of their methods that
change their content.

-   ObservableObject.observePropertyChange(object, handler, note, capture)
-   ObservableObject.observePropertyWillChange(object, handler, note)
-   ObservableObject.dispatchPropertyChange(object, name, plus, minus, capture)
-   ObservableObject.dispatchPropertyWillChange(object, name, plus, minus)
-   ObservableObject.getPropertyChangeObservers(object, name, capture)
-   ObservableObject.getPropertyWillChangeObservers(object, name)
-   ObservableObject.makePropertyObservable(object, name)
-   ObservableObject.preventPropertyObserver(object, name)

-   ObservableObject.prototype.observePropertyChange(handler, note, capture)
-   ObservableObject.prototype.observePropertyWillChange(handler, note)
-   ObservableObject.prototype.dispatchPropertyChange(name, plus, minus, capture)
-   ObservableObject.prototype.dispatchPropertyWillChange(name, plus, minus)
-   ObservableObject.prototype.getPropertyChangeObservers(name, capture)
-   ObservableObject.prototype.getPropertyWillChangeObservers(name)
-   ObservableObject.prototype.makePropertyObservable(name)
-   ObservableObject.prototype.preventPropertyObserver(name)

-   ObservableRange.prototype.observeRangeChange(handler, name, note, capture)
-   ObservableRange.prototype.observeRangeWillChange(handler, name, note)
-   ObservableRange.prototype.dispatchRangeChange(handler, name, note, capture)
-   ObservableRange.prototype.dispatchRangeWillChange(handler, name, note)
-   ObservableRange.prototype.makeRangeChangesObservable()

-   ObservableMap.prototype.observeMapChange(handler, name, note, capture)
-   ObservableMap.prototype.observeMapWillChange(handler, name, note)
-   ObservableMap.prototype.dispatchMapChange(type, key, plus, minus, capture)
-   ObservableMap.prototype.dispatchMapWillChange(type, key, plus, minus)
-   ObservableMap.prototype.makeMapChangesObservable()


## Copyright and License

Copyright (c) 2015 Motorola Mobility, Montage Studio, Kristopher Michael Kowal,
and contributors.
All rights reserved.
BSD 3-Clause license.


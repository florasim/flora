
# MiniMap

The mini map implements a subset of the ECMAScript 6 Map that performs well
enough for very small collections.
This implementation exists for a good-enough approximation of a Map that doesn't
waste much space even if an alternative is almost always used in practice.
Keys may be objects.
Supports `has`, `get`, `set`, `delete`, and `clear`.
The `MiniMap` constructor supports no arguments.

```js
var map = new Map();
var key = {};
expect(map.has(key)).toBe(false);
map.set(key, 10);
expect(map.get(key)).toBe(10);
expect(map.has(key)).toBe(true);
map.delete(key);
expect(map.has(key)).toBe(false);
map.clear();
```

## License and Copyright

Copyright (c) 2015 Kristopher Michael Kowal and contributors.
All rights reserved.
MIT License.


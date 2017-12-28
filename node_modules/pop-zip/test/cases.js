'use strict';

module.exports = {
    'common case': {
        input: [
            ['a', 'b', 'c'],
            [1, 2, 3],
            ['x', 'y', 'z']
        ],
        output: [
            ['a', 1, 'x'],
            ['b', 2, 'y'],
            ['c', 3, 'z']
        ]
    },
    'one short row': {
        input: [
            ['a', 'b', 'c'],
            [1, 2]
        ],
        output: [
            ['a', 1],
            ['b', 2]
        ]
    },
    'one empty row': {
        input: [
            ['a', 'b', 'c'],
            [1, 2, 3],
            []
        ],
        output: [
        ]
    },
    'degenerate': {
        input: [],
        output: []
    }
};

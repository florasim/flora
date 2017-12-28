'use strict'

var url = require('url')

module.exports = function(input, replace) {
  var parsed = url.parse(input)

  parsed.auth = replace || null

  return parsed.format()
}

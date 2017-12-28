// From http://stackoverflow.com/a/5306111/106302
// Originally from http://simonwillison.net/2006/Jan/20/escape/ (dead link)
RegExp.escape = function(value) {
    return value.replace(/[-[\]{}()*+?.,\\^$|#\s]/g, "\\$&");
};

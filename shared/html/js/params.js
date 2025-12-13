(function(global) {
    "use strict";
    if (typeof Array.isArray === 'undefined') {
        Array.isArray = function(obj) {
            return Object.prototype.toString.call(obj) === '[object Array]' ||
                obj instanceof Array;
        };
    }

    function getParams() {
        var params = {};
        var queryString = window.location.search.substring(1);

        if (queryString) {
            var pairs = queryString.split('&');
            for (var i = 0; i < pairs.length; i++) {
                var pair = pairs[i].split('=');
                var key = decodeURIComponent(pair[0]);
                var value = decodeURIComponent(pair[1] || '');
                if (params[key]) {
                    if (Array.isArray(params[key])) {
                        params[key].push(value);
                    } else {
                        params[key] = [params[key], value];
                    }
                } else {
                    params[key] = value;
                }
            }
        }
        return params;
    }
    Object.defineProperty(global, 'Params', {
        get: getParams
    });
    Object.defineProperty(global, 'params', {
        get: getParams
    });
})(this);
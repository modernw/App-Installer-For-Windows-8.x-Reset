(function(global) {
    "use strict";

    function ready(e) {
        Windows.UI.DPI.mode = 1
    }
    OnLoad.add(ready);
})(this);
(function(global) {
    "use strict";

    function ready(e) {
        function nextstep() {
            Resources.processAll();
        }
        if (typeof WinJS !== "undefined") WinJS.UI.processAll().done(nextstep);
        else nextstep();
    }
    OnLoad.add(ready);
})(this);
(function(global) {
    "use strict";

    function ready(e) {
        function nextstep() {
            Resources.processAll();
        }
        WinJS.UI.processAll().done(nextstep);
    }
    OnLoad.add(ready);
})(this);
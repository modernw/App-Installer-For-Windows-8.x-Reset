(function(global) {
    "use strict";

    function getSettingsItem(page, displayName) {
        return {
            page: page,
            displayName: displayName
        };
    }
    var settingItems = {
        appinstaller: getSettingsItem("appinstaller.html", "App Installer")
    };
    Object.defineProperty(global, "settingPages", {
        get: function() { return settingItems; }
    });
})(this);
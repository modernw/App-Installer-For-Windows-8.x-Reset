(function(global) {
    "use strict";

    function getPage(page, display) {
        return {
            page: page,
            title: display
        };
    }
    var pages = {
        general: getPage("appinstaller/general.html", "General"),
        theme: getPage("appinstaller/theme.html", "Theme"),
        update: getPage("update.html", "Update")
    };
    Object.defineProperty(global, "pages", {
        get: function() {
            return pages;
        }
    });
    Object.defineProperty(global, "guidePage", {
        get: function() {
            return getPage("appinstaller/guide.html", "Guide");
        }
    });
})(this);
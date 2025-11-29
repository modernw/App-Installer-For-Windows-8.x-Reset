(function(global) {
    "use strict";

    function getPage(page, display) {
        return {
            page: page,
            title: display
        };
    }
    var pages = {
        general: getPage("appinstaller/general.html", getPublicRes(101)),
        theme: getPage("appinstaller/theme.html", getPublicRes(102)),
        update: getPage("update.html", getPublicRes(103))
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
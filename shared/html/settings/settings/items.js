(function(global) {
    "use strict";

    function getPage(tag, page, display) {
        return {
            tag: tag,
            page: page,
            title: display
        };
    }
    var pages = {
        general: getPage("general", "settings/general.html", getPublicRes(101)),
    };
    Object.defineProperty(global, "pages", {
        get: function() {
            return pages;
        }
    });
    Object.defineProperty(global, "guidePage", {
        get: function() {
            return getPage("guide", "settings/guide.html", "guide");
        }
    });
})(this);
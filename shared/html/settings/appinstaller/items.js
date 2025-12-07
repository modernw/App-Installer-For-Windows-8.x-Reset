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
        general: getPage("general", "appinstaller/general.html", getPublicRes(101)),
        theme: getPage("theme", "appinstaller/theme.html", getPublicRes(102)),
        update: getPage("update", "update.html", getPublicRes(103)),
        about: getPage("about", "appinstaller/about.html", getPublicRes(124))
    };
    Object.defineProperty(global, "pages", {
        get: function() {
            return pages;
        }
    });
    Object.defineProperty(global, "guidePage", {
        get: function() {
            return getPage("guide", "appinstaller/guide.html", "Guide");
        }
    });
})(this);
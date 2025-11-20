(function(global) {
    "use strict";
    Object.defineProperty(global, "pagecontainer", {
        get: function() { return document.querySelector(".pagecontainer"); }
    });

    function getPage() {
        return pagecontainer.querySelector(".page");
    }
    var supportPageList = [
        "splash",
        "select",
        "preinstall",
        "installing",
        "installsuccess",
        "installfailed"
    ];
    var supportMulPageList = [
        "preinstall",
        "installing",
        "installsuccess",
        "installfailed"
    ];

    function setPage(swPageLabel, bIsMulti) {
        var page = getPage();
        swPageLabel = ("" + (swPageLabel || ""));
        for (var i = 0; i < supportPageList.length; i++) {
            if (Bridge.NString.equals(swPageLabel, supportPageList[i])) {
                page.classList.add(supportPageList[i]);
            } else {
                if (page.classList.contains(supportPageList[i])) page.classList.remove(supportPageList[i]);
            }
        }
        if (page.classList.contains("multiple")) page.classList.remove("multiple");
        for (var j = 0; j < supportMulPageList.length; j++) {
            if (Bridge.NString.equals(swPageLabel, supportMulPageList[j]) && bIsMulti) {
                page.classList.add("multiple");
                break;
            }
        }
    }

    function getPageLabel() {
        var page = getPage();
        for (var i = 0; i < supportPageList.length; i++) {
            if (page.classList.contains(supportPageList[i])) {
                return supportPageList[i];
            }
        }
        return "";
    }

    function isMultiPage() {
        var page = getPage();
        return page.classList.contains("multiple");
    }

    function setPageMultiple(bIsMulti) {
        setPage(getPageLabel(), bIsMulti);
    }

    module.exports = {
        Page: {}
    };
    Object.defineProperty(Page, "current", {
        get: function() { return getPageLabel(); },
        set: function(swPageLabel) { setPage(swPageLabel, isMultiPage()); }
    });
    Object.defineProperty(Page, "multiple", {
        get: function() { return isMultiPage(); },
        set: function(bIsMulti) { setPage(getPageLabel(), bIsMulti); }
    });
})(this);
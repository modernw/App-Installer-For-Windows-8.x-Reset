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
        "loading",
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

    var splashBackColor = "";

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
        if (Bridge.NString.equals(swPageLabel, "splash")) {
            if (splashBackColor) {
                page.style.backgroundColor = splashBackColor;
            }
        } else {
            page.style.backgroundColor = "";
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

    function getSplashPage() {
        var page = document.querySelector(".page.splash");
        if (!page) return null;
        var ret = {};
        Object.defineProperty(ret, "background", {
            get: function() {
                var page = document.querySelector(".page.splash");
                return page.style.backgroundColor || splashBackColor;
            },
            set: function(value) {
                splashBackColor = value;
                var page = document.querySelector(".page.splash");
                if (page) page.style.backgroundColor = value;
            }
        });
        Object.defineProperty(ret, "imagesrc", {
            get: function() {
                var splashimg = page.querySelector("img.splash");
                return splashimg.src;
            },
            set: function(value) {
                var splashimg = page.querySelector("img.splash");
                splashimg.src = value;
            }
        });
        Object.defineProperty(ret, "content", {
            get: function() {
                var content = page.querySelector(".content.splash");
                return content;
            },
        });
        return ret;
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
    Object.defineProperty(Page, "splash", {
        get: function() { return getSplashPage(); }
    });
})(this);
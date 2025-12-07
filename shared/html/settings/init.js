(function(global) {
    "use strict";

    function ready(e) {
        var page = document.querySelector("#settingpage");
        var guide = page.querySelector(".page.guide");
        var slide = guide.querySelector("aside");
        var content = guide.querySelector("main");
        var shead = slide.querySelector("header");
        var list = slide.querySelector("ul");
        list.innerHTML = "";
        var items = global.settingPages;
        var keys = Object.keys(items);
        for (var i = 0; i < keys.length; i++) {
            var item = items[keys[i]];
            var inode = document.createElement("li");
            inode.setAttribute("data-page", item.page);
            inode.textContent = item.displayName;
            Windows.UI.Event.Util.addEvent(inode, "click", function(e) {
                window.location = "settings/" + this.getAttribute("data-page");
            });
            list.appendChild(inode);
            // WinJS.UI.Animation.createAddToListAnimation(list, inode).execute();
        }
    }
    OnLoad.add(ready);
})(this);
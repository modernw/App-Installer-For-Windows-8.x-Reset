(function(global) {
    "use strict";
    var cmdargs = JSON.parse(Bridge.External.cmdArgs);

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
            inode.setAttribute("data-name", item.displayName);
            inode.setAttribute("data-tag", keys[i]);
            inode.textContent = item.displayName;
            Windows.UI.Event.Util.addEvent(inode, "click", function(e) {
                window.location = "settings/" + this.getAttribute("data-page");
            });
            list.appendChild(inode);
            // WinJS.UI.Animation.createAddToListAnimation(list, inode).execute();
        }
        var current = "";
        try { if (cmdargs.length > 0) current = cmdargs[0] } catch (e) {}
        if (current && current.length > 0 && !Bridge.External.jump1) {
            for (var i = 0; i < list.children.length; i++) {
                if (Bridge.NString.equals(list.children[i].getAttribute("data-tag"), current)) {
                    Bridge.External.jump1 = true;
                    setTimeout(function(thisnode) {
                        thisnode.click();
                    }, 0, list.children[i]);
                    break;
                }
            }
        }
    }
    OnLoad.add(ready);
})(this);
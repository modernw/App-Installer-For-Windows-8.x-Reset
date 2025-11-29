(function(global) {
    "use strict";

    function ready(e) {
        var page = document.querySelector("#settingpage");
        var guide = page.querySelector(".page.guide");
        var slide = guide.querySelector("aside");
        var content = guide.querySelector(".main");
        var shead = slide.querySelector("header");
        var list = slide.querySelector("ul");
        list.innerHTML = "";
        var items = pages;
        var tags = Object.keys(items);
        var eventutil = Windows.UI.Event.Util;
        for (var i = 0; i < tags.length; i++) {
            var tag = tags[i];
            var item = items[tag];
            var li = document.createElement("li");
            li.setAttribute("data-page", item.page);
            li.innerHTML = item.title;
            eventutil.addEvent(li, "click", function() {
                for (var j = 0; j < list.children.length; j++) {
                    var child = list.children[j];
                    if (child.classList.contains("selected"))
                        child.classList.remove("selected");
                }
                content.src = this.getAttribute("data-page");
                this.classList.add("selected");
            });
            list.appendChild(li);
        }
        content.src = guidePage.page;
    }
    OnLoad.add(ready);
})(this);
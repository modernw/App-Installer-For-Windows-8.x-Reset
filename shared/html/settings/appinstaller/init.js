(function(global) {
    "use strict";

    function ready(e) {
        var page = document.querySelector("#settingpage");
        var guide = page.querySelector(".page.guide");
        var slide = guide.querySelector("aside");
        setTimeout(function() {
            var barcolor = visual["BackgroundColor"];
            slide.style.backgroundColor = barcolor;
            slide.style.color = Color.getSuitableForegroundTextColor(Color.parse(barcolor), [Color.Const.white, Color.Const.black]).stringify();
        }, 50);
        var content = guide.querySelector(".main");
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
                content.style.display = "none";
                for (var j = 0; j < list.children.length; j++) {
                    var child = list.children[j];
                    if (child.classList.contains("selected"))
                        child.classList.remove("selected");
                }
                content.src = this.getAttribute("data-page");
                setTimeout(function() {
                    content.style.display = "";
                    Windows.UI.Animation.runAsync(content, Windows.UI.Animation.Keyframes.SlideInFromBottom);
                }, 0);
                this.classList.add("selected");
            });
            list.appendChild(li);
        }
        content.src = guidePage.page;
    }
    OnLoad.add(ready);
})(this);
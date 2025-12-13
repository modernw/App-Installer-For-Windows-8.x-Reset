(function(global) {
    "use strict";

    function ready(e) {
        var page = document.querySelector("#settingpage");
        var guide = page.querySelector(".page.guide");
        var slide = guide.querySelector("aside");
        var content = guide.querySelector(".main");
        var shead = slide.querySelector("header");
        var list = slide.querySelector("ul");
        var apptitle = shead.querySelector("#apptitle");
        var backbtn = shead.querySelector("#back");
        if (backbtn && backbtn.disabled) {
            apptitle.style.marginLeft = backbtn.style.marginLeft;
        } else {
            apptitle.style.marginLeft = "";
        }
        try {
            slide.style.backgroundColor = Bridge.UI.themeColor;
            slide.style.color = Color.getSuitableForegroundTextColor(Color.parse(Bridge.UI.themeColor), [Color.Const.white, Color.Const.black]).RGBA.stringify();
            setTimeout(function() {
                var h2style = document.getElementById("h2-style");
                if (!h2style) {
                    h2style = document.createElement("style");
                    h2style.id = "h2-style";
                }
                h2style.innerHTML = ".main h2 { color: " + Color.getSuitableForegroundTextColor(Color.parse("#F3F3F3"), [Color.parse(Bridge.UI.themeColor), Color.Const.black]).RGBA.stringify() + " }";
                document.head.appendChild(h2style);
            }, 0);
        } catch (e) {
            console.error(e);
        }
        setTimeout(function() {
            slide.style.transition = "all 0.5s cubic-bezier(0.1, 0.9, 0.2, 1)";
        }, 0);
        shead.style.display = "none";
        if (list) list.style.display = "none";
        content.style.display = "none";
        setTimeout(function() {
            shead.style.display = "";
            Windows.UI.Animation.runAsync(shead, Windows.UI.Animation.Keyframes.Flyout.toLeft);
        }, 0);
        setTimeout(function() {
            if (list) {
                list.style.display = "";
                Windows.UI.Animation.runAsync(list, Windows.UI.Animation.Keyframes.Flyout.toLeft);
            }
        }, 50);
        setTimeout(function() {
            content.style.display = "";
            Windows.UI.Animation.runAsync(content, [Windows.UI.Animation.Keyframes.Flyout.toLeft, Windows.UI.Animation.Keyframes.Opacity.visible]);
        }, 100);
    }
    OnLoad.add(ready);
})(this);
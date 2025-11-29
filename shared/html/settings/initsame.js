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
            slide.style.color = Color.getSuitableForegroundTextColor(Color.parse(slide.style.backgroundColor), [Color.Const.white, Color.Const.black]);
        } catch (e) {}
        setTimeout(function() {
            slide.style.transition = "all 0.5s cubic-bezier(0.1, 0.9, 0.2, 1)";
        }, 0);
        shead.style.display = "none";
        list.style.display = "none";
        content.style.display = "none";
        setTimeout(function() {
            shead.style.display = "";
            Windows.UI.Animation.runAsync(shead, Windows.UI.Animation.Keyframes.Flyout.toLeft);
        }, 0);
        setTimeout(function() {
            list.style.display = "";
            Windows.UI.Animation.runAsync(list, Windows.UI.Animation.Keyframes.Flyout.toLeft);
        }, 50);
        setTimeout(function() {
            content.style.display = "";
            Windows.UI.Animation.runAsync(content, Windows.UI.Animation.Keyframes.SlideInFromBottom);
        }, 100);
    }
    OnLoad.add(ready);
})(this);
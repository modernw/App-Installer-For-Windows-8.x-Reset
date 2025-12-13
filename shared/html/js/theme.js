(function(global) {
    "use strict";
    var winjsCss = document.querySelector("link#winjs-style");
    var themeCss = document.querySelector("link#theme-style");
    var winjsPath = "libs/winjs/2.0/css/ui-{themecolor}.css";
    var themePath = "theme/{themecolor}/{id}/{id}.css";
    var ini = Bridge.External.Config.getConfig();
    var themeSection = ini.getSection("Personalization");
    var ThemeType = {
        light: 1, // 浅色模式
        dark: 2, // 深色模式
        auto: 3, // 跟随系统
        time: 4, // 跟随时间
        custom: 5 // 自定义
    };
    var nstrutil = Bridge.NString;
    var ColorType = {
        light: 0xFFFFFF,
        dark: 0x000000,
    };

    function getThemeSwitchType() {
        var themeType = themeSection.getKey("AppInstaller:ThemeMode").value;
        if (nstrutil.equals(themeType, "light")) return ThemeType.light;
        else if (nstrutil.equals(themeType, "dark")) return ThemeType.dark;
        else if (nstrutil.equals(themeType, "auto")) return ThemeType.auto;
        else if (nstrutil.equals(themeType, "time")) return ThemeType.time;
        else if (nstrutil.equals(themeType, "custom")) return ThemeType.custom;
        else return ThemeType.light;
    }

    function getCurrentThemeColor() {
        try {
            var themeType = getThemeSwitchType();
            if (themeType === ThemeType.light) return ColorType.light;
            else if (themeType === ThemeType.dark) return ColorType.dark;
            else if (themeType === ThemeType.auto) {
                if (Bridge.UI.darkmode) return ColorType.dark;
                else return ColorType.light;
            } else if (themeType === ThemeType.time) {
                var daytime = new Date();
                var nighttime = new Date();
                try {
                    var daytimestr = themeSection.getKey("AppInstaller:DayTime").value;
                    daytime = new Date(daytimestr);
                } catch (e) {
                    console.error(e);
                    daytime = new Date('2011-07-15T08:00:00.000Z');
                }
                try {
                    var nighttimestr = themeSection.getKey("AppInstaller:NightTime").value;
                    nighttime = new Date(nighttimestr);
                } catch (e) {
                    console.error(e);
                    nighttime = new Date('2011-07-15T20:00:00.000Z');
                }
                var now = new Date();
                daytime.setFullYear(now.getFullYear());
                nighttime.setFullYear(now.getFullYear());
                daytime.setMonth(now.getMonth());
                nighttime.setMonth(now.getMonth());
                daytime.setDate(now.getDate());
                nighttime.setDate(now.getDate());
                if (now >= daytime && now < nighttime) return ColorType.light;
                else return ColorType.dark;
            } else if (themeType === ThemeType.custom) {
                var customColor = themeSection.getKey("AppInstaller:CustomColor").value;
                if (nstrutil.equals(customColor, "light")) return ColorType.light;
                else if (nstrutil.equals(customColor, "dark")) return ColorType.dark;
                else return ColorType.light;
            } else {
                return ColorType.light;
            }
        } catch (e) {
            return ColorType.light;
        }
    }

    function getSuitableTheme() {
        var type = getThemeSwitchType();
        var color = getCurrentThemeColor();
        var ret = "";
        if (color === ColorType.light) ret = themeSection.getKey("AppInstaller:LightTheme").value;
        else ret = themeSection.getKey("AppInstaller:DarkTheme").value;
        if (type === ThemeType.custom) ret = "custom";
        if (nstrutil.empty(ret)) ret = "default";
        return ret;
    }

    function getTimeModeTimeLimit() {
        var ret = {
            day: new Date(),
            night: new Date()
        };
        try {
            var daytimestr = themeSection.getKey("AppInstaller:DayTime").value;
            ret.day = new Date(daytimestr);
        } catch (e) {
            console.error(e);
            ret.day = new Date('2011-07-15T08:00:00.000Z');
        }
        try {
            var nighttimestr = themeSection.getKey("AppInstaller:NightTime").value;
            ret.night = new Date(nighttimestr);
        } catch (e) {
            console.error(e);
            ret.night = new Date('2011-07-15T20:00:00.000Z');
        }
        var now = new Date();
        ret.day.setFullYear(now.getFullYear());
        ret.night.setFullYear(now.getFullYear());
        ret.day.setMonth(now.getMonth());
        ret.night.setMonth(now.getMonth());
        ret.day.setDate(now.getDate());
        ret.night.setDate(now.getDate());
        return ret;
    }

    function copyTime(to, from) {
        to.setHours(
            from.getHours(),
            from.getMinutes(),
            from.getSeconds(),
            from.getMilliseconds()
        );
    }
    var dayTimer = null;
    var nightTimer = null;

    function setupThemeTimers(limit, execFunc) {
        if (limit === null || limit === void 0) limit = getTimeModeTimeLimit();
        var now = new Date();
        var dayTime = new Date(now);
        var nightTime = new Date(now);
        copyTime(dayTime, limit.day);
        copyTime(nightTime, limit.night);
        // 处理跨天：如果 night <= day，说明夜晚跨到第二天
        var nightIsNextDay = nightTime <= dayTime;
        if (nightIsNextDay) {
            nightTime.setDate(nightTime.getDate() + 1);
        }
        var isDayTime;
        if (nightIsNextDay) {
            isDayTime = now >= dayTime && now < nightTime;
        } else {
            isDayTime = now >= dayTime && now < nightTime;
        }
        var nextDay = new Date(dayTime);
        var nextNight = new Date(nightTime);
        if (now >= dayTime) nextDay.setDate(nextDay.getDate() + 1);
        if (now >= nightTime) nextNight.setDate(nextNight.getDate() + 1);
        if (dayTimer) {
            clearTimeout(dayTimer);
            dayTimer = null;
        }
        if (nightTimer) {
            clearTimeout(nightTimer);
            nightTimer = null;
        }
        dayTimer = setTimeout(function() {
            execFunc("day");
            setupThemeTimers(); // 递归重建，防漂移
        }, nextDay - now);
        nightTimer = setTimeout(function() {
            execFunc("night");
            setupThemeTimers();
        }, nextNight - now);
        var msgstr = "距下一个白天模式还有 " + Math.floor((nextDay - now) / 1000 / 60 / 60) + " 时\n" +
            "距下一个夜晚模式还有 " + Math.floor((nextNight - now) / 1000 / 60 / 60) + " 时";
        // alert(msgstr);
        return isDayTime ? "day" : "night";
    }

    function refreshTheme() {
        var type = getThemeSwitchType();
        var color = getCurrentThemeColor();
        var theme = getSuitableTheme();
        if (type === ThemeType.time) {
            setupThemeTimers(getTimeModeTimeLimit(), refreshTheme);
        }
        var colorstr = "light";
        if (color === ColorType.light) colorstr = "light";
        else if (color === ColorType.dark) colorstr = "dark";
        winjsCss.setAttribute("href", winjsPath.replace("{themecolor}", colorstr));
        themeCss.setAttribute("href", themePath.replace("{themecolor}", colorstr).replace("{id}", theme).replace("{id}", theme));
        setTimeout(function() {
            try { Windows.UI.DPI.mode = 1; } catch (e) {}
        }, 0);
    }

    function setThemeSwitchType(type) {
        var themeType = themeSection.getKey("AppInstaller:ThemeMode");
        var value = "light";
        switch (type) {
            case ThemeType.light:
                value = "light";
                break;
            case ThemeType.dark:
                value = "dark";
                break;
            case ThemeType.auto:
                value = "auto";
                break;
            case ThemeType.time:
                value = "time";
                break;
            case ThemeType.custom:
                value = "custom";
                break;
            default:
                value = "light";
                break;
        }
        themeType.value = value;
    }

    function setTimeModeDayTime(time) {
        var dayTime = themeSection.getKey("AppInstaller:DayTime");
        dayTime.value = time.toISOString();
    }

    function setTimeModeNightTime(time) {
        var nightTime = themeSection.getKey("AppInstaller:NightTime");
        nightTime.value = time.toISOString();
    }

    function setLightTheme(theme) {
        var lightTheme = themeSection.getKey("AppInstaller:LightTheme");
        lightTheme.value = theme;
    }

    function setDarkTheme(theme) {
        var darkTheme = themeSection.getKey("AppInstaller:DarkTheme");
        darkTheme.value = theme;
    }

    function setCustomColor(color) {
        var customColor = themeSection.getKey("AppInstaller:CustomColor");
        var value = "light";
        switch (color) {
            case ColorType.light:
                value = "light";
                break;
            case ColorType.dark:
                value = "dark";
                break;
            default:
                value = "light";
                break;
        }
        customColor.value = value;
    }
    module.exports = {
        Theme: {
            ThemeType: ThemeType,
            ColorType: ColorType,
            getType: getThemeSwitchType,
            getColor: getCurrentThemeColor,
            getTheme: getSuitableTheme,
            getTimeModeTimeLimit: getTimeModeTimeLimit,
            refresh: refreshTheme,
            setType: setThemeSwitchType,
            setDayTime: setTimeModeDayTime,
            setNightTime: setTimeModeNightTime,
            setLightTheme: setLightTheme,
            setDarkTheme: setDarkTheme,
            setCustomColor: setCustomColor
        }
    };
    var themeNS = Theme;
    Object.defineProperty(themeNS, "type", {
        get: function() {
            return themeNS.getType();
        },
        set: function(value) {
            return themeNS.setType(value);
        }
    });
    Object.defineProperty(themeNS, "dayTime", {
        get: function() {
            return themeNS.getTimeModeTimeLimit().day;
        },
        set: function(value) {
            return themeNS.setTimeModeDayTime(value);
        }
    });
    Object.defineProperty(themeNS, "nightTime", {
        get: function() {
            return themeNS.getTimeModeTimeLimit().night;
        },
        set: function(value) {
            return themeNS.setTimeModeNightTime(value);
        }
    });
    Object.defineProperty(themeNS, "lightTheme", {
        get: function() {
            return themeSection.getKey("AppInstaller:LightTheme").value;
        },
        set: function(value) {
            return themeNS.setLightTheme(value);
        }
    });
    Object.defineProperty(themeNS, "darkTheme", {
        get: function() {
            return themeSection.getKey("AppInstaller:DarkTheme").value;
        },
        set: function(value) {
            return themeNS.setDarkTheme(value);
        }
    });
    Object.defineProperty(themeNS, "customColor", {
        get: function() {
            var str = themeSection.getKey("AppInstaller:CustomColor").value;
            if (nstrutil.equals(str, "light")) return ColorType.light;
            else if (nstrutil.equals(str, "dark")) return ColorType.dark;
            else return ColorType.light;
        },
        set: function(value) {
            return themeNS.setCustomColor(value);
        }
    });
    Object.defineProperty(themeNS, "color", {
        get: function() {
            return themeNS.getColor();
        },
    });
    Object.defineProperty(themeNS, "theme", {
        get: function() {
            return themeNS.getTheme();
        },
    });
    Object.defineProperty(themeNS, "currentTheme", {
        get: function() {
            return themeCss.getAttribute("href").split("/").pop().split(".")[0];
        },
        set: function(themeId) {
            var colorstr = "light";
            var color = themeNS.currentColor;
            if (color === ColorType.light) colorstr = "light";
            else if (color === ColorType.dark) colorstr = "dark";
            var href = themePath.replace("{themecolor}", colorstr).replace("{id}", themeId).replace("{id}", themeId);
            themeCss.setAttribute("href", href);
            setTimeout(function() {
                try { Windows.UI.DPI.mode = 1; } catch (e) {}
            }, 0);
        }
    });
    Object.defineProperty(themeNS, "currentColor", {
        get: function() {
            var href = winjsCss.getAttribute("href");
            if (href.indexOf("ui-light") >= 0) return ColorType.light;
            else if (href.indexOf("ui-dark") >= 0) return ColorType.dark;
            else return ColorType.light;
        },
        set: function(color) {
            var colorstr = "light";
            if (color === ColorType.light) colorstr = "light";
            else if (color === ColorType.dark) colorstr = "dark";
            winjsCss.setAttribute("href", winjsPath.replace("{themecolor}", colorstr));
            themeCss.setAttribute("href", themePath.replace("{themecolor}", colorstr).replace("{id}", themeNS.theme).replace("{id}", themeNS.currentTheme));
            setTimeout(function() {
                try { Windows.UI.DPI.mode = 1; } catch (e) {}
            }, 0);
        }
    });
})(this);
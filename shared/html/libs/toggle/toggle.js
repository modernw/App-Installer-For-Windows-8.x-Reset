(function(global) {
    /**
     * 检查 CSS 选择器是否重复（精确匹配）
     * @param {string} targetSelector 要检查的目标选择器（如 ".toggle-switch>input[type="checkbox"]"）
     * @returns {number} 返回该选择器在样式表中的出现次数
     */
    function checkSelectorDuplicate(targetSelector) {
        var count = 0;

        function traverseRules(rules) {
            for (var i = 0; i < rules.length; i++) {
                var rule = rules[i];
                if (rule.type === 1) {
                    if (normalizeSelector(rule.selectorText) === normalizeSelector(targetSelector)) {
                        count++;
                    }
                } else if (rule.type === 4 || rule.type === 12) {
                    var nestedRules = rule.cssRules || rule.rules;
                    if (nestedRules) traverseRules(nestedRules);
                }
            }
        }
        var sheets = document.styleSheets;
        for (var i = 0; i < sheets.length; i++) {
            try {
                var rules = sheets[i].cssRules || sheets[i].rules;
                if (rules) traverseRules(rules);
            } catch (e) {
                console.warn('无法读取跨域样式表:', e);
            }
        }
        return count;
    }

    function normalizeSelector(selector) {
        return selector
            .replace(/\s*([>+~])\s*/g, '$1')
            .replace(/\s{2,}/g, ' ')
            .trim();
    }
    /**
     * 通过选择器移除匹配的 CSS 规则
     * @param {string} targetSelector 要移除的目标选择器（如 ".toggle-switch>input[type="checkbox"]"）
     * @returns {number} 返回实际移除的规则数量
     */
    function removeCSSRulesBySelector(targetSelector) {
        var removedCount = 0;

        function traverseAndRemove(rules) {
            for (var i = rules.length - 1; i >= 0; i--) {
                var rule = rules[i];
                if (rule.type === 1) {
                    if (rule.selectorText === targetSelector) {
                        if (rules.removeRule) {
                            rules.removeRule(i);
                        } else {
                            rules.deleteRule(i);
                        }
                        removedCount++;
                    }
                } else if (rule.type === 4 || rule.type === 12) {
                    var nestedRules = rule.cssRules || rule.rules;
                    if (nestedRules) {
                        traverseAndRemove(nestedRules);
                        if (nestedRules.length === 0) {
                            if (rules.removeRule) {
                                rules.removeRule(i);
                            } else {
                                rules.deleteRule(i);
                            }
                        }
                    }
                }
            }
        }
        var sheets = document.styleSheets;
        for (var i = 0; i < sheets.length; i++) {
            try {
                var rules = sheets[i].cssRules || sheets[i].rules;
                if (rules) traverseAndRemove(rules);
            } catch (e) {
                console.warn('无法操作跨域样式表:', e);
            }
        }
        return removedCount;
    }

    var cssPool = {};

    var strutil = Bridge.External.String;
    var nstrutil = Bridge.NString;
    var boolTrue = ["true", "1", "yes", "on", "y", "t", "zhen", "真"];
    var boolFalse = ["false", "0", "no", "off", "n", "f", "jia", "假"];

    function Toggle() {
        this.element = null;
        Object.defineProperty(this, "value", {
            get: function() {
                return this.getValue();
            },
            set: function(value) {
                this.setValue(value);
            }
        });
        Object.defineProperty(this, "checked", {
            get: function() {
                return this.getValue();
            },
            set: function(value) {
                this.setValue(value);
            }
        });
        Object.defineProperty(this, "disabled", {
            get: function() {
                return this.getDisabled();
            },
            set: function(value) {
                this.setDisabled(value);
            }
        });
        Object.defineProperty(this, "showlabel", {
            get: function() {
                return this.isShowStatus();
            },
            set: function(value) {
                this.showStatus(value);
            }
        });
        Object.defineProperty(this, "darkMode", {
            get: function() {
                return this.isDarkMode();
            },
            set: function(value) {
                this.setDarkMode(value);
            }
        });
        Object.defineProperty(this, "id", {
            get: function() {
                return this.getElementId();
            },
            set: function(value) {
                this.setElementId(value);
            }
        });
        Object.defineProperty(this, "inputId", {
            get: function() {
                return this.getElementInputElementId();
            },
            set: function(value) {
                this.setElementInputElementId(value);
            }
        });
        Object.defineProperty(this, "status", {
            get: function() {
                return this.getElementStatus();
            }
        });
        Object.defineProperty(this, "parent", {
            get: function() {
                return this.getParent();
            },
            set: function(value) {
                this.setParent(value);
            }
        });
    }

    Toggle.prototype.create = function() {
        this.element = document.createElement("label");
        this.element.tabIndex = 0;
        this.element.classList.add("toggle-switch");
        this.element.id = "toggle-switch" + (new Date()).getTime() + Math.floor(Math.random() * 1000);
        var input = document.createElement("input");
        input.type = "checkbox";
        input.id = this.element.id + "-input";
        this.element.addEventListener("keydown", function(event) {
            if (event.keyCode == 13) {
                input.click();
            }
        });
        var back = document.createElement("div");
        back.classList.add("toggle-background");
        var border = document.createElement("div");
        border.classList.add("toggle-switch-border");
        this.element.appendChild(input);
        this.element.appendChild(back);
        this.element.appendChild(border);
        var status = document.createElement("span");
        status.classList.add("toggle-status");
        this.element.appendChild(status);
        status.style.display = "none";
        this.element.objController = this;
        return this.element;
    }

    Toggle.prototype.addEventListener = function(type, listener) {
        this.element.addEventListener(type, listener);
    }

    Toggle.prototype.setParent = function(parent) {
        if (parent == null) {
            return;
        }
        var parentNode = null;
        if (parent instanceof HTMLElement) {
            parentNode = parent;
        } else if (typeof parent === "string" || parent instanceof String) {
            parentNode = document.getElementById(parent);
            if (!parentNode) {
                return;
            }
        } else {
            console.error("Invalid parent type:", parent);
            return;
        }
        if (!(this.element instanceof Node)) {
            console.error("this.element is not a Node:", this.element);
            return;
        }
        parentNode.appendChild(this.element);
    }

    Toggle.prototype.setValue = function(value) {
        var input = this.element.getElementsByTagName("input")[0];
        if (input === null) {
            return;
        }

        parseBool = function(str) {
            str = "" + str;
            for (var i = 0; i < boolTrue.length; i++) {
                if (nstrutil.equals(str, boolTrue[i])) {
                    return true;
                }
            }
            for (var i = 0; i < boolFalse.length; i++) {
                if (nstrutil.equals(str, boolFalse[i])) {
                    return false;
                }
            }
            return null;
        };
        input.checked = parseBool(value);
    }

    Toggle.prototype.getValue = function() {
        var input = this.element.getElementsByTagName("input")[0];
        if (input === null) {
            return false;
        }
        return input.checked;
    }

    Toggle.prototype.setDisabled = function(disabled) {
        var input = this.element.querySelector("input");
        if (input === null) {
            return;
        }
        input.disabled = disabled;
    }

    Toggle.prototype.getDisabled = function() {
        var input = this.element.querySelector("input");
        if (input === null) {
            return false;
        }
        return input.disabled;
    }

    Toggle.prototype.isAvailable = function() {
        return this.element !== null;
    }
    Toggle.prototype.destroy = function() {
        if (this.element !== null) {
            if (this.element.remove) {
                this.element.remove();
            } else {
                this.element.removeNode(true);
            }
            this.element = null;
        }
    }
    Toggle.prototype.getElementId = function() {
        if (this.element === null) {
            return null;
        }
        return this.element.id;
    }
    Toggle.prototype.setElementId = function(elementId) {
        if (this.element === null) {
            return false;
        }
        this.element.id = elementId;
        return true;
    }
    Toggle.prototype.getElement = function() {
        return this.element;
    }
    Toggle.prototype.getElementInput = function() {
        if (this.element === null) {
            return null;
        }
        return this.element.getElementsByTagName("input")[0];
    }
    Toggle.prototype.getElementInputElementId = function() {
        var input = this.getElementInput();
        if (input === null) {
            return null;
        }
        return input.id;
    }
    Toggle.prototype.setElementInputElementId = function(elementId) {
        var input = this.getElementInput();
        if (input === null) {
            return false;
        }
        input.id = elementId;
        return true;
    }
    Toggle.prototype.getElementStatus = function() {
        if (this.element === null) {
            return null;
        }
        return this.element.querySelector(".toggle-status");
    }
    Toggle.prototype.showStatus = function(show) {
        var status = this.element.querySelector(".toggle-status");
        if (status === null) {
            return;
        }
        if (show) {
            status.style.display = "";
            if (this.element) {
                if (this.element.classList.contains("toggle-showstatus")) {
                    return;
                }
                this.element.classList.add("toggle-showstatus");
            }
        } else {
            status.style.display = "none";
            if (this.element) {
                if (!this.element.classList.contains("toggle-showstatus")) {
                    return;
                }
                this.element.classList.remove("toggle-showstatus");
            }
        }
    }
    Toggle.prototype.isShowStatus = function() {
        return this.element.classList.contains("toggle-showstatus");
    }

    Toggle.prototype.setDarkMode = function(isDark) {
        if (this.element === null) {
            return;
        }
        var element = this.element;
        var background = element.querySelector(".toggle-background");
        var border = element.querySelector(".toggle-switch-border");
        var status = element.querySelector(".toggle-status");
        if (isDark) {
            element.classList.add("toggle-switch-dark");
            background.classList.add("toggle-background-dark");
            border.classList.add("toggle-switch-border-dark");
            status.classList.add("toggle-status-dark");
        } else {
            element.classList.remove("toggle-dark");
            background.classList.remove("toggle-background-dark");
            border.classList.remove("toggle-switch-border-dark");
            status.classList.remove("toggle-status-dark");
        }
    }

    Toggle.prototype.isDarkMode = function() {
        if (this.element === null) {
            return false;
        }
        return this.element.classList.contains("toggle-switch-dark");
    }

    function ColorStringToRGBA(input) {
        var str = input.replace(/\s+/g, '').toLowerCase();

        function hexToRgba(hex) {
            hex = hex.slice(1);
            if (hex.length === 3 || hex.length === 4) {
                hex = hex.split('').map(function(c) { return c + c; }).join('');
            }
            var r = parseInt(hex.substr(0, 2), 16);
            var g = parseInt(hex.substr(2, 2), 16);
            var b = parseInt(hex.substr(4, 2), 16);
            var a = hex.length === 8 ? parseInt(hex.substr(6, 2), 16) / 255 : 1;
            return { r: r, g: g, b: b, a: a };
        }

        function hslToRgb(h, s, l) {
            h = (h % 360 + 360) % 360 / 360;
            s = Math.max(0, Math.min(1, s));
            l = Math.max(0, Math.min(1, l));
            var m2 = l <= 0.5 ? l * (s + 1) : l + s - l * s;
            var m1 = 2 * l - m2;

            function hueToRgb(m1, m2, hK) {
                if (hK < 0) hK += 1;
                if (hK > 1) hK -= 1;
                if (hK * 6 < 1) return m1 + (m2 - m1) * 6 * hK;
                if (hK * 2 < 1) return m2;
                if (hK * 3 < 2) return m1 + (m2 - m1) * (2 / 3 - hK) * 6;
                return m1;
            }
            return {
                r: Math.round(hueToRgb(m1, m2, h + 1 / 3) * 255),
                g: Math.round(hueToRgb(m1, m2, h) * 255),
                b: Math.round(hueToRgb(m1, m2, h - 1 / 3) * 255),
                a: 1
            };
        }

        function parseFunc(func, vals) {
            var arr = vals.split(',');
            if (/^hsl/.test(func)) {
                var h = parseFloat(arr[0]);
                var s = parseFloat(arr[1]) / 100;
                var l = parseFloat(arr[2]) / 100;
                var alpha = arr[3] !== undefined ? (arr[3].indexOf('%') > -1 ? parseFloat(arr[3]) / 100 : parseFloat(arr[3])) : 1;
                var rgb = hslToRgb(h, s, l);
                rgb.a = alpha;
                return rgb;
            }
            var out = arr.slice(0, 3).map(function(v, i) {
                if (v.indexOf('%') > -1) return Math.round(parseFloat(v) / 100 * 255);
                return Math.round(parseFloat(v));
            });
            return {
                r: out[0],
                g: out[1],
                b: out[2],
                a: arr[3] !== undefined ? (arr[3].indexOf('%') > -1 ? parseFloat(arr[3]) / 100 : parseFloat(arr[3])) : 1
            };
        }

        function hwbToRgb(h, w, b, a) {
            var rgb = hslToRgb(h, 1, 0.5);
            w = Math.max(0, Math.min(1, w));
            b = Math.max(0, Math.min(1, b));
            var sum = w + b;
            if (sum > 1) {
                var gray = w / sum;
                return { r: Math.round(gray * 255), g: Math.round(gray * 255), b: Math.round(gray * 255), a: a };
            }
            var factor = 1 - w - b;
            return {
                r: Math.round((rgb.r / 255 * factor + w) * 255),
                g: Math.round((rgb.g / 255 * factor + w) * 255),
                b: Math.round((rgb.b / 255 * factor + w) * 255),
                a: a
            };
        }

        function parseRGBString(rgbString) {
            var m = /rgba?\((\d+),\s*(\d+),\s*(\d+)(?:,\s*(\d*\.?\d+))?\)/.exec(rgbString);
            if (m) {
                return { r: +m[1], g: +m[2], b: +m[3], a: m[4] !== undefined ? +m[4] : 1 };
            }
            return { r: 0, g: 0, b: 0, a: 0 };
        }
        var m;
        if ((m = /^#([0-9a-f]{3,8})$/.exec(str))) {
            return hexToRgba(m[0]);
        }
        if ((m = /^(rgba?|hsla?)\((.+)\)$/.exec(str))) {
            return parseFunc(m[1], m[2]);
        }
        if ((m = /^(hwb)a?\((.+)\)$/.exec(str))) {
            var parts = m[2].split(',');
            var h = parseFloat(parts[0]);
            var w0 = parseFloat(parts[1]) / 100;
            var b0 = parseFloat(parts[2]) / 100;
            var a0 = parts[3] !== undefined ? (parts[3].indexOf('%') > -1 ? parseFloat(parts[3]) / 100 : parseFloat(parts[3])) : 1;
            return hwbToRgb(h, w0, b0, a0);
        }
        try {
            var div = document.createElement('div');
            div.style.color = input;
            document.body.appendChild(div);
            var css = window.getComputedStyle(div).color;
            document.body.removeChild(div);
            return parseRGBString(css);
        } catch (e) {
            return { r: 0, g: 0, b: 0, a: 0 };
        }
        return { r: 0, g: 0, b: 0, a: 0 };
    }

    function ColorRGBToString(color) {
        return "#" + ((1 << 24) + (color.r << 16) + (color.g << 8) + color.b).toString(16).slice(1);
    }

    function ColorRGBToHex(color) {
        return "" + ((1 << 24) + (color.r << 16) + (color.g << 8) + color.b).toString(16).slice(1);
    }

    function CalcLightHoverColor(_obj_rgb_) {
        function rgbToHsl(r, g, b) {
            r /= 255, g /= 255, b /= 255;
            var max = Math.max(r, g, b),
                min = Math.min(r, g, b);
            var h, s, l = (max + min) / 2;
            if (max == min) {
                h = s = 0;
            } else {
                var d = max - min;
                s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
                switch (max) {
                    case r:
                        h = (g - b) / d + (g < b ? 6 : 0);
                        break;
                    case g:
                        h = (b - r) / d + 2;
                        break;
                    case b:
                        h = (r - g) / d + 4;
                        break;
                }
                h /= 6;
            }
            return [h * 360, s * 100, l * 100];
        }

        function hslToRgb(h, s, l) {
            h = (h % 360) / 360;
            s /= 100;
            l /= 100;
            var r, g, b;
            if (s == 0) {
                r = g = b = l;
            } else {
                function hue2rgb(p, q, t) {
                    if (t < 0) t += 1;
                    if (t > 1) t -= 1;
                    if (t < 1 / 6) return p + (q - p) * 6 * t;
                    if (t < 1 / 2) return q;
                    if (t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6;
                    return p;
                }
                var q = l < 0.5 ? l * (1 + s) : l + s - l * s;
                var p = 2 * l - q;
                r = hue2rgb(p, q, h + 1 / 3);
                g = hue2rgb(p, q, h);
                b = hue2rgb(p, q, h - 1 / 3);
            }
            return [
                Math.round(r * 255),
                Math.round(g * 255),
                Math.round(b * 255)
            ];
        }
        var hsl = rgbToHsl(_obj_rgb_.r, _obj_rgb_.g, _obj_rgb_.b);
        hsl[2] = Math.min(98, hsl[2] + 12);
        var rgb = hslToRgb(hsl[0], hsl[1], hsl[2]);

        return {
            r: rgb[0],
            g: rgb[1],
            b: rgb[2]
        };
    }

    function CalcLightActiveColor(_obj_rgb_) {
        function rgbToHsl(r, g, b) {
            r /= 255, g /= 255, b /= 255;
            var max = Math.max(r, g, b),
                min = Math.min(r, g, b);
            var h, s, l = (max + min) / 2;
            if (max === min) {
                h = s = 0;
            } else {
                var d = max - min;
                s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
                switch (max) {
                    case r:
                        h = (g - b) / d + (g < b ? 6 : 0);
                        break;
                    case g:
                        h = (b - r) / d + 2;
                        break;
                    case b:
                        h = (r - g) / d + 4;
                        break;
                }
                h /= 6;
            }
            return [h * 360, s * 100, l * 100];
        }

        function hslToRgb(h, s, l) {
            h = (h % 360) / 360;
            s /= 100;
            l /= 100;
            var r, g, b;
            if (s === 0) {
                r = g = b = l;
            } else {
                function hue2rgb(p, q, t) {
                    if (t < 0) t += 1;
                    if (t > 1) t -= 1;
                    if (t < 1 / 6) return p + (q - p) * 6 * t;
                    if (t < 1 / 2) return q;
                    if (t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6;
                    return p;
                }
                var q = l < 0.5 ? l * (1 + s) : l + s - l * s;
                var p = 2 * l - q;
                r = hue2rgb(p, q, h + 1 / 3);
                g = hue2rgb(p, q, h);
                b = hue2rgb(p, q, h - 1 / 3);
            }
            return [
                Math.round(r * 255),
                Math.round(g * 255),
                Math.round(b * 255)
            ];
        }
        var hsl = rgbToHsl(_obj_rgb_.r, _obj_rgb_.g, _obj_rgb_.b);
        hsl[2] = hsl[2] < 50 ?
            hsl[2] * 1.6 :
            hsl[2] + (100 - hsl[2]) * 0.3;
        hsl[1] = hsl[1] < 80 ?
            hsl[1] * 1.3 :
            Math.min(100, hsl[1] + 15);
        hsl[2] = Math.min(95, Math.max(5, hsl[2]));
        hsl[1] = Math.min(100, Math.max(0, hsl[1]));
        var rgb = hslToRgb(hsl[0], hsl[1], hsl[2]);
        return { r: rgb[0], g: rgb[1], b: rgb[2] };
    }

    function GetLightCSSString(_string_selector_, _obj_rgb_) {
        var hover = CalcLightHoverColor(_obj_rgb_);
        var active = CalcLightActiveColor(_obj_rgb_);
        var background = _string_selector_ + "-background";
        var css = "";
        css += "." + background + ":before" + " {" +
            "background-color: " + ColorRGBToString(_obj_rgb_) + ";" +
            "}\n";
        css += ".toggle-switch:hover>input:checked:not(:disabled)+." + background + ":before {" +
            "background-color: " + ColorRGBToString(hover) + ";" +
            "}\n";
        css += ".toggle-switch:active>input:checked:not(:disabled)+." + background + ":before {" +
            "background-color: " + ColorRGBToString(active) + ";" +
            "}\n";
        return css;
    }

    function GetDarkCSSString(_string_selector_, _obj_rgb_) {
        function rgbToHsl(r, g, b) {
            r /= 255;
            g /= 255;
            b /= 255;
            var max = Math.max(r, g, b),
                min = Math.min(r, g, b),
                h, s, l = (max + min) / 2;
            if (max === min) {
                h = s = 0;
            } else {
                var d = max - min;
                s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
                switch (max) {
                    case r:
                        h = ((g - b) / d + (g < b ? 6 : 0));
                        break;
                    case g:
                        h = ((b - r) / d + 2);
                        break;
                    case b:
                        h = ((r - g) / d + 4);
                        break;
                }
                h /= 6;
            }
            return { h: h * 360, s: s * 100, l: l * 100 };
        }

        function hslToRgb(h, s, l) {
            s /= 100;
            l /= 100;
            h /= 360;

            function hue2rgb(p, q, t) {
                if (t < 0) t += 1;
                if (t > 1) t -= 1;
                if (t < 1 / 6) return p + (q - p) * 6 * t;
                if (t < 1 / 2) return q;
                if (t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6;
                return p;
            }
            var r, g, b;
            if (!s) {
                r = g = b = l;
            } else {
                var q = l < 0.5 ? l * (1 + s) : l + s - l * s;
                var p = 2 * l - q;
                r = hue2rgb(p, q, h + 1 / 3);
                g = hue2rgb(p, q, h);
                b = hue2rgb(p, q, h - 1 / 3);
            }
            return { r: Math.round(r * 255), g: Math.round(g * 255), b: Math.round(b * 255) };
        }

        function clamp(v, min, max) {
            return v < min ? min : (v > max ? max : v);
        }

        function ColorRGBToString(c) {
            return "rgb(" + c.r + ", " + c.g + ", " + c.b + ")";
        }
        var hsl = rgbToHsl(_obj_rgb_.r, _obj_rgb_.g, _obj_rgb_.b);
        var borderRGB = hslToRgb(hsl.h, hsl.s, clamp(hsl.l + 15, 0, 100));
        var backgroundRGB = hslToRgb(hsl.h, clamp(hsl.s * 0.9, 0, 100), clamp(hsl.l + 12, 0, 100));
        var disabledBgRGB = hslToRgb(hsl.h, clamp(hsl.s * 0.6, 0, 100), clamp(hsl.l + 5, 0, 100));
        var disabledBrdRGB = hslToRgb(hsl.h, hsl.s, clamp(hsl.l + 5, 0, 100));
        var hoverBgRGB = hslToRgb(hsl.h, clamp(hsl.s * 0.8, 0, 100), clamp(hsl.l + 25, 0, 100));
        var activeBgRGB = hslToRgb(hsl.h, clamp(hsl.s * 1.1, 0, 100), clamp(hsl.l + 35, 0, 100));
        var hoverChecked = CalcLightHoverColor(_obj_rgb_);
        var activeChecked = CalcLightActiveColor(_obj_rgb_);
        var base = _string_selector_;
        var bgClass = base + "-background";
        var brClass = base + "-border";
        var css = "";
        css += "." + base + " { border-color: " + ColorRGBToString(borderRGB) + "; }\n";
        css += "." + bgClass + " { background-color: " + ColorRGBToString(backgroundRGB) + "; }\n";
        css += "." + base + ">input[type=\"checkbox\"]:disabled:not(:checked)+." + bgClass +
            " { background-color: " + ColorRGBToString(disabledBgRGB) + "; }\n";
        css += "." + base + ">input[type=\"checkbox\"]:disabled~." + brClass + "," +
            "." + base + ">input[type=\"checkbox\"]:disabled~." + base + "-switch" +
            " { border-color: " + ColorRGBToString(disabledBrdRGB) + "; }\n";
        css += "." + base + ":hover>input[type=\"checkbox\"]:not(:disabled):not(:checked)+." + bgClass +
            " { background-color: " + ColorRGBToString(hoverBgRGB) + "; }\n";
        css += "." + base + ":active>input[type=\"checkbox\"]:not(:disabled):not(:checked)+." + bgClass +
            " { background-color: " + ColorRGBToString(activeBgRGB) + "; }\n";
        css += "." + brClass + " { border-color:" + ColorRGBToString(borderRGB) + "; }\n";
        css += "." + base + ">input:disabled+." + base + "-background:after" +
            " { background-color: " + ColorRGBToString(borderRGB) + "; }\n";
        css += "." + bgClass + ":before { background-color: " + ColorRGBToString(_obj_rgb_) + "; }\n";
        css += "." + base + ":hover>input:checked:not(:disabled)+." + bgClass + ":before" +
            " { background-color: " + ColorRGBToString(hoverChecked) + "; }\n";
        css += "." + base + ":active>input:checked:not(:disabled)+." + bgClass + ":before" +
            " { background-color: " + ColorRGBToString(activeChecked) + "; }\n";
        return css;
    }

    Toggle.prototype.setDarkMode = function(dark) {
        var element = this.element;
        if (this.isDarkMode === dark) {
            return;
        }
        var background = element.querySelector(".toggle-background");
        var border = element.querySelector(".toggle-switch-border");
        var status = element.querySelector(".toggle-status");
        if (this.color) {
            var label = ColorRGBToHex(this.color);
            var uniid = "toggle-theme-" + label;
            var uniidlight = uniid + "-light";
            var uniiddark = uniid + "-dark";
        }
        if (this.element === null) {
            return;
        }
        if (dark) {
            element.classList.add("toggle-switch-dark");
            background.classList.add("toggle-background-dark");
            border.classList.add("toggle-switch-border-dark");
            status.classList.add("toggle-status-dark");
            if (this.color) {
                element.classList.add(uniiddark);
                background.classList.add(uniiddark + "-background");
                border.classList.add(uniiddark + "-border");
                element.classList.remove(uniidlight);
                background.classList.remove(uniidlight + "-background");
                border.classList.remove(uniidlight + "-border");
            }
        } else {
            element.classList.remove("toggle-dark");
            background.classList.remove("toggle-background-dark");
            border.classList.remove("toggle-switch-border-dark");
            status.classList.remove("toggle-status-dark");
            if (this.color) {
                element.classList.remove(uniiddark);
                background.classList.remove(uniiddark + "-background");
                border.classList.remove(uniiddark + "-border");
                element.classList.add(uniidlight);
                background.classList.add(uniidlight + "-background");
                border.classList.add(uniidlight + "-border");
            }
        }
    }

    Toggle.prototype.setColor = function(color) {
        var rgb = ColorStringToRGBA(color);
        this.color = rgb;
        var label = ColorRGBToHex(rgb);
        var uniid = "toggle-theme-" + label;
        var uniidlight = uniid + "-light";
        var uniiddark = uniid + "-dark";
        if (cssPool[uniid] === undefined || cssPool[uniid] === null) {
            cssPool[uniid] = {
                light: uniidlight,
                dark: uniiddark,
                count: 1,
            }
            var lightCSS = GetLightCSSString(uniidlight, rgb);
            var darkCSS = GetDarkCSSString(uniiddark, rgb);
            var style = document.createElement("style");
            style.type = "text/css";
            style.id = uniid;
            style.innerHTML = "/* Light Theme */\n" + lightCSS + "\n/* Dark Theme */\n" + darkCSS;
            document.head.appendChild(style);
            cssPool[uniid].style = style;
        } else {
            cssPool[uniid].count++;
        }
        var dark = this.isDarkMode();
        this.setDarkMode(!dark);
        this.setDarkMode(dark);
    }

    Toggle.prototype.dispose = function() {
        if (this.element) {
            removeElement(this.element);
            this.element = null;
        }
        if (this.color) {
            var label = ColorRGBToHex(this.color);
            var uniid = "toggle-theme-" + label;
            var ucss = cssPool[uniid];
            if (ucss) {
                ucss.count--;
                if (ucss.count <= 0) {
                    removeElement(ucss.style);
                    delete cssPool[uniid];
                }
            }
            this.color = null;
        }
    };

    Toggle.prototype.destroy = Toggle.prototype.dispose;

    Toggle.prototype.setStatusText = function(onText, offText) {
        /**
         * 动态插入一段 CSS 文本到页面 <head> 中
         * @param {string} cssText  - 要插入的完整 CSS 文本，如 "body{background:red;}"
         * @param {string} [id]     - 可选，为 <style> 元素指定 id，方便后面删除
         */
        function addCssText(cssText, id) {
            var head = document.head || document.getElementsByTagName('head')[0];
            var style = document.createElement('style');

            if (id) style.id = id;
            style.type = 'text/css';

            // 对 IE8- 兼容：style.styleSheet.cssText；其他浏览器用 textNode
            if (style.styleSheet) {
                style.styleSheet.cssText = cssText; // :contentReference[oaicite:0]{index=0}
            } else {
                style.appendChild(document.createTextNode(cssText));
            }
            head.appendChild(style);
        }
        /**
         * 根据 id 移除之前插入的 <style> 标签
         * @param {string} id   - addCssText 时指定的 id
         */
        function removeCssById(id) {
            var style = document.getElementById(id);
            if (style && style.parentNode) {
                style.parentNode.removeChild(style);
            }
        }
        removeCssById("toggle-status-lang");
        var status = this.element.querySelector(".toggle-status");
        var csstext = ".toggle-status-lang:before { content: \"" + offText + "\"; }\n";
        csstext += "label.toggle-status-lang:has(input:checked)::before { content: \"" + onText + "\"; }\n";
        csstext += ".toggle-switch>input:checked~.toggle-status-lang::before { content: \"" + onText + "\"; }\n";
        addCssText(csstext, "toggle-status-lang");
        status.classList.add("toggle-status-lang");
    }
    global.Toggle = Toggle;
})(this);
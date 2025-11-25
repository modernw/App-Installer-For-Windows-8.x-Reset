(function(global) {
    "use strict";

    function RGB(parent) {
        Object.defineProperty(this, "red", {
            get: function() { return parent.red; },
            set: function(value) { parent.red = value; }
        });
        Object.defineProperty(this, "green", {
            get: function() { return parent.green; },
            set: function(value) { parent.green = value; }
        });
        Object.defineProperty(this, "blue", {
            get: function() { return parent.blue; },
            set: function(value) { parent.blue = value; }
        });
        this.toString = function() { return "rgb(" + parent.red + "," + parent.green + "," + parent.blue + ")"; };
        this.valueOf = function() { return parent.valueOf(); };
        this.convert = function(type) { return new type(parent); };
        Object.defineProperty(this, "hex", {
            get: function() { return parent.hex; },
            set: function(value) { parent.hex = value; }
        });
        Object.defineProperty(this, "color", {
            get: function() { return parent; }
        });
        this.stringify = function() { return "rgb(" + parent.red + "," + parent.green + "," + parent.blue + ")"; };
    }

    function RGBA(parent) {
        parent.RGB.constructor.call(this, parent);
        Object.defineProperty(this, "alpha", {
            get: function() { return parent.alpha; },
            set: function(value) { parent.alpha = value; }
        });
        this.toString = function() { return "rgba(" + parent.red + "," + parent.green + "," + parent.blue + "," + (parent.alpha / 255).toFixed(2) + ")"; };
        this.valueOf = function() { return parent.valueOf(); };
        this.stringify = function() { return "rgba(" + parent.red + "," + parent.green + "," + parent.blue + "," + (parent.alpha / 255).toFixed(2) + ")"; };
    }

    function HSL(parent) {
        parent.RGB.constructor.call(this, parent);
        /**
         * @type {number} 色调 0 - 360
         */
        Object.defineProperty(this, "hue", {
            get: function() {
                var r = parent.red / 255,
                    g = parent.green / 255,
                    b = parent.blue / 255;
                var max = Math.max(r, g, b),
                    min = Math.min(r, g, b);
                var h, s, l = (max + min) / 2;
                if (max == min) {
                    h = s = 0; // achromatic
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
                return h * 360;
            },
            set: function(value) {
                var r = parent.red / 255,
                    g = parent.green / 255,
                    b = parent.blue / 255;
                var max = Math.max(r, g, b),
                    min = Math.min(r, g, b);
                var h = value / 360,
                    s = (max == 0 ? 0 : (max - min) / (max + min)),
                    l = (max + min) / 2;
                if (s == 0) {
                    r = g = b = l; // achromatic
                } else {
                    var hue2rgb = function hue2rgb(p, q, t) {
                        if (t < 0) t += 1;
                        if (t > 1) t -= 1;
                        if (t < 1 / 6) return p + (q - p) * 6 * t;
                        if (t < 1 / 2) return q;
                        if (t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6;
                        return p;
                    };
                    var q = l < 0.5 ? l * (1 + s) : l + s - l * s;
                    var p = 2 * l - q;
                    r = hue2rgb(p, q, h + 1 / 3);
                    g = hue2rgb(p, q, h);
                    b = hue2rgb(p, q, h - 1 / 3);
                }
                parent.red = Math.round(r * 255);
                parent.green = Math.round(g * 255);
                parent.blue = Math.round(b * 255);
            }
        });
        /**
         * @type {number} 饱和度 0 - 1
         */
        Object.defineProperty(this, "saturation", {
            get: function() {
                var r = parent.red / 255,
                    g = parent.green / 255,
                    b = parent.blue / 255;
                var max = Math.max(r, g, b),
                    min = Math.min(r, g, b);
                var h, s, l = (max + min) / 2;
                if (max == min) {
                    s = 0; // achromatic
                } else {
                    var d = max - min;
                    s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
                }
                return s;
            },
            set: function(value) {
                var r = parent.red / 255,
                    g = parent.green / 255,
                    b = parent.blue / 255;
                var max = Math.max(r, g, b),
                    min = Math.min(r, g, b);
                var h = parent.hue / 360,
                    s = value,
                    l = (max + min) / 2;
                if (s == 0) {
                    r = g = b = l; // achromatic
                } else {
                    var hue2rgb = function hue2rgb(p, q, t) {
                        if (t < 0) t += 1;
                        if (t > 1) t -= 1;
                        if (t < 1 / 6) return p + (q - p) * 6 * t;
                        if (t < 1 / 2) return q;
                        if (t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6;
                        return p;
                    };
                    var q = l < 0.5 ? l * (1 + s) : l + s - l * s;
                    var p = 2 * l - q;
                    r = hue2rgb(p, q, h + 1 / 3);
                    g = hue2rgb(p, q, h);
                    b = hue2rgb(p, q, h - 1 / 3);
                }
                parent.red = Math.round(r * 255);
                parent.green = Math.round(g * 255);
                parent.blue = Math.round(b * 255);
            }
        });
        /**
         * @type {number} 亮度 0 - 1
         */
        Object.defineProperty(this, "lightness", {
            get: function() {
                var r = parent.red / 255,
                    g = parent.green / 255,
                    b = parent.blue / 255;
                var max = Math.max(r, g, b),
                    min = Math.min(r, g, b);
                var h, s, l = (max + min) / 2;
                return l;
            },
            set: function(value) {
                var r = parent.red / 255,
                    g = parent.green / 255,
                    b = parent.blue / 255;
                var max = Math.max(r, g, b),
                    min = Math.min(r, g, b);
                var h = parent.hue / 360,
                    s = parent.saturation,
                    l = value;
                if (s == 0) {
                    r = g = b = l; // achromatic
                } else {
                    var hue2rgb = function hue2rgb(p, q, t) {
                        if (t < 0) t += 1;
                        if (t > 1) t -= 1;
                        if (t < 1 / 6) return p + (q - p) * 6 * t;
                        if (t < 1 / 2) return q;
                        if (t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6;
                        return p;
                    };
                    var q = l < 0.5 ? l * (1 + s) : l + s - l * s;
                    var p = 2 * l - q;
                    r = hue2rgb(p, q, h + 1 / 3);
                    g = hue2rgb(p, q, h);
                    b = hue2rgb(p, q, h - 1 / 3);
                }
                parent.red = Math.round(r * 255);
                parent.green = Math.round(g * 255);
                parent.blue = Math.round(b * 255);
            }
        });
        this.toString = function() { return "hsl(" + this.hue + "," + (this.saturation * 100).toFixed(2) + "%," + (this.lightness * 100).toFixed(2) + "%)"; };
        this.valueOf = function() { return parent.valueOf(); };
        this.stringify = function() { return "hsl(" + this.hue + "," + (this.saturation * 100).toFixed(2) + "%," + (this.lightness * 100).toFixed(2) + "%)"; };
    }

    function HSLA(parent) {
        parent.HSL.constructor.call(this, parent);
        Object.defineProperty(this, "alpha", {
            get: function() { return parent.alpha; },
            set: function(value) { parent.alpha = value; }
        });
        this.toString = function() { return "hsla(" + this.hue + "," + (this.saturation * 100).toFixed(2) + "%," + (this.lightness * 100).toFixed(2) + "%," + (parent.alpha / 255).toFixed(2) + ")"; };
        this.valueOf = function() { return parent.valueOf(); };
        this.stringify = function() { return "hsla(" + this.hue + "," + (this.saturation * 100).toFixed(2) + "%," + (this.lightness * 100).toFixed(2) + "%," + (parent.alpha / 255).toFixed(2) + ")"; };
    }

    function HWB(parent) {
        parent.RGB.constructor.call(this, parent);
        /**
         * @type {number} 色调 0 - 360
         */
        Object.defineProperty(this, "hue", {
            get: function() {
                var r = parent.red / 255,
                    g = parent.green / 255,
                    b = parent.blue / 255;
                var max = Math.max(r, g, b),
                    min = Math.min(r, g, b);
                var h, w, b = (max + min) / 2;
                if (max == min) {
                    h = w = 0; // achromatic
                } else {
                    var d = max - min;
                    w = (max == 0 ? 0 : d / max);
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
                return h * 360;
            },
            set: function(value) {
                var r = parent.red / 255,
                    g = parent.green / 255,
                    b = parent.blue / 255;
                var max = Math.max(r, g, b),
                    min = Math.min(r, g, b);
                var h = value / 360,
                    w = parent.white,
                    b = (1 - w) * max;
                if (max == min) {
                    r = g = b = max; // achromatic
                } else {
                    var hue2rgb = function hue2rgb(p, q, t) {
                        if (t < 0) t += 1;
                        if (t > 1) t -= 1;
                        if (t < 1 / 6) return p + (q - p) * 6 * t;
                        if (t < 1 / 2) return q;
                        if (t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6;
                        return p;
                    };
                    var q = b < 0.5 ? b * (1 + w) : b + w - b * w;
                    var p = 2 * b - q;
                    r = hue2rgb(p, q, h + 1 / 3);
                    g = hue2rgb(p, q, h);
                    b = hue2rgb(p, q, h - 1 / 3);
                }
                parent.red = Math.round(r * 255);
                parent.green = Math.round(g * 255);
                parent.blue = Math.round(b * 255);
            }
        });
        /**
         * @type {number} 白色分量 0 - 1
         */
        Object.defineProperty(this, "white", {
            get: function() {
                var r = parent.red / 255,
                    g = parent.green / 255,
                    b = parent.blue / 255;
                var max = Math.max(r, g, b),
                    min = Math.min(r, g, b);
                var h, w, b = (max + min) / 2;
                if (max == min) {
                    w = 0; // achromatic
                } else {
                    var d = max - min;
                    w = (max == 0 ? 0 : d / max);
                }
                return w;
            },
            set: function(value) {
                var r = parent.red / 255,
                    g = parent.green / 255,
                    b = parent.blue / 255;
                var max = Math.max(r, g, b),
                    min = Math.min(r, g, b);
                var h = parent.hue / 360,
                    w = value,
                    b = (1 - w) * max;
                if (max == min) {
                    r = g = b = max; // achromatic
                } else {
                    var hue2rgb = function hue2rgb(p, q, t) {
                        if (t < 0) t += 1;
                        if (t > 1) t -= 1;
                        if (t < 1 / 6) return p + (q - p) * 6 * t;
                        if (t < 1 / 2) return q;
                        if (t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6;
                        return p;
                    };
                    var q = b < 0.5 ? b * (1 + w) : b + w - b * w;
                    var p = 2 * b - q;
                    r = hue2rgb(p, q, h + 1 / 3);
                    g = hue2rgb(p, q, h);
                    b = hue2rgb(p, q, h - 1 / 3);
                }
                parent.red = Math.round(r * 255);
                parent.green = Math.round(g * 255);
                parent.blue = Math.round(b * 255);
            }
        });
        Object.defineProperty(this, "black", {
            get: function() {
                var r = parent.red / 255,
                    g = parent.green / 255,
                    b = parent.blue / 255;
                var max = Math.max(r, g, b),
                    min = Math.min(r, g, b);
                var h, w, b = (max + min) / 2;
                if (max == min) {
                    w = 0; // achromatic
                } else {
                    var d = max - min;
                    w = (max == 0 ? 0 : d / max);
                }
                return 1 - w;
            },
            set: function(value) {
                var r = parent.red / 255,
                    g = parent.green / 255,
                    b = parent.blue / 255;
                var max = Math.max(r, g, b),
                    min = Math.min(r, g, b);
                var h = parent.hue / 360,
                    w = 1 - value,
                    b = (1 - w) * max;
                if (max == min) {
                    r = g = b = max; // achromatic
                } else {
                    var hue2rgb = function hue2rgb(p, q, t) {
                        if (t < 0) t += 1;
                        if (t > 1) t -= 1;
                        if (t < 1 / 6) return p + (q - p) * 6 * t;
                        if (t < 1 / 2) return q;
                        if (t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6;
                        return p;
                    };
                    var q = b < 0.5 ? b * (1 + w) : b + w - b * w;
                    var p = 2 * b - q;
                    r = hue2rgb(p, q, h + 1 / 3);
                    g = hue2rgb(p, q, h);
                    b = hue2rgb(p, q, h - 1 / 3);
                }
                parent.red = Math.round(r * 255);
                parent.green = Math.round(g * 255);
                parent.blue = Math.round(b * 255);
            }
        });
        Object.defineProperty(this, "alpha", {
            get: function() { return parent.alpha; },
            set: function(value) { parent.alpha = value; }
        });
        this.toString = function() {
            if (parent.alpha == 255) {
                return "hwb(" + this.hue + "," + (this.white * 100).toFixed(2) + "%," + (this.black * 100).toFixed(2) + "%)";
            } else {
                return "hwb(" + this.hue + "," + (this.white * 100).toFixed(2) + "%," + (this.black * 100).toFixed(2) + "% / " + (parent.alpha / 255).toFixed(2) + ")";
            }
        };
        this.valueOf = function() { return parent.valueOf(); };
        this.stringify = function() {
            if (parent.alpha == 255) {
                return "hwb(" + this.hue + "," + (this.white * 100).toFixed(2) + "%," + (this.black * 100).toFixed(2) + "%)";
            } else {
                return "hwb(" + this.hue + "," + (this.white * 100).toFixed(2) + "%," + (this.black * 100).toFixed(2) + "% / " + (parent.alpha / 255).toFixed(2) + ")";
            }
        };
    }
    /**
     * 
     * @param {number} red 红色通道值 0-255
     * @param {number} green 绿色通道值 0-255
     * @param {number} blue 蓝色通道值 0-255
     * @param {number} alpha 透明度通道值 0-255
     */
    function Color(red, green, blue, alpha) {
        this._red = red & 0xFF;
        this._green = green & 0xFF;
        this._blue = blue & 0xFF;
        this._alpha = (typeof alpha === "undefined") ? 255 : (alpha & 0xFF);
        Object.defineProperty(this, "rgbData", {
            get: function() { return this._rgbData; },
            set: function(value) { this._rgbData = value & 0xFFFFFF; }
        });
        // 红色通道
        Object.defineProperty(this, "red", {
            get: function() { return this._red; },
            set: function(value) { this._red = value & 0xFF; }
        });
        // 绿色通道
        Object.defineProperty(this, "green", {
            get: function() { return this._green; },
            set: function(value) { this._green = value & 0xFF; }
        });
        // 蓝色通道
        Object.defineProperty(this, "blue", {
            get: function() { return this._blue; },
            set: function(value) { this._blue = value & 0xFF; }
        });
        // Alpha 通道单独存储
        Object.defineProperty(this, "alpha", {
            get: function() { return this._alpha; },
            set: function(value) { this._alpha = value & 0xFF; }
        });
        // hex 属性
        Object.defineProperty(this, "hex", {
            get: function() {
                function padZero(str, length) {
                    while (str.length < length) str = "0" + str;
                    return str;
                }
                var r = padZero(this.red.toString(16), 2),
                    g = padZero(this.green.toString(16), 2),
                    b = padZero(this.blue.toString(16), 2),
                    a = padZero(this.alpha.toString(16), 2);
                return this.alpha === 255 ? "#" + r + g + b : "#" + r + g + b + a;
            },
            set: function(value) {
                var hex = value.replace(/^#/, "");
                if (hex.length === 3) hex = hex[0] + hex[0] + hex[1] + hex[1] + hex[2] + hex[2];
                if (hex.length === 6) hex = hex + "ff";
                if (hex.length === 8) {
                    this.red = parseInt(hex.substr(0, 2), 16);
                    this.green = parseInt(hex.substr(2, 2), 16);
                    this.blue = parseInt(hex.substr(4, 2), 16);
                    this.alpha = parseInt(hex.substr(6, 2), 16);
                }
            }
        });
        this.toString = function() { return this.hex; };
        this.valueOf = function() {
            function padZero(str, length) {
                while (str.length < length) str = "0" + str;
                return str;
            }
            var r = padZero(this.red.toString(16), 2),
                g = padZero(this.green.toString(16), 2),
                b = padZero(this.blue.toString(16), 2),
                a = padZero(this.alpha.toString(16), 2);
            return "#" + r + g + b + a;
        };
        this.RGB = new RGB(this);
        this.RGBA = new RGBA(this);
        this.HSL = new HSL(this);
        this.HSLA = new HSLA(this);
        this.HWB = new HWB(this);
        this.stringify = function() { return this.hex; };
    }
    /**
     * 解析颜色字符串
     * @param {string} str 颜色字符串 
     * @returns {Color} 解析得到的颜色对象
     */
    Color.parse = function(str) {
        var json = JSON.parse(window.external.IEFrame.ParseHtmlColor(str));
        return new Color(json.r, json.g, json.b, json.a);
    }
    module.exports = { Color: Color };
})(this);
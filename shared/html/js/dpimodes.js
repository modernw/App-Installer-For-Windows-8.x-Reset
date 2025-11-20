(function(global) {
    "use strict";
    var highDpiMode = 0; // 默认不进行转换
    var dpiValue = 1.0; // 默认 100%

    function IsIeVersionElder() {
        return Bridge.Frame.version < 11;
    }
    // 0 - 不使用
    // 1 - 转换为 px
    // 2 - 转换为 pt
    function setHighDpiMode(modeType) {
        highDpiMode = modeType;
        refreshModeDisplay();
    }

    function refreshModeDisplay() {
        switch (highDpiMode) {
            case 1:
                convertAllPtToPx();
                break;
            case 2:
                convertAllPxToPt();
                break;
        }
        var images = document.getElementsByTagName('img');
        for (var i = 0; i < images.length; i++) {
            var img = images[i];
            var parent = img.parentElement;
            if (!parent) continue;
            var parentWidth = parent.offsetWidth;
            var parentHeight = parent.offsetHeight;
            var scaledWidth = img.naturalWidth * dpiValue;
            var scaledHeight = img.naturalHeight * dpiValue;
            if (scaledWidth > parentWidth || scaledHeight > parentHeight) {
                img.style.transform = 'none';
            } else {
                img.style.transform = 'scale(' + dpiValue + ')';
            }
            // console.log('Image:', img, 'Parent Size:', parentWidth, parentHeight, 'Scaled:', scaledWidth, scaledHeight, 'Transform:', img.style.transform);
        }
        if (IsIeVersionElder && IsIeVersionElder()) {
            var contentElements = document.getElementsByClassName('content');
            var controlElements = document.getElementsByClassName('control-column-bottom');
            if (controlElements.length === 0 || contentElements.length === 0) {
                console.error('未找到相应元素');
            } else {
                var controlElement = controlElements[0];
                var vheight = controlElement.currentStyle ?
                    controlElement.currentStyle.height :
                    window.getComputedStyle(controlElement).height;
                for (var i = 0; i < contentElements.length; i++) {
                    var element = contentElements[i];
                    element.style.height = 'calc(100% - ' + vheight + ')';
                    // console.log('Set height of', element, 'to', 'calc(100% - ' + vheight + ')');
                }
            }
        }
    }

    function convertAllPtToPx() {
        var conversionFactor = 96 / 72;
        var allElements = document.getElementsByTagName("*");
        for (var i = 0; i < allElements.length; i++) {
            var el = allElements[i];
            if (el.style && el.style.cssText) {
                el.style.cssText = el.style.cssText.replace(/(\d+(\.\d+)?)pt/g, function(match, p1) {
                    var pxValue = parseFloat(p1) * conversionFactor;
                    return pxValue + "px";
                });
            }
        }
        for (var i = 0; i < document.styleSheets.length; i++) {
            var styleSheet = document.styleSheets[i];
            try {
                var rules = styleSheet.cssRules || styleSheet.rules;
                if (rules) {
                    for (var j = 0; j < rules.length; j++) {
                        var rule = rules[j];
                        if (rule.style) {
                            for (var k = 0; k < rule.style.length; k++) {
                                var propertyName = rule.style[k];
                                var value = rule.style.getPropertyValue(propertyName);
                                if (value.indexOf("pt") !== -1) {
                                    var newValue = value.replace(/(\d+(\.\d+)?)pt/g, function(match, p1) {
                                        var pxValue = parseFloat(p1) * conversionFactor;
                                        return pxValue + "px";
                                    });
                                    var priority = (rule.style.getPropertyPriority) ? rule.style.getPropertyPriority(propertyName) : "";
                                    rule.style.setProperty(propertyName, newValue, priority);
                                }
                            }
                        }
                    }
                }
            } catch (e) {
                console.warn("Could not access stylesheet", styleSheet.href, e);
            }
        }
    }

    function convertAllPxToPt() {
        var conversionFactor = 72 / 96;
        var allElements = document.getElementsByTagName("*");
        for (var i = 0; i < allElements.length; i++) {
            var el = allElements[i];
            if (el.style && el.style.cssText) {
                el.style.cssText = el.style.cssText.replace(/(\d+(\.\d+)?)px/g, function(match, p1) {
                    var pxValue = parseFloat(p1) * conversionFactor;
                    return pxValue + "pt";
                });
            }
        }
        for (var i = 0; i < document.styleSheets.length; i++) {
            var styleSheet = document.styleSheets[i];
            try {
                var rules = styleSheet.cssRules || styleSheet.rules;
                if (rules) {
                    for (var j = 0; j < rules.length; j++) {
                        var rule = rules[j];
                        if (rule.style) {
                            for (var k = 0; k < rule.style.length; k++) {
                                var propertyName = rule.style[k];
                                var value = rule.style.getPropertyValue(propertyName);
                                if (value.indexOf("px") !== -1) {
                                    var newValue = value.replace(/(\d+(\.\d+)?)px/g, function(match, p1) {
                                        var pxValue = parseFloat(p1) * conversionFactor;
                                        return pxValue + "pt";
                                    });
                                    var priority = (rule.style.getPropertyPriority) ? rule.style.getPropertyPriority(propertyName) : "";
                                    rule.style.setProperty(propertyName, newValue, priority);
                                }
                            }
                        }
                    }
                }
            } catch (e) {
                console.warn("Could not access stylesheet", styleSheet.href, e);
            }
        }
    }

    function setPageZoom(zoomLevel) {
        // document.body.style.zoom = zoomLevel;
        dpiValue = zoomLevel;
        refreshModeDisplay();
    }

    module.exports = {
        Windows: {
            UI: {
                DPI: new function() {
                    Object.defineProperty(this, "mode", {
                        get: function() {
                            return highDpiMode;
                        },
                        set: function(modeType) {
                            setHighDpiMode(modeType);
                            return highDpiMode;
                        }
                    });
                    this.refresh = refreshModeDisplay;
                    Object.defineProperty(this, "zoom", {
                        get: function() {
                            return dpiValue;
                        },
                        set: function(zoomLevel) {
                            setPageZoom(zoomLevel);
                            return dpiValue;
                        }
                    });
                    this.ptToPx = convertAllPtToPx;
                    this.pxToPt = convertAllPxToPt;
                }()
            }
        }
    };
})(this);
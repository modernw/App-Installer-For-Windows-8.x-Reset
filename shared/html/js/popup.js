(function(global) {
    "use strict";
    var flags = {
        HIDE_WHEN_CLICK_OUTSIDE: 0x00000001,
        SHOW_CONTROL_BUTTONS: 0x00000002,
    };

    function Popup(content, options) {
        var _content = content;
        var _flags = options["flags"] || 0;
        var _element = null;
        _element = document.createElement("div");
        _element.className = "popup";
        _element.style.position = "absolute";
        _element.style.display = "none";
        document.body.appendChild(_element);
        if (_content instanceof HTMLElement) {
            _element.appendChild(_content);
        } else if (typeof _content === "string") {
            _element.textContent = _content;
        }
        Object.defineProperty(this, "content", {
            get: function() { return _content; },
            set: function(value) {
                _element.innerHTML = "";
                if (value instanceof HTMLElement) {
                    _element.appendChild(value);
                } else if (typeof value === "string") {
                    _element.textContent = value;
                }
                _content = value;
            }
        });
        Object.defineProperty(this, "", {

        });
    }

    Popup.Options = function() {
        this.flags = 0;
        this.position = {
            left: null,
            top: null
        };
        this.size = {
            width: null,
            height: null
        };
    };

    module.exports = {
        Windows: {
            UI: {
                Popups: {
                    DisplayBlock: Popup
                }
            }
        }
    };
})(this);
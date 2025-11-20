(function(global) {
    "use strict";

    if (!global.Windows) global.Windows = {};
    if (!global.Windows.UI) global.Windows.UI = {};
    if (!global.Windows.UI.Event) global.Windows.UI.Event = {};

    var Monitor = (function() {
        var _sIdAttr = "data-monitor-id";
        var _idCounter = 1;
        var _aRegistry = {};
        var _typeRegistry = {}; // 按事件类型分类缓存
        var _polling = false;
        var _loopHandle = null;
        var _cleanupThreshold = 30000; // 30秒清理一次
        var _lastCleanup = Date.now();
        var _checkInterval = 200; // 节流时间
        var _eventTypes = [
            "resize",
            "position",
            "attribute",
            "child"
        ];

        // 缓存 DOM 元素引用
        var _elementCache = {};

        function _ensureId(el) {
            if (!el.getAttribute(_sIdAttr)) {
                el.setAttribute(_sIdAttr, "monitor_" + (_idCounter++));
            }
            return el.getAttribute(_sIdAttr);
        }

        function _getElementById(id) {
            if (_elementCache[id] && _elementCache[id].parentNode) {
                return _elementCache[id];
            }
            var el = document.querySelector("[" + _sIdAttr + "=\"" + id + "\"]");
            if (el) _elementCache[id] = el;
            return el;
        }

        function _getAttrSnapshot(el) {
            var attrs = {};
            for (var i = 0; i < el.attributes.length; i++) {
                var attr = el.attributes[i];
                attrs[attr.name] = attr.value;
            }
            attrs["_rect"] = el.getBoundingClientRect();
            return attrs;
        }

        function _hasChanged(snapshotA, snapshotB) {
            for (var key in snapshotA) {
                if (snapshotA.hasOwnProperty(key)) {
                    if (key === "_rect") {
                        var a = snapshotA[key],
                            b = snapshotB[key];
                        if (!b || a.top !== b.top || a.left !== b.left || a.width !== b.width || a.height !== b.height) {
                            return true;
                        }
                    } else {
                        if (snapshotA[key] !== snapshotB[key]) {
                            return true;
                        }
                    }
                }
            }
            return false;
        }

        function _pollOnce() {
            var now = Date.now();

            // 按事件类型遍历，减少不必要检查
            for (var type in _typeRegistry) {
                if (!_typeRegistry.hasOwnProperty(type)) continue;

                var list = _typeRegistry[type];
                for (var i = 0; i < list.length; i++) {
                    var item = list[i];
                    var el = _getElementById(item.id);
                    if (!el) {
                        list.splice(i--, 1);
                        delete _elementCache[item.id];
                        continue;
                    }

                    var newSnapshot = _getAttrSnapshot(el);
                    if (_hasChanged(item.snapshot, newSnapshot)) {
                        item.snapshot = newSnapshot;
                        try {
                            item.callback.call(el, { type: type });
                        } catch (ex) {
                            console.error("Monitor callback error:", ex);
                        }
                    }
                }
            }

            // 清理过期节点
            if (now - _lastCleanup > _cleanupThreshold) {
                _cleanup();
                _lastCleanup = now;
            }
        }

        function _startLoop() {
            if (_polling) return;
            _polling = true;

            function loop() {
                _pollOnce();
                _loopHandle = global.requestAnimationFrame ? requestAnimationFrame(loop) : setTimeout(loop, _checkInterval);
            }
            loop();
        }

        function _stopLoop() {
            _polling = false;
            if (_loopHandle) {
                if (global.cancelAnimationFrame) cancelAnimationFrame(_loopHandle);
                else clearTimeout(_loopHandle);
                _loopHandle = null;
            }
        }

        function _cleanup() {
            for (var type in _typeRegistry) {
                if (!_typeRegistry.hasOwnProperty(type)) continue;
                var list = _typeRegistry[type];
                for (var i = 0; i < list.length; i++) {
                    if (!_getElementById(list[i].id)) {
                        list.splice(i--, 1);
                        delete _elementCache[list[i].id];
                    }
                }
            }
        }

        function observe(el, type, callback) {
            if (_eventTypes.indexOf(type) < 0) throw new Error("Unsupported event type: " + type);
            var id = _ensureId(el);
            if (!_typeRegistry[type]) _typeRegistry[type] = [];
            _typeRegistry[type].push({
                id: id,
                callback: callback,
                snapshot: _getAttrSnapshot(el)
            });
            _startLoop();
        }

        function detach(el, type, callback) {
            if (!_typeRegistry[type]) return;
            var id = el.getAttribute(_sIdAttr);
            if (!id) return;
            var list = _typeRegistry[type];
            for (var i = 0; i < list.length; i++) {
                if (list[i].id === id && (!callback || list[i].callback === callback)) {
                    list.splice(i--, 1);
                    delete _elementCache[id];
                }
            }
        }

        function clearAll() {
            _typeRegistry = {};
            _elementCache = {};
            _stopLoop();
        }

        return {
            observe: observe,
            detach: detach,
            clearAll: clearAll,
            EventType: {
                resize: "resize",
                position: "position",
                attribute: "attribute",
                child: "child"
            }
        };
    })();

    global.Windows.UI.Event.Monitor = Monitor;

})(window);

/*

// 1) 监听元素尺寸变化
var el = document.getElementById("box");
Windows.UI.Event.Monitor.observe(el, "resize", function (e) {
    console.log("resized", e.oldValue, e.newValue, e.rect);
});

// 2) 监听属性变化
Windows.UI.Event.Monitor.observe(el, "attributeChange", function (e) {
    console.log("attrs changed", e.detail); // detail.added / removed / changed
});

// 3) 监听附着/分离
Windows.UI.Event.Monitor.observe(el, "attach", function (e) {
    console.log("attached to doc");
});
Windows.UI.Event.Monitor.observe(el, "detach", function (e) {
    console.log("detached from doc");
});

// 4) 取消监听
Windows.UI.Event.Monitor.unobserve(el, "resize", handler);

*/

(function(global) {
    "use strict";

    var EventUtil = {};

    /**
     * 添加事件，兼容 IE10/IE11
     * @param {Element|Window|Document} el 目标元素
     * @param {string} sType 事件类型，如 "click", "resize", "scroll"
     * @param {function} pfHandler 回调函数
     * @param {boolean} [bUseCapture] 是否捕获阶段，默认 false
     */
    EventUtil.addEvent = function(el, sType, pfHandler, bUseCapture) {
        if (!el || typeof sType !== "string" || typeof pfHandler !== "function") return;

        bUseCapture = !!bUseCapture;

        if (el.addEventListener) {
            // 标准方式
            el.addEventListener(sType, pfHandler, bUseCapture);
        } else if (el.attachEvent) {
            // IE8-9 fallback
            el.attachEvent("on" + sType, pfHandler);
        } else {
            // 最原始方式
            var oldHandler = el["on" + sType];
            el["on" + sType] = function(e) {
                if (oldHandler) oldHandler(e || window.event);
                pfHandler(e || window.event);
            };
        }
    };

    /**
     * 移除事件，兼容 IE10/IE11
     * @param {Element|Window|Document} el 目标元素
     * @param {string} sType 事件类型，如 "click", "resize", "scroll"
     * @param {function} pfHandler 回调函数
     * @param {boolean} [bUseCapture] 是否捕获阶段，默认 false
     */
    EventUtil.removeEvent = function(el, sType, pfHandler, bUseCapture) {
        if (!el || typeof sType !== "string" || typeof pfHandler !== "function") return;

        bUseCapture = !!bUseCapture;

        if (el.removeEventListener) {
            el.removeEventListener(sType, pfHandler, bUseCapture);
        } else if (el.detachEvent) {
            el.detachEvent("on" + sType, pfHandler);
        } else {
            var oldHandler = el["on" + sType];
            if (oldHandler === pfHandler) {
                el["on" + sType] = null;
            }
        }
    };

    // 暴露到全局命名空间
    if (typeof module !== "undefined" && module.exports) {
        module.exports = {
            Windows: {
                UI: {
                    Event: {
                        Util: EventUtil
                    }
                }
            }
        };
    } else {
        global.Windows = global.Windows || {};
        global.Windows.UI = global.Windows.UI || {};
        global.Windows.UI.Event = global.Windows.UI.Event || {};
        global.Windows.UI.Event.Util = EventUtil;
    }

})(this);
/*
使用示例：
var handler = function (e) {
    console.log("事件触发", e.type);
};

// 添加事件
Windows.UI.Event.Util.addEvent(window, "resize", handler);

// 删除事件
Windows.UI.Event.Util.removeEvent(window, "resize", handler);

*/
(function(global) {
    "use strict";
    /**
     * 
     * @param {function} fn 
     * @param {number} delay 
     * @param {boolean} immediate 是否在第一次立即执行（可选，默认 false）
     * @returns {function} 返回一个新的函数，该函数在 delay 时间后执行 fn 函数，如果在 delay 时间内再次调用该函数，则会重新计时。
     */
    function debounce(fn, delay, immediate) {
        var timer = null;
        var lastCall = 0;
        return function() {
            var context = this;
            var args = arguments;
            var now = +new Date();
            var callNow = immediate && !timer;
            if (now - lastCall >= delay) {
                lastCall = now;
                if (callNow) {
                    fn.apply(context, args);
                }
            }
            clearTimeout(timer);
            timer = setTimeout(function() {
                lastCall = +new Date();
                if (!immediate) {
                    fn.apply(context, args);
                }
            }, delay);
        };
    }
    module.exports = { debounce: debounce };
})(this);
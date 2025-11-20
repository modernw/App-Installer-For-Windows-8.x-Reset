(function(global) {
    "use strict";

    function PromisePolyfill(pfExecutor) {
        var swState = "pending"; // "fulfilled" | "rejected"
        var vValue = undefined;
        var aHandlers = [];
        var pfOnCancel = null;

        function invokeHandlers() {
            if (swState === "pending") return;
            for (var i = 0; i < aHandlers.length; i++) {
                handle(aHandlers[i]);
            }
            aHandlers = [];
        }

        function handle(hHandler) {
            if (swState === "pending") {
                aHandlers.push(hHandler);
                return;
            }
            var pfCallback = swState === "fulfilled" ? hHandler.onFulfilled : hHandler.onRejected;
            if (!pfCallback) {
                if (swState === "fulfilled") {
                    hHandler.resolve(vValue);
                } else {
                    hHandler.reject(vValue);
                }
                return;
            }
            try {
                var vResult = pfCallback(vValue);
                hHandler.resolve(vResult);
            } catch (ex) {
                hHandler.reject(ex);
            }
        }

        function resolve(vResult) {
            try {
                if (vResult === self) throw new TypeError("A promise cannot be resolved with itself.");
                if (vResult && (typeof vResult === "object" || typeof vResult === "function")) {
                    var pfThen = vResult.then;
                    if (typeof pfThen === "function") {
                        pfThen.call(vResult, resolve, reject);
                        return;
                    }
                }
                swState = "fulfilled";
                vValue = vResult;
                invokeHandlers();
            } catch (ex) {
                reject(ex);
            }
        }

        function reject(vReason) {
            swState = "rejected";
            vValue = vReason;
            if (typeof PromisePolyfill.onerror === "function") {
                PromisePolyfill.onerror(vReason);
            }
            invokeHandlers();
        }
        var self = this;
        try {
            pfExecutor(resolve, reject, function(pfCancel) {
                pfOnCancel = pfCancel;
            });
        } catch (ex) {
            reject(ex);
        }
        this.then = function(pfOnFulfilled, pfOnRejected) {
            return new PromisePolyfill(function(resolve, reject) {
                handle({
                    onFulfilled: pfOnFulfilled,
                    onRejected: pfOnRejected,
                    resolve: resolve,
                    reject: reject
                });
            });
        };
        this["catch"] = function(pfOnRejected) {
            return this.then(null, pfOnRejected);
        };
        this.done = function(pfOnFulfilled, pfOnRejected) {
            this.then(pfOnFulfilled, pfOnRejected)["catch"](function(ex) {
                setTimeout(function() { throw ex; }, 0);
            });
        };
        this.cancel = function() {
            if (pfOnCancel) {
                try { pfOnCancel(); } catch (ex) {}
            }
            reject(new Error("Promise was canceled"));
        };
        this._oncancel = pfOnCancel;
        this._state = swState;
        this._value = vValue;
    }
    PromisePolyfill.is = function(vObj) {
        return vObj instanceof PromisePolyfill;
    };
    PromisePolyfill.resolve = function(vValue) {
        return new PromisePolyfill(function(resolve) { resolve(vValue); });
    };
    PromisePolyfill.reject = function(vReason) {
        return new PromisePolyfill(function(resolve, reject) { reject(vReason); });
    };
    PromisePolyfill.all = function(aPromises) {
        return new PromisePolyfill(function(resolve, reject) {
            var nRemaining = aPromises.length;
            var aResults = new Array(nRemaining);
            if (nRemaining === 0) resolve([]);

            function resolver(iIndex) {
                return function(vValue) {
                    aResults[iIndex] = vValue;
                    nRemaining--;
                    if (nRemaining === 0) resolve(aResults);
                };
            }
            for (var i = 0; i < aPromises.length; i++) {
                PromisePolyfill.resolve(aPromises[i]).then(resolver(i), reject);
            }
        });
    };
    PromisePolyfill.race = function(aPromises) {
        return new PromisePolyfill(function(resolve, reject) {
            for (var i = 0; i < aPromises.length; i++) {
                PromisePolyfill.resolve(aPromises[i]).then(resolve, reject);
            }
        });
    };
    PromisePolyfill.join = function(aPromises) {
        return PromisePolyfill.all(aPromises);
    };
    PromisePolyfill.any = function(aPromises) {
        return new PromisePolyfill(function(resolve, reject) {
            var nRemaining = aPromises.length;
            var aErrors = new Array(nRemaining);
            if (nRemaining === 0) reject(new Error("No promises provided."));

            function resolver(vValue) { resolve(vValue); }

            function rejecter(iIndex) {
                return function(ex) {
                    aErrors[iIndex] = ex;
                    nRemaining--;
                    if (nRemaining === 0) reject(aErrors);
                };
            }
            for (var i = 0; i < aPromises.length; i++) {
                PromisePolyfill.resolve(aPromises[i]).then(resolver, rejecter(i));
            }
        });
    };
    PromisePolyfill.timeout = function(pPromise, nMilliseconds) {
        return new PromisePolyfill(function(resolve, reject) {
            var hTimer = setTimeout(function() {
                reject(new Error("Promise timed out after " + nMilliseconds + "ms"));
            }, nMilliseconds);
            PromisePolyfill.resolve(pPromise).then(function(vValue) {
                clearTimeout(hTimer);
                resolve(vValue);
            }, function(ex) {
                clearTimeout(hTimer);
                reject(ex);
            });
        });
    };
    PromisePolyfill.as = function(vValue) {
        return PromisePolyfill.resolve(vValue);
    };
    PromisePolyfill.wrap = function(vValue) {
        return PromisePolyfill.resolve(vValue);
    };
    PromisePolyfill.wrapError = function(vError) {
        return PromisePolyfill.reject(vError);
    };
    PromisePolyfill.thenEach = function(aValues, pfCallback) {
        var aPromises = [];
        for (var i = 0; i < aValues.length; i++) {
            aPromises.push(PromisePolyfill.resolve(aValues[i]).then(pfCallback));
        }
        return PromisePolyfill.all(aPromises);
    };
    var hListeners = {};
    PromisePolyfill.addEventListener = function(sType, pfHandler) {
        if (!hListeners[sType]) hListeners[sType] = [];
        hListeners[sType].push(pfHandler);
    };
    PromisePolyfill.removeEventListener = function(sType, pfHandler) {
        if (!hListeners[sType]) return;
        var aList = hListeners[sType];
        for (var i = 0; i < aList.length; i++) {
            if (aList[i] === pfHandler) {
                aList.splice(i, 1);
                break;
            }
        }
    };
    PromisePolyfill.dispatchEvent = function(sType, vDetail) {
        if (!hListeners[sType]) return;
        var aList = hListeners[sType].slice();
        for (var i = 0; i < aList.length; i++) {
            try { aList[i](vDetail); } catch (ex) {}
        }
    };
    PromisePolyfill.supportedForProcessing = true;
    PromisePolyfill.onerror = null;
    if (typeof global.Promise !== "undefined") {
        var p = global.Promise;
        if (!p.join) p.join = p.all;
        if (!p.any) p.any = PromisePolyfill.any;
        if (!p.timeout) p.timeout = PromisePolyfill.timeout;
        if (!p.as) p.as = p.resolve;
        if (!p.wrap) p.wrap = p.resolve;
        if (!p.wrapError) p.wrapError = p.reject;
        if (!p.thenEach) p.thenEach = PromisePolyfill.thenEach;
        if (!p.is) p.is = function(vObj) { return vObj instanceof p; };
        if (!p.supportedForProcessing) p.supportedForProcessing = true;
        if (!p.addEventListener) p.addEventListener = PromisePolyfill.addEventListener;
        if (!p.removeEventListener) p.removeEventListener = PromisePolyfill.removeEventListener;
        if (!p.dispatchEvent) p.dispatchEvent = PromisePolyfill.dispatchEvent;
        if (!p.onerror) p.onerror = null;
    }
    if (typeof global.WinJS !== "undefined" && typeof global.WinJS.Promise !== "undefined") {
        var wp = global.WinJS.Promise;
        if (!wp.resolve) wp.resolve = function(vValue) { return new wp(function(c) { c(vValue); }); };
        if (!wp.reject) wp.reject = function(vReason) { return new wp(function(c, e) { e(vReason); }); };
        if (!wp.all) wp.all = function(aPromises) { return wp.join(aPromises); };
        if (!wp.race) wp.race = PromisePolyfill.race;
        global.Promise = wp;
        if (typeof global.Promise === "undefined") global.Promise = wp;
    }
    if (typeof global.Promise === "undefined" && typeof global.WinJS === "undefined") {
        global.Promise = PromisePolyfill;
    }
})(this);
/**
 * @module module
 * @description 模块化相关的函数。

 * 将模块化相关的函数封装在一个对象中，方便使用。
 * 1. directexports(cObject, cDirection)：直接将对象或函数公开到指定方向。
 * 2. exports：公开的对象或函数。
 * 3. imports：已加载的脚本。
 * 4. unload：卸载已加载的脚本。
 * 5. namespace(swNameSpace, cDirection)：创建命名空间。
 * 6. export(cObject, cDirection)：兼容旧版本的函数。
 * 7. import(aswSrc)：兼容旧版本的函数。
 * 兼容的旧版本函数需要引用预处理模块，否则不会兼容。
 */
(function(global) {
    "use strict";
    // 是否为通过构造函数创建的对象实例
    function isNewFunctionInstance(obj) {
        return typeof obj === "object" && Object.getPrototypeOf(obj) !== Object.prototype;
    }
    var module = new function() {
        if (this._instance) return this._instance;
        else this._instance = this;
        /**
         * 便于从代码块中公开。
         * @param {Object} cObject 需要公开的内容，以对象的形式分配好命名空间和别名。如：
         * {
         *     myFunc: function() { },
         *     myObj: function () {
         *         // object constructor
         *     }
         * }
         * 然后默认公开到全局。则公开后可以直接通过 myFunc() 或 myObj() 调用。
         * @param {global | Object} cDirection 公开方向
         * @returns {void}
         */
        this.directexports = function(cObject, cDirection) {
            if (!cDirection) cDirection = global;
            if (typeof cObject === "function") {
                cDirection[cObject.name] = cObject;
                return;
            } else if (typeof cObject === "object") {
                var keys = Object.keys(cObject);
                for (var i = 0; i < keys.length; i++) {
                    if (typeof cDirection[keys[i]] === "undefined") {
                        cDirection[keys[i]] = {};
                    }
                    if (isNewFunctionInstance(cObject[keys[i]])) {
                        cDirection[keys[i]] = cObject[keys[i]];
                    } else if (typeof cObject[keys[i]] === "object") {
                        this.directexports(cObject[keys[i]], cDirection[keys[i]]);
                    } else {
                        cDirection[keys[i]] = cObject[keys[i]];
                    }
                }
            } else {
                cDirection = cObject;
            }
        };
        /**
         * @property {global | Object} exports 欲公开的内容以对象的形式分配好命名空间和别名。如：
         */
        Object.defineProperty(this, "exports", {
            get: function() {
                return global;
            },
            set: function(cObject, cDirection) {
                this.directexports(cObject, cDirection);
            }
        });
        if (typeof Array.isArray === "undefined") {
            /**
             * 判断是否为数组。这是函数补充。
             * @param {*} arg 需要判断的对象
             * @returns {boolean}
             */
            Array.isArray = function(arg) {
                return Object.prototype.toString.call(arg) === "[object Array]" || arg instanceof Array || arg instanceof HTMLCollection || arg instanceof NodeList;
            };
        }
        /**
         * @property {HTMLCollection | NodeList | null} imports 当前页面已加载的脚本。会以 NodeList 形式返回。如果为设置，则是追加脚本，而不是覆盖。传入脚本路径或其数组。
         */
        Object.defineProperty(this, "imports", {
            get: function() {
                if (typeof window !== "undefined" && typeof document !== "undefined") {
                    var scripts = document.querySelectorAll("script[src]");
                    return scripts;
                } else return null;
            },
            set: function(aswSrc) {
                if (typeof aswSrc === "string") {
                    var scripts = this.imports;
                    if (scripts && scripts.length > 0) {
                        for (var i = 0; i < scripts.length; i++) {
                            if (scripts[i].src === aswSrc) {
                                return;
                            }
                        }
                        var script = document.createElement("script");
                        script.src = aswSrc;
                        document.head.appendChild(script);
                    }
                } else if (Array.isArray(aswSrc) || aswSrc instanceof Array || aswSrc instanceof HTMLCollection || aswSrc instanceof NodeList) {
                    for (var i = 0; i < aswSrc.length; i++) {
                        this.imports = aswSrc[i];
                    }
                }
            }
        });
        /**
         * @property {HTMLCollection | NodeList | null} unload 卸载当前页面已加载的脚本。传入脚本路径或其数组。无法只读
         */
        Object.defineProperty(this, "unload", {
            set: function(aswSrc) {
                if (typeof aswSrc === "string") {
                    var scripts = this.imports;
                    if (scripts && scripts.length > 0) {
                        for (var i = 0; i < scripts.length; i++) {
                            if (scripts[i].src === aswSrc) {
                                scripts[i].remove();
                                return;
                            }
                        }
                    }
                } else if (Array.isArray(aswSrc) || aswSrc instanceof Array || aswSrc instanceof HTMLCollection || aswSrc instanceof NodeList) {
                    for (var i = 0; i < aswSrc.length; i++) {
                        this.unload = aswSrc[i];
                    }
                }
            }
        });
        Object.defineProperty(this, "global", {
            get: function() {
                return global;
            }
        });
        /**
         * 命名空间，其实为一种字典。
         * @param {string} swNameSpace 命名空间路径。如："WinJS.Namespace"。不能为空。命名空间之间用"."分隔。
         * @param {*} cDirection 创建的路径，默认创建到全局
         * @returns 
         */
        this.namespace = function(swNameSpace, cDirection) {
            if (!cDirection) cDirection = global;
            var strarr = (swNameSpace || "").split(".");
            if (!strarr.length) strarr.push(swns);

            function setNs(strarr, direct) {
                var newdirect = null;
                if (!strarr.length) return direct;
                else {
                    if (typeof direct[strarr[0]] === "undefined") {
                        direct[strarr[0]] = {};
                    }
                    newdirect = direct[strarr[0]];
                }
                strarr.shift();
                return setNs((strarr || []), newdirect);
            }
            return setNs(strarr, cDirection);
        };
        if (typeof preprocess !== "undefined" && preprocess.ifdef("COMPATIBLE")) {
            /**
             * 兼容旧版本的函数。
             * @param {Object | Function} cObject 欲公开的对象或函数 
             * @param {global | Object} cDirection 公开方向
             */
            this.export = function(cObject, cDirection) {
                this.directexports(cObject, cDirection);
            };
            /**
             * 
             * @param {Array <string> | string} aswSrc 脚本路径或其数组 
             * @returns {boolean} 是否成功添加脚本。在兼容模式下只会返回真。
             */
            this.import = function(aswSrc) {
                this.imports = aswSrc;
                return true;
            };
        }
    };
    module.exports = {
        module: module,
        namespace: module.namespace,
    };
})(this);
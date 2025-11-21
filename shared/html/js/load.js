(function() {
    "use strict";
    var loaddelegate = [];
    loaddelegate.push(window.onload);
    var elderload = loaddelegate[0];

    function onLoad(e) {
        for (var i = 0; i < loaddelegate.length; i++) {
            if (typeof loaddelegate[i] === "function") {
                loaddelegate[i](e);
            }
        }
    }
    window.onload = onLoad;
    module.exports = {
        OnLoad: {
            add: function(func) {
                if (typeof func === "function") {
                    for (var i = 0; i < loaddelegate.length; i++) {
                        if (!i) continue; // 第一个元素必须保留，因为其为原生的 onload 事件
                        if (loaddelegate[i] === func) {
                            return;
                        }
                    }
                    loaddelegate.push(func);
                }
            },
            remove: function(func) {
                if (!func) return;
                for (var i = 0; i < loaddelegate.length; i++) {
                    if (!i) continue;
                    if (loaddelegate[i] === func) {
                        loaddelegate.splice(i, 1);
                        return;
                    }
                }
            }
        }
    };
})();
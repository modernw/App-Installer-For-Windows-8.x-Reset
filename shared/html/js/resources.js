(function() {
    "use strict";

    function getAllNodesHasResource() {
        var allElements = document.body.getElementsByTagName('*');
        var result = [];
        for (var i = 0; i < allElements.length; i++) {
            var el = allElements[i];
            var tag = el.tagName.toLowerCase();
            if (tag === 'script' || tag === 'style' || tag === 'link') {
                continue;
            }
            var byName = el.getAttribute('data-res-byname');
            var byId = el.getAttribute('data-res-byid');
            if ((byName && !Bridge.NString.empty(byName)) || (byId && parseInt(byId, 10) > 0)) {
                result.push(el);
            }
        }

        return result; // 返回符合条件的元素数组
    }
    module.exports = {
        Resources: {
            processAll: function() {
                var nodes = getAllNodesHasResource();
                for (var i = 0; i < nodes.length; i++) {
                    if (nodes[i].hasAttribute('data-res-byname')) {
                        var resName = nodes[i].getAttribute('data-res-byname');
                        nodes[i].textContent = Bridge.Resources.byname(resName);
                    } else if (nodes[i].hasAttribute('data-res-byid')) {
                        var resId = parseInt(nodes[i].getAttribute('data-res-byid'), 10);
                        nodes[i].textContent = Bridge.Resources.byid(resId);
                    } else {
                        nodes[i].textContent = "";
                    }
                }
            }
        }
    };
})();
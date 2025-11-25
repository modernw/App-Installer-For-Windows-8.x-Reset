(function() {
    "use strict";

    function calcColorComponent(x) {
        var y;
        if (x <= 127.5) {
            y = 0.999 * x - 5.4361 + x;
        } else {
            y = -0.999 * x + 249.32 + x;
        }
        return (y < 0 ? 0 : y) % 256; // 确保y不小于0
    }

    function roundToNearestInt(num) {
        return Math.round(num);
    }

    function genTileBackFilter(backcolor) {
        var basecolor = Color.parse(backcolor);
        var rightcolor = new Color(
            roundToNearestInt(calcColorComponent(basecolor.red)),
            roundToNearestInt(calcColorComponent(basecolor.green)),
            roundToNearestInt(calcColorComponent(basecolor.blue)),
            0.25
        );
        return "linear-gradient(to right, rgba(0,0,0,0), " + rightcolor.RGBA.stringify() + ")";
    }
    module.exports = {
        Color: {
            genTileBackFilter: genTileBackFilter
        }
    };
})();
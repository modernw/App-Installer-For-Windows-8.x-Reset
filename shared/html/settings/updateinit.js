(function(global) {
    global.autoCheckUpdate = false;
    global.autoInstallUpdate = false;
    if (Bridge.External.exec1 == true) return;
    var args = cmdargs;
    var optionarg = "";
    if (args.length > 2) {
        optionarg = args[2];
    } else {
        return;
    }
    if (
        Bridge.NString.equals(optionarg, "/checkupdate") ||
        Bridge.NString.equals(optionarg, "-checkupdate") ||
        Bridge.NString.equals(optionarg, "checkupdate")
    ) global.autoCheckUpdate = true;
    if (
        Bridge.NString.equals(optionarg, "/autoupdate") ||
        Bridge.NString.equals(optionarg, "-autoupdate") ||
        Bridge.NString.equals(optionarg, "autoupdate")
    ) {
        global.autoCheckUpdate = true;
        global.autoInstallUpdate = true;
    }
    Bridge.External.exec1 = true;
})(this);
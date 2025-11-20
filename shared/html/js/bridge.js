(function(global) {
    "use strict";
    var ext = global.external;

    function blankFunc(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10) {
        console.log("blankFunc called with arguments: " + arg1 + ", " + arg2 + ", " + arg3 + ", " + arg4 + ", " + arg5 + ", " + arg6 + ", " + arg7 + ", " + arg8 + ", " + arg9 + ", " + arg10);
    }

    global.Bridge = {
        External: ext,
        Frame: {
            isIe10: function() { return ext.IEFrame.Version === 10; },
            isIe11: function() { return ext.IEFrame.Version === 11; }
        },
        UI: {},
        String: {
            trim: function(str) { return ext.String.Trim(str); },
            tolower: function(str) { return ext.String.ToLower(str); },
            toupper: function(str) { return ext.String.ToUpper(str); },
        },
        NString: {
            equals: function(str1, str2) { return ext.String.NString.NEquals(str1, str2); },
            compare: function(str1, str2) { return ext.String.NString.Compare(str1, str2); },
            empty: function(str) { return ext.String.NString.Empty(str); },
            length: function(str) { return ext.String.NString.Length(str); },
        }
    }
    Object.defineProperty(global.Bridge.Frame, "scale", {
        get: function() { return ext.IEFrame.Scale; },
        set: function(value) { ext.IEFrame.Scale = value; return ext.IEFrame.Scale; }
    });
    Object.defineProperty(global.Bridge.Frame, "version", {
        get: function() { return ext.IEFrame.Version; },
    });
    Object.defineProperty(global.Bridge.UI, "dpiPercent", {
        get: function() { return ext.System.UI.DPIPercent; }
    });
    Object.defineProperty(global.Bridge.UI, "dpi", {
        get: function() { return ext.System.UI.DPI; }
    });
})(this);
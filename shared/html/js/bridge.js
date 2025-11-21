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
        UI: {
            Splash: {
                show: function() { ext.System.UI.ShowSplash(); },
                fadeAway: function() { ext.System.UI.FadeAwaySplash(); },
                fadeOut: function() { ext.System.UI.FadeOutSplash(); }
            }
        },
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
        },
        Resources: {
            byname: function(resname) { return ext.System.Resources.GetByName(resname); },
            byid: function(resid) { return ext.System.Resources.GetById(resid); },
            nameToId: function(resname) { return ext.System.Resources.ToId(resname); },
            idToName: function(resid) { return ext.System.Resources.ToName(resid); },
        }
    };
    Object.defineProperty(global.Bridge.Frame, "scale", {
        get: function() { return ext.IEFrame.Scale; },
        set: function(value) { ext.IEFrame.Scale = value; return ext.IEFrame.Scale; }
    });
    Object.defineProperty(global.Bridge.Frame, "version", {
        get: function() { return ext.IEFrame.Version; },
    });
    Object.defineProperty(global.Bridge.Frame, "WindowSize", {
        get: function() { return ext.System.UI.WndSize; },
    });
    Object.defineProperty(global.Bridge.Frame, "ClientSize", {
        get: function() { return ext.System.UI.ClientSize; },
    });
    Object.defineProperty(global.Bridge.UI, "dpiPercent", {
        get: function() { return ext.System.UI.DPIPercent; }
    });
    Object.defineProperty(global.Bridge.UI, "dpi", {
        get: function() { return ext.System.UI.DPI; }
    });
    Object.defineProperty(global.Bridge.UI.Splash, "backcolor", {
        get: function() { return ext.System.UI.SplashBackgroundColor; },
    });
    Object.defineProperty(global.Bridge.UI.Splash, "imageurl", {
        get: function() { return ext.System.UI.SplashImage; },
    });


})(this);
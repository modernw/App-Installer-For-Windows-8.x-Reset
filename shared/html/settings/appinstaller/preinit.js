(function(global) {
    var storage = Bridge.External.Storage;
    var path = storage.path;
    var root = path.getDir(path.program);
    var exepath = path.combine(root, "appinstaller.exe");
    var id = "App";
    var ve = Bridge.External.VisualElements.get(id);
    var slideback = ve["BackgroundColor"];
    global.slideback = slideback;
    global.exepath = exepath;
    global.visual = ve;
})(this);
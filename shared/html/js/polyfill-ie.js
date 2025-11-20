(function(global) {
    if (typeof msWriteProfilerMark === "undefined") {
        function msWriteProfilerMark(swMark) {
            if (typeof performance !== "undefined" && typeof performance.mark === "function") {
                return performance.mark(swMark);
            } else if (typeof console !== "undefined" && typeof console.log === "function") {
                return console.log(swMark);
            }
        }
        module.exports = {
            msWriteProfilerMark: msWriteProfilerMark
        };
    }
})(this);
(function(global) {

    if (typeof global.Debug === "undefined") {
        var fakeDebug = {};

        // 基本属性
        fakeDebug.debuggerEnabled = true;
        fakeDebug.setNonUserCodeExceptions = false;

        // 常量
        fakeDebug.MS_ASYNC_CALLBACK_STATUS_ASSIGN_DELEGATE = 0;
        fakeDebug.MS_ASYNC_CALLBACK_STATUS_JOIN = 1;
        fakeDebug.MS_ASYNC_CALLBACK_STATUS_CHOOSEANY = 2;
        fakeDebug.MS_ASYNC_CALLBACK_STATUS_CANCEL = 3;
        fakeDebug.MS_ASYNC_CALLBACK_STATUS_ERROR = 4;

        fakeDebug.MS_ASYNC_OP_STATUS_SUCCESS = 1;
        fakeDebug.MS_ASYNC_OP_STATUS_CANCELED = 2;
        fakeDebug.MS_ASYNC_OP_STATUS_ERROR = 3;

        // 方法：输出
        fakeDebug.write = function(msg) {
            if (console && console.log) console.log("[Debug.write] " + msg);
        };
        fakeDebug.writeln = function(msg) {
            if (console && console.log) console.log("[Debug.writeln] " + msg);
        };

        // 方法：断言 / 中断
        fakeDebug.assert = function(cond, msg) {
            if (!cond) {
                if (console && console.error) {
                    console.error("[Debug.assert] Assertion failed: " + (msg || ""));
                }
                // 可选触发断点
                // debugger;
            }
        };
        fakeDebug.break = function() {
            debugger;
        };

        // 方法：异步跟踪（空实现）
        fakeDebug.msTraceAsyncCallbackCompleted = function() {};
        fakeDebug.msTraceAsyncCallbackStarting = function() {};
        fakeDebug.msTraceAsyncOperationCompleted = function() {};
        fakeDebug.msTraceAsyncOperationStarting = function() {};
        fakeDebug.msUpdateAsyncCallbackRelation = function() {};

        global.Debug = fakeDebug;
    }

})(this);
(function(global) {

    if (typeof global.setImmediate === "undefined") {
        var nextHandle = 1; // 唯一任务 id
        var tasksByHandle = {};
        var currentlyRunning = false;

        function addTask(fn, args) {
            tasksByHandle[nextHandle] = function() {
                fn.apply(undefined, args);
            };
            return nextHandle++;
        }

        function run(handle) {
            if (currentlyRunning) {
                // 如果已经在运行，延迟一下
                setTimeout(run, 0, handle);
            } else {
                var task = tasksByHandle[handle];
                if (task) {
                    currentlyRunning = true;
                    try {
                        task();
                    } finally {
                        delete tasksByHandle[handle];
                        currentlyRunning = false;
                    }
                }
            }
        }

        function installSetImmediate() {
            if (typeof MessageChannel !== "undefined") {
                var channel = new MessageChannel();
                channel.port1.onmessage = function(event) {
                    run(event.data);
                };
                return function() {
                    var handle = addTask(arguments[0], Array.prototype.slice.call(arguments, 1));
                    channel.port2.postMessage(handle);
                    return handle;
                };
            } else {
                // fallback: setTimeout
                return function() {
                    var handle = addTask(arguments[0], Array.prototype.slice.call(arguments, 1));
                    setTimeout(run, 0, handle);
                    return handle;
                };
            }
        }

        global.setImmediate = installSetImmediate();

        // 对应 clearImmediate
        if (typeof global.clearImmediate === "undefined") {
            global.clearImmediate = function(handle) {
                delete tasksByHandle[handle];
            };
        }
    }
})(this);
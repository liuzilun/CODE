package logger

type LogInterface interface {
	SetLevel(level int)
	LogDebug(format string, args ...interface{})
	LogTrace(format string, args ...interface{})
	LogInfo(format string, args ...interface{})
	LogWarm(format string, args ...interface{})
	LogError(format string, args ...interface{})
	LogFatal(format string, args ...interface{})
	Close()
}

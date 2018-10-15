package logger

import (
	"fmt"
	"os"
	"path"
	"runtime"
	"time"
)

type FileLogger struct {
	level    int
	logPath  string
	logName  string
	file     *os.File
	warnFile *os.File
}

func NewFileLogger(level int, logPath, logName string) LogInterface {
	logger := &FileLogger{
		level:   level,
		logPath: logPath,
		logName: logName,
	}
	logger.init()
	return logger
}
func (f *FileLogger) SetLevel(level int) {
	if level < LogLevelDebug || level > LogLevelFatal {
		level = LogLevelDebug
	}
	f.level = level
}

func (f *FileLogger) init() {
	filename := fmt.Sprintf("%s/%s.log", f.logPath, f.logName)
	file, err := os.OpenFile(filename, os.O_CREATE|os.O_APPEND|os.O_WRONLY, 0755)
	if err != nil {
		panic(fmt.Sprintf("open faile %s failed,err:%v", filename, err))
	}
	f.file = file

	filename = fmt.Sprintf("%s/%s.wf", f.logPath, f.logName)
	file, err = os.OpenFile(filename, os.O_CREATE|os.O_APPEND|os.O_WRONLY, 0755)
	if err != nil {
		panic(fmt.Sprintf("open wf file %s failed,err:%v", filename, err))
	}
	f.warnFile = file

}
func (f *FileLogger) LogDebug(format string, args ...interface{}) {
	if f.level > LogLevelDebug {
		return
	}
	now := time.Now()
	nowStr := now.Format("2006-1-2 15:04:05.999")

	fileName, funcName, line := GetLineInfo()
	fileName = path.Base(fileName)
	funcName = path.Base(funcName)

	msg := fmt.Sprintf(format, args...)

	fmt.Fprintf(f.file, "[%s]|%s|%s:%d|%s|%s \n", nowStr, "DEBUG", fileName, line, funcName, msg)

}
func (f *FileLogger) LogTrace(format string, args ...interface{}) {
	if f.level > LogLevelTrace {
		return
	}
	now := time.Now()
	nowStr := now.Format("2006-1-2 15:04:05.999")

	fileName, funcName, line := GetLineInfo()
	fileName = path.Base(fileName)
	funcName = path.Base(funcName)

	msg := fmt.Sprintf(format, args...)

	fmt.Fprintf(f.file, "[%s]|%s|%s:%d|%s|%s \n", nowStr, "Trace", fileName, line, funcName, msg)

}
func (f *FileLogger) LogInfo(format string, args ...interface{}) {
	if f.level > LogLevelInfo {
		return
	}
	fmt.Fprintf(f.file, format, args...)
}
func (f *FileLogger) LogWarm(format string, args ...interface{}) {
	if f.level > LogLevelWarn {
		return
	}
	fmt.Fprintf(f.warnFile, format, args...)
}
func (f *FileLogger) LogError(format string, args ...interface{}) {
	if f.level > LogLevelWarn {
		return
	}
	fmt.Fprintf(f.warnFile, format, args...)
}

func (f *FileLogger) LogFatal(format string, args ...interface{}) {
	if f.level > LogLevelWarn {
		return
	}
	fmt.Fprintf(f.warnFile, format, args...)
}

func (f FileLogger) Close() {
	f.file.Close()
	f.warnFile.Close()
}

func GetLineInfo() (fineName string, funcName string, lineNo int) {
	pc, file, line, ok := runtime.Caller(3)
	if ok {
		fineName = file
		funcName = runtime.FuncForPC(pc).Name()
		lineNo = line

	}
	return
}

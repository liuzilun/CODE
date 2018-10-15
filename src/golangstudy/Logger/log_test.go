package logger

import (
	"testing"
)

func TestFileLogger(t *testing.T) {
	logger := NewFileLogger(LogLevelDebug, "d:/logs/", "test")

	logger.LogDebug("user id %d is test come", 2222)
	//logger.LogInfo("test LogInfo")
	//logger.LogWarm("test warn")
	//logger.LogError("test error")

	logger.Close()
}

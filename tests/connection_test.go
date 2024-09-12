package tests

import (
	"testing"
	"workspace/tests/utils_config"
)

var DataRoot = "./data"

func TestConnection(t *testing.T) {
	t.Log("Connection test")

	ConfigFilePath := DataRoot + "/configs/default.conf"
	config := utils_config.NewConfig(ConfigFilePath)
	err := config.Parse()
	if err != nil {
		t.Fatalf("Error parsing config file: %v", err)
	}
	config.PrintAllBlocks()
}

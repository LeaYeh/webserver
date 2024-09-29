package main

import (
	"fmt"
	"path/filepath"
	"workspace/tests/utils_config"
)

func DemoConfigParser() {
	configPath := filepath.Join("tests", "data", "configs", "webserv_all_features.conf")
	config := utils_config.NewConfig(configPath)
	err := config.Parse()
	if err != nil {
		fmt.Printf("error parsing config file: %v\n", err)
		return
	}
	config.PrintAllBlocks()
}

func main() {
	DemoConfigParser()
}


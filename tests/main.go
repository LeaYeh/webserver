package main

import (
	"fmt"
	"path/filepath"
	"workspace/tests/utils_config"
)

func main() {
	fmt.Println("Main test")
	configPath := filepath.Join("tests", "data", "configs", "webserv_all_features.conf")
	config := utils_config.NewConfig(configPath)
	err := config.Parse()
	if err != nil {
		fmt.Println("Error parsing config file: %v", err)
		return
	}
	config.PrintAllBlocks()
}


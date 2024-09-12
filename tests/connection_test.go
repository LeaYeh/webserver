package tests

import (
	"fmt"
	"os"
	"runtime"
	"net/http"
	"os/exec"
	"path/filepath"
	"testing"
	"workspace/tests/utils_config"
)

func TestMain(m *testing.M) {
	fmt.Println("Re-compiling c++ web server")
	err := CompileCppServer()
	if err != nil {
		fmt.Printf("Global setup: %v\n", err)
		os.Exit(1)
	}
	os.Exit(m.Run())
}

func TestConnection(t *testing.T) {
	t.Log("Connection test")
	ConfigName := "default.conf"

	// Load config
	config := LoadConfig(t, ConfigName)
	config.PrintAllBlocks()

	// Start c++ web server
	fmt.Println("Starting c++ web server...")
	cmd := StartCppServer(t, ConfigName)
	defer StopCppServer(t, cmd)

	// Test connection
	t.Log("Testing connection...")
	t.Run("GET /", func(t *testing.T) {
		for _, serverBlock := range config.ServerBlockList {
			serverBlock := serverBlock.(*utils_config.ConfigServerBlock)
			listen := serverBlock.GetDirectives()["listen"]
			locationBlock := serverBlock.GetLocationBlock("/").(*utils_config.ConfigLocationBlock)
			url := "http://" + listen.(string) + locationBlock.GetDirectives()["route"].(string)
			resp, err := http.Get(url)
			if err != nil {
				t.Fatalf("Error making GET request: %v", err)
			}
			defer resp.Body.Close()
			t.Logf("Response status: %s", resp.Status)
		}
	})
}

func findProjectRoot() (string, error) {
	_, currentFile, _, ok := runtime.Caller(0)
	if !ok {
		return "", fmt.Errorf("failed to get current file path")
	}
	dir := filepath.Dir(currentFile)
	for {
		if _, err := os.Stat(filepath.Join(dir, "go.mod")); err == nil {
			return dir, nil
		}
		parentDir := filepath.Dir(dir)
		if parentDir == dir {
			break
		}
		dir = parentDir
	}
	return "", fmt.Errorf("can't find project root by go.mod")
}

func LoadConfig(t *testing.T, configName string) *utils_config.Config {
	t.Helper()
	DataRoot := "./data"
	ConfigFilePath := filepath.Join(DataRoot, "configs", configName)
	config := utils_config.NewConfig(ConfigFilePath)
	err := config.Parse()
	if err != nil {
		t.Fatalf("Error parsing config file: %v", err)
	}
	return config
}

func CompileCppServer() error {
	projectRoot, err := findProjectRoot()
	if err != nil {
		return fmt.Errorf("error finding project root: %v", err)
	}
	cmd := exec.Command("make", "re")
	cmd.Dir = projectRoot
	output, err := cmd.CombinedOutput()
	if err != nil {
		return fmt.Errorf("error compiling c++ web server: %v\n", err)
	}
	fmt.Printf("C++ web server compiled\nOutput: %s\n", output)
	return nil
}

func StartCppServer(t *testing.T, configName string) *exec.Cmd {
	t.Helper()
	projectRoot, err := findProjectRoot()
	if err != nil {
		t.Fatalf("Error finding project root: %v", err)
	}
	cmd := exec.Command("./webserv", filepath.Join("tests", "data", "configs", configName))
	cmd.Dir = projectRoot
	err = cmd.Start()
	if err != nil {
		t.Fatalf("Error starting c++ web server: %v", err)
	}
	return cmd
}

func StopCppServer(t *testing.T, cmd *exec.Cmd) error {
	t.Helper()
	err := cmd.Process.Kill()
	if err != nil {
		return fmt.Errorf("error stopping c++ web server: %v", err)
	}
	return nil
}

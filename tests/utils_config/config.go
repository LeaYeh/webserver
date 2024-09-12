package utils_config

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

type ConfigBlock interface {
	Parse(line string) error
	GetDirectives() map[string]interface{}
	String() string
}

type Config struct {
	Filename        string
	GlobalBlock     ConfigBlock
	HttpBlock       ConfigBlock
	ServerBlockList []ConfigBlock
	CurrentBlock    ConfigBlockLevel
	parseFunctions  map[ConfigBlockLevel]func(line string) error
}

type ConfigBlockLevel int

const (
	ConfigBlockLevelGlobal ConfigBlockLevel = iota
	ConfigBlockLevelHttp
	ConfigBlockLevelServer
	ConfigBlockLevelLocation
)

func (c *Config) PrintAllBlocks() {
	fmt.Println("--- Config[", c.Filename, "] ---")
	fmt.Println("  " + c.GlobalBlock.String())
	fmt.Println("  " + c.HttpBlock.String())
	for _, serverBlock := range c.ServerBlockList {
		fmt.Println("  " + serverBlock.String())
		for _, locationBlock := range serverBlock.(*ConfigServerBlock).LocationBlocks {
			fmt.Println("    " + locationBlock.String())
		}
	}
}

func NewConfig(filename string) *Config {
	c := &Config{
		Filename: filename,
		GlobalBlock: NewConfigGlobalBlock().(*ConfigGlobalBlock),
		HttpBlock: NewConfigHttpBlock().(*ConfigHttpBlock),
		ServerBlockList: []ConfigBlock{},
		CurrentBlock: ConfigBlockLevelGlobal,
	}
	c.parseFunctions = map[ConfigBlockLevel]func(line string) error{
		ConfigBlockLevelGlobal: func(line string) error {
			return c.GlobalBlock.Parse(line)
		},
		ConfigBlockLevelHttp: func(line string) error {
			return c.HttpBlock.Parse(line)
		},
		ConfigBlockLevelServer: func(line string) error {
			return c.ServerBlockList[len(c.ServerBlockList)-1].Parse(line)
		},
		ConfigBlockLevelLocation: func(line string) error {
			if len(c.ServerBlockList) == 0 {
				return fmt.Errorf("no server block found")
			}
			lastServerBlock := c.ServerBlockList[len(c.ServerBlockList)-1].(*ConfigServerBlock)
			lastLocationBlock := lastServerBlock.LocationBlocks[len(lastServerBlock.LocationBlocks)-1]
			return lastLocationBlock.Parse(line)
		},
	}
	return c
}

func (c *Config) SetBlockLevel(level ConfigBlockLevel) {
	c.CurrentBlock = level

	switch level {
	case ConfigBlockLevelGlobal:
		// Do nothing on unique block
		return
	case ConfigBlockLevelHttp:
		// Do nothing on unique block
		return
	case ConfigBlockLevelServer:
		c.ServerBlockList = append(c.ServerBlockList, NewConfigServerBlock().(*ConfigServerBlock))
	case ConfigBlockLevelLocation:
		lastServerBlock := c.ServerBlockList[len(c.ServerBlockList)-1].(*ConfigServerBlock)
		lastServerBlock.LocationBlocks = append(lastServerBlock.LocationBlocks, NewConfigLocationBlock().(*ConfigLocationBlock))
	}
}

func (c *Config) Parse() error {
	file, err := os.Open(c.Filename)
	if err != nil {
		return err
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := scanner.Text()
		line = strings.TrimSpace(line)
		if len(line) == 0 || strings.HasPrefix(line, "#") {
			// Skip empty lines and comments
			continue
		}
		if strings.HasPrefix(line, "http {") {
			c.SetBlockLevel(ConfigBlockLevelHttp)
		} else if strings.HasPrefix(line, "server {") {
			c.SetBlockLevel(ConfigBlockLevelServer)
		} else if strings.HasPrefix(line, "location {") {
			c.SetBlockLevel(ConfigBlockLevelLocation)
		} else {
			c.parseFunctions[c.CurrentBlock](line)
		}
	}

	if err := scanner.Err(); err != nil {
		return err
	}

	return nil
}

func ParseDirective(line string, directives map[string]interface{}) error {
	parts := strings.SplitN(line, " ", 2)
	if len(parts) != 2 {
		return fmt.Errorf("invalid config format: %s", line)
	}

	key := strings.TrimSpace(parts[0])
	value := strings.TrimSpace(parts[1])

	if _, ok := directives[key]; !ok {
		return fmt.Errorf("invalid directive: %s", key)
	}

	directives[key] = value
	return nil
}

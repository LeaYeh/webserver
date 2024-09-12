package utils_config

import "fmt"

type ConfigGlobalBlock struct {
	Directives map[string]interface{}
}

func (c *ConfigGlobalBlock) String() string {
	return fmt.Sprintf("GlobalBlock: %v", c.Directives)
}

func NewConfigGlobalBlock() ConfigBlock {
	return &ConfigGlobalBlock{
		Directives: map[string]interface{}{
			"worker_processes":   "1",
			"worker_connections": "1024",
		},
	}
}

func (c *ConfigGlobalBlock) GetDirectives() map[string]interface{} {
	return c.Directives
}

func (c *ConfigGlobalBlock) Parse(line string) error {
	return ParseDirective(line, c.Directives)
}

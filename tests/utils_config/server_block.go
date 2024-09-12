package utils_config

import "fmt"

type ConfigServerBlock struct {
	Directives     map[string]interface{}
	LocationBlocks []ConfigBlock
}

func (c *ConfigServerBlock) String() string {
	return fmt.Sprintf("ServerBlock: %v", c.Directives)
}

func NewConfigServerBlock() ConfigBlock {
	return &ConfigServerBlock{
		Directives: map[string]interface{}{
			"listen":             "80",
			"server_name":        "localhost",
			"error_log":          "error.log",
			"keep_alive_timeout": "65",
		},
		LocationBlocks: []ConfigBlock{},
	}
}

func (c *ConfigServerBlock) GetDirectives() map[string]interface{} {
	return c.Directives
}

func (c *ConfigServerBlock) Parse(line string) error {
	return ParseDirective(line, c.Directives)
}

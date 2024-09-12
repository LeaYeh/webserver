package utils_config

import "fmt"

type ConfigHttpBlock struct {
	Directives map[string]interface{}
}

func (c *ConfigHttpBlock) String() string {
	return fmt.Sprintf("HttpBlock: %v", c.Directives)
}

func NewConfigHttpBlock() ConfigBlock {
	return &ConfigHttpBlock{
		Directives: map[string]interface{}{
			"client_max_body_size": "10M",
			"default_type":         "text/plain",
			"error_page": map[string]string{
				"404": "/404.html",
				"500": "/500.html",
				"502": "/502.html",
				"503": "/503.html",
				"504": "/504.html",
			},
		},
	}
}

func (c *ConfigHttpBlock) GetDirectives() map[string]interface{} {
	return c.Directives
}

func (c *ConfigHttpBlock) Parse(line string) error {
	return ParseDirective(line, c.Directives)
}

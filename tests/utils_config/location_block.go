package utils_config

import "fmt"

type ConfigLocationBlock struct {
	Directives map[string]interface{}
}

func (c *ConfigLocationBlock) String() string {
	return fmt.Sprintf("LocationBlock: %v", c.Directives)
}

func NewConfigLocationBlock() ConfigBlock {
	return &ConfigLocationBlock{
		Directives: map[string]interface{}{
			"limit_except": map[string]interface{}{
				"GET":    true,
				"POST":   true,
				"PUT":    true,
				"DELETE": true,
			},
			"route":         "/",
			"root":          "/var/www/html",
			"index":         "index.html",
			"redirect":      "",
			"autoindex":     false,
			"cgi_extension": "",
			"cgi_path":      "",
			"enable_upload": false,
			"upload_path":   "",
		},
	}
}

func (c *ConfigLocationBlock) GetDirectives() map[string]interface{} {
	return c.Directives
}

func (c *ConfigLocationBlock) Parse(line string) error {
	return ParseDirective(line, c.Directives)
}

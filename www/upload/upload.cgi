#!/bin/bash
echo "Content-Type: application/json"
echo ""

# 处理文件上传
read -r line
while [[ $line != $'\r' ]]; do
    echo "$line"
    read -r line
done

# 提取上传的文件信息
boundary="--$(echo "$line" | sed 's/^.*--//')"
while IFS= read -r line; do
    if [[ $line == *"$boundary"* ]]; then
        break
    fi
    if [[ $line == *"Content-Disposition:"* ]]; then
        filename=$(echo "$line" | sed -n 's/.*filename="\([^"]*\)".*/\1/p')
        read -r line
        read -r line  # 跳过 Content-Type 行
        read -r line  # 跳过空行
        # 保存文件
        cat > "/workspace/www/upload/$filename"
        echo "{\"message\": \"File uploaded successfully\"}"
    fi
done

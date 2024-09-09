# Using Dev Containers in VSCode

## Prerequisites

- [Docker](https://docs.docker.com/get-docker/)
- [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

## Steps to Use Dev Containers

0. Please make sure you have the root access to your machine.

1. **Reopen in Container**: Press `F1` or `Ctrl+Shift+P` to open the command palette. Type `Dev Containers: Rebuild Container` and select it. VS Code will start building your development container based on the configuration in the `.devcontainer` folder.

2. **Wait for the Container to Build**: The first time you do this, VS Code will build your container. This might take a few minutes depending on the size of the image and your internet connection.

3. **Start Coding**: Once the container is built and VS Code has reopened inside the container, you are ready to start coding. You can use the integrated terminal, debugger, and other features as you normally would.

4. No other steps, it's that simple!

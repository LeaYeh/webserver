# Setup Coding Style Linter

## Install the clangd VSCode extension

1. Open the Extensions view.

2. Search for `clangd`.

3. Click the `Install` button.

## Config VSCode settings

Create a `.vscode/settings.json` file in the root of your project with the following content:
```
{
    "[cpp]": {
        "editor.formatOnSave": true
    }
}
```

## Config clang-format

Create a `.clang-format` file in the root of your project with the following content:
```
BasedOnStyle: LLVM                          # Use the LLVM style as a base
IndentWidth: 4                              # Commonly used indent width
UseTab: Never                               # Prefer spaces, as is typical in older C++ code styles
BreakBeforeBraces: Allman                   # Use the Allman style for braces
AllowShortIfStatementsOnASingleLine: false  # Keep even short if statements on separate lines
ColumnLimit: 80                             # Keeping with the traditional 80 characters per line limit
PointerAlignment: Left                      # Place the asterisk close to the variable type e.g., int* x;
AllowShortFunctionsOnASingleLine: false     # Force functions to be on multiple lines
AllowShortBlocksOnASingleLine: false        # Force blocks to be on multiple lines
SortIncludes: true                          # Sort includes alphabetically
InsertNewlineAtEOF: true                    # Ensure a newline at the end of the file
```

## Select the default formatter

In VSCode, go to any source file and press `Ctrl + Shift + I` to `Format Document`.
You will be prompted to select a default formatter. Select `clangd`.

Now, every time you save a file (`Ctrl + S`), it will be automatically formatted according to the `.clang-format` file.

---

## Bonus: Install clang-format without clangd

clangd already includes clang-format, but if you want to install clang-format separately, you can do so with the following steps:

### With sudo

```bash
sudo apt-get install clang-format
```

### Without sudo (statically linked)

```bash
# Download the latest clang-format binary from a mirror into $HOME/bin
LATEST_URL=$(wget -qO- https://api.github.com/repos/muttleyxd/clang-tools-static-binaries/releases/latest | grep -oP '(?<="browser_download_url": ")[^"]*clang-format-[0-9]+_linux-amd64(?=")' | sort -V | tail -n 1)
wget -qO $HOME/bin/clang-format $LATEST_URL
chmod +x $HOME/bin/clang-format

# Add the path to the clang-format binary to your PATH
echo 'export PATH=$PATH:$HOME/bin' >> $HOME/.zshrc
echo 'export PATH=$PATH:$HOME/bin' >> $HOME/.bashrc
```

### Without sudo (dynamically linked)

```bash
# Download the latest clang-format binary from LLVM into $HOME/bin
LATEST_URL=$(wget -qO- https://api.github.com/repos/llvm/llvm-project/releases/latest | grep "browser_download_url.*clang.*x86_64-linux-gnu-ubuntu-.*.tar.xz" | cut -d : -f 2,3 | tr -d \" | xargs)
wget -qO- $LATEST_URL | tar -xJv -C $HOME/bin/ --strip-components=2 --occurrence=1 --wildcards \*/bin/clang-format
chmod +x $HOME/bin/clang-format

# Add the path to the clang-format binary to your PATH
echo 'export PATH=$PATH:$HOME/bin' >> $HOME/.zshrc
echo 'export PATH=$PATH:$HOME/bin' >> $HOME/.bashrc
```

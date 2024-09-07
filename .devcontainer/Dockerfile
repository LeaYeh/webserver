# Start from the latest Ubuntu version
FROM ubuntu:latest

# Update the system and install utils
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    sudo \
    cmake \
    meson \
    valgrind \
    build-essential \
    binutils \
    clang \
    zsh \
    git \
    wget \
    curl \
    python3 python3-venv python3-pip \
    libreadline-dev \
    libreadline8 \
    xorg libxext-dev zlib1g-dev libbsd-dev \
    libcmocka-dev \
    pkg-config

# Download and install the latest version of make
RUN wget http://ftp.gnu.org/gnu/make/make-4.3.tar.gz && \
    tar -xvzf make-4.3.tar.gz && \
    cd make-4.3 && \
    ./configure && \
    make && \
    sudo make install

# Install the stable version of Go
RUN wget https://go.dev/dl/go1.20.6.linux-arm64.tar.gz && \
    tar -C /usr/local -xzf go1.20.6.linux-arm64.tar.gz

# Set Go environment variables
ENV PATH="/usr/local/go/bin:${PATH}"

# Install oh-my-zsh
RUN sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" || true
RUN echo "source $HOME/.oh-my-zsh/oh-my-zsh.sh" >> $HOME/.zshrc

# Set the working directory in the container to /app
WORKDIR /app

CMD ["/bin/zsh"]

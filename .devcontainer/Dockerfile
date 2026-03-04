FROM gcc:14.2.0

RUN apt-get update && apt-get install -y \
    cmake python3 python3-dev python3-pip wrk clangd strace curl tar \
    && rm -rf /var/lib/apt/lists/*

RUN pip install numpy --break-system-packages

# Предустановка VS Code Server (опционально)
RUN mkdir -p /root/.vscode-server/bin

WORKDIR /workspace
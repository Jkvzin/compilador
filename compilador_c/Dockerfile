# Usar Ubuntu 32-bit como base (melhor para x86-32)
FROM i386/ubuntu:latest

# Instalar ferramentas necessárias
RUN apt-get update && apt-get install -y \
    nasm \
    binutils \
    gcc \
    make \
    && rm -rf /var/lib/apt/lists/*

# Criar pasta de trabalho
WORKDIR /app

# O container iniciará no bash por padrão
CMD ["/bin/bash"]

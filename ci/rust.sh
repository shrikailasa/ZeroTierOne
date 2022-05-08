#!/bin/bash
set -euo pipefail
IFS=$'\n\t'

if [ $# -eq 0 ]; then
    echo "This script requires RUST_TRIPLET as an argument"
    exit 1
else
    export RUST_TRIPLET=$1
    echo "Using RUST_TRIPLET=${RUST_TRIPLET}"
fi

cd /tmp
echo curl -sSf "https://static.rust-lang.org/rustup/dist/${RUST_TRIPLET}/rustup-init" -o rustup-init
curl -sSf "https://static.rust-lang.org/rustup/dist/${RUST_TRIPLET}/rustup-init" -o rustup-init
chmod +x ./rustup-init
ls -la ./rustup-init
file ./rustup-init
./rustup-init --default-host ${RUST_TRIPLET} --default-toolchain stable -y

/root/.cargo/bin/rustup install 1.60.0
/root/.cargo/bin/rustc --version
/root/.cargo/bin/cargo --version
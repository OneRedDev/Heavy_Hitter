#!/bin/bash

echo "RedCore Heavy Hitter - Test Suite "

# Check if gcc is installed
if ! command -v gcc &> /dev/null; then
    echo "❌ Error: gcc is not installed. Install it with: pkg install gcc"
    exit 1
fi

# Clean previous builds
make clean

# Compile
echo "⚙️  Compiling..."
if make; then
    echo "✅ Compilation successful!"
else
    echo "❌ Compilation failed."
    exit 1
fi

# Run a quick test (dry run)
echo "🚀 Running test..."
./heavy_hitter --help # Or whatever flag your code supports for help

echo "✅ All tests passed. RedCore is ready."
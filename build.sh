#!/bin/bash

set -e

echo "Building Simple Chess Games C Wrapper..."

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

# Configure and build
cmake ..
make -j$(nproc)

echo "Build completed successfully!"
echo ""
echo "Generated files in bin/ directory:"
echo "  - libsimplechess-c.so (shared library)"
echo "  - libsimplechess-c.a (static library)"
echo "  - test_suite (test executable)"
echo "  - test_suite_static (static test executable)"

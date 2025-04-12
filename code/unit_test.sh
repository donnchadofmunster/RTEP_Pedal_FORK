sudo ./build.sh

# Run the unit tests
echo "[unit_test.sh] Running unit tests..."
sudo ctest --test-dir build/src/test/unit_tests --output-on-failure
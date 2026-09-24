#!/bin/bash
set -e

echo "======================================================="
echo "         BankCore SDET Test Suite Runner"
echo "======================================================="
echo ""

echo "[1/3] Compiling BankCore & Unit Tests..."
make all

echo ""
echo "[2/3] Running C++ Unit Tests..."
./unit_tests.exe || ./unit_tests

echo ""
echo "[3/3] Running Python Pytest Subprocess CLI Harness..."
pytest -v

echo ""
echo "======================================================="
echo "    All BankCore SDET Reliability Tests Passed!"
echo "======================================================="

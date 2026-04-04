#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "[INFO] Build+run MOCK quote test..."
pushd "$SCRIPT_DIR" >/dev/null
g++ -std=c++14 -DTDX_TEST_MOCK test_tdx_quote_wrapper.cpp -o test_tdx_quote_wrapper_mock
./test_tdx_quote_wrapper_mock
popd >/dev/null

echo "[INFO] Done."

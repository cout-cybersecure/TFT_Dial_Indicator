Build and run on Linux.

Install Qt6 dev, CMake, and ONNX Runtime dev. Example on Ubuntu.

sudo apt-get update
sudo apt-get install -y cmake qt6-base-dev qt6-base-dev-tools qt6-tools-dev qt6-tools-dev-tools
Download ONNX Runtime prebuilt CPU package for Linux from the official site and set ONNXRUNTIME_DIR to its root. Or install onnxruntime-dev from your distro.

Python training environment.

python3 -m venv .venv
source .venv/bin/activate
pip install -r python/requirements.txt

Configure and build.

mkdir build
cd build
cmake -DONNXRUNTIME_DIR=/path/to/onnxruntime -DCMAKE_PREFIX_PATH=$(qtpaths6 --install-prefix) ..
cmake --build . -j

Run.

From the project root, create models dir if needed. Start the app.
./build/tft_dial

Enter the stock symbol and an Alpha Vantage API key. Click Start. The app shows Training/Loading while python trains and exports an ONNX. The dial begins updating every second as the fetcher polls intraday CSV. The prediction maps to [-1,1] where negative tilts left and positive tilts right.

Sub-millisecond local inference is achievable for the tiny model once data is in memory. Network polling and GUI refresh are slower by design. Use a provider with WebSocket streaming and reduce the timer if your data source and hardware allow it.
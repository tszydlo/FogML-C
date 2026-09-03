# FogML-C

A plain-C, host-buildable example for the FogML project — anomaly detection and classification
on resource-constrained devices, without any board framework (no Arduino/PlatformIO, no Zephyr).
It builds with a plain `Makefile` and `gcc`, so it can run directly on a desktop/laptop (Linux,
Windows, macOS). It uses:

* FogML-SDK [https://github.com/tszydlo/fogml_sdk]
* FogML tools [https://github.com/tszydlo/FogML]

More details on the algorithms used are described in the paper https://arxiv.org/abs/2206.14265 .

## Repository layout

* `src` — application source (`main.c`); generated model code goes in `src/fogml_generated`
* `lib/fogml_sdk` — the FogML SDK, vendored as a git submodule
* `dataset` — [OPS-SAT-AD](https://github.com/kplabs-pl/OPS-SAT-AD) telemetry dataset prep for the
  anomaly-detection example (see `dataset/README.md`)
* `tools` — Jupyter notebook(s) for training models and generating the C source consumed by
  `src/fogml_generated`
* `doc` — diagrams/images used in this README
* `run_all_channels.sh` — batch-runs every per-channel CSV in `dataset/` through `bin/fogml_c`

## Getting the code

`lib/fogml_sdk` is a git submodule, so clone with:

```bash
git clone --recurse-submodules <this repo URL>
```

or, if already cloned:

```bash
git submodule update --init --recursive
```

## Building

```bash
make        # release build -> bin/fogml_c, no debug info
make debug  # debug build (-g) -> bin/fogml_c, objects under build/debug
make clean  # removes build/ and bin/
```

`make run` also exists but invokes the binary with no arguments, which just prints its usage
message — a real CSV path is required, so see **Usage** below instead.

## Usage

```bash
./bin/fogml_c dataset/<channel>.csv
```

e.g. `./bin/fogml_c dataset/CADC0886-5.csv`. Each per-channel CSV's first line is a legend/header
and is skipped; every row after that is one labeled segment. For each row the program prints its
metadata (`segment`, `anomaly`, `train`, `channel`, `sampling`, `duration`, `len`), then:

* if the row belongs to the training split and is labeled non-anomalous (`train==1 && anomaly==0`),
  its feature vector updates the reservoir-sampled Local Outlier Factor (LOF) model
  (`fogml_learning`, via `fogml_sdk`'s `anomaly_rt`);
* otherwise (`train==0`), the feature vector is scored against that model (`fogml_processing`),
  the LOF `score` is printed, and the row is classified anomalous when `score > 2.0` (this
  threshold is set in `src/fogml_config.h` and can be tuned there) — that classification is then
  checked against the row's ground-truth `anomaly` label.

After all rows are processed, it prints `Train lines: X / Y`, `Matched count: Z`, and
`Accuracy: ...` — the fraction of non-training rows whose classification matched the ground-truth
label.

## Running all channels

```bash
./run_all_channels.sh
```

Requires `bin/fogml_c` to already be built (`make`). Runs every `dataset/CADC*.csv` file (the
per-channel splits — not the unsplit `dataset/dataset.csv`) through the binary in turn, printing
each channel's results and accuracy.

## Status

This example implements a working, semi-supervised online anomaly detector for the OPS-SAT-AD
dataset: reservoir sampling + Local Outlier Factor from `fogml_sdk`, trained incrementally on the
non-anomalous rows of each channel's training split and evaluated against the remaining rows. See
**Usage** above for what it prints, and `src/fogml_config.h` for the model configuration
(reservoir size, LOF `k`, anomaly-score threshold).

## Bibliography

Please mention us and cite our papers if you use this work and find it useful.

```
@misc{OnlineAnomalySzydlo2022,
  doi = {10.48550/ARXIV.2206.14265},
  url = {https://arxiv.org/abs/2206.14265},
  author = {Szydlo, Tomasz},
  keywords = {Machine Learning (cs.LG), FOS: Computer and information sciences, FOS: Computer and information sciences},
  title = {Online Anomaly Detection Based On Reservoir Sampling and LOF for IoT devices},
  publisher = {arXiv},
  year = {2022},
  copyright = {arXiv.org perpetual, non-exclusive license}
}
```

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

## Getting the code

`lib/fogml_sdk` is a git submodule, so clone with:

```bash
git clone --recurse-submodules <this repo URL>
```

or, if already cloned:

```bash
git submodule update --init --recursive
```

## Building and running

```bash
make        # builds bin/fogml_c
make run    # builds (if needed) and runs it
make clean  # removes build/ and bin/
```

## Status

This example currently wires up the build against `fogml_sdk` (DSP, scaler, anomaly/LOF,
reservoir sampling) and runs a minimal "Hello World!" to prove the include paths and linking are
correct. The actual anomaly-detection/classification pipeline (feature extraction → scaling →
model) is not yet wired into `main.c`.

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

# Hydrogenic Python Interface

This directory contains an installable Python interface for the Hydrogenic-demo C++ atom routines.

## Install

Use `python3`, not `python`, from the repository root. Since this repository has a `python/` folder, some shell setups can otherwise resolve `python` incorrectly and report `Permission denied`.

To build the Python extension and run the notebook with existing Homebrew/JupyterLab installations, you need:

- `numpy`;
- `matplotlib`;
- `jupyterlab` or `notebook`;
- build-time `setuptools` and `pybind11`;
- compiled dependencies: GCC with OpenMP, GSL, and Boost headers.

On macOS with Homebrew:

```sh
brew install gcc gsl boost python-setuptools pybind11 numpy matplotlib jupyterlab
```

Build the local Python extension:

```sh
make python-build
make python
make py
```

Open the demo notebook:

```sh
make run-jupyter
```

`make run-jupyter` runs `make python-build` first, then opens the notebook with an available Jupyter command (`jupyter lab`, `jupyter-lab`, or `jupyter-notebook`) and sets `PYTHONPATH` to include `python/src`. This lets the notebook import the local `hydrogenic` extension without installing it into JupyterLab's private Python environment.

The Makefile workflow avoids installing the package into your system Python. If you deliberately want a pip install into a Homebrew-managed Python, Homebrew may require:

```sh
python3 -m pip install --user --break-system-packages ./python
```

The setup script defaults to Homebrew paths and tries Homebrew GCC first. If needed, override compiler and dependency paths explicitly:

```sh
CXX=/opt/homebrew/bin/g++-16 \
GSL_INC_PATH=/opt/homebrew/include \
GSL_LIB_PATH=/opt/homebrew/lib \
BOOST_INC_PATH=/opt/homebrew/include \
python3 -m pip install ./python
```

## Quick use

```python
import numpy as np
from hydrogenic import hydrogen, oxygen

h = hydrogen(shells=5)
print(h.level(2, 1))
print(h.transition(2, 1, 1, 0))

o = oxygen(shells=30)
nu0 = o.level(1, 0)["ionization_frequency_hz"]
nu = np.geomspace(1.001, 10.0, 100)*nu0
data = o.photoionization(1, 0, nu)
print(data["sigma_cm2"])
```

Photoionization cross sections require `recombination_mode=1` or `recombination_mode=2`; this is the default for the helpers above.

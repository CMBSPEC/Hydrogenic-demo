# Hydrogenic Python Interface

This directory contains an installable Python interface for the Hydrogenic-demo C++ atom routines.

## Install

From the repository root:

```sh
python -m pip install ./python
```

For editable development:

```sh
python -m pip install -e "./python[notebooks]"
```

The native extension is built with `pybind11` and links against GSL. On macOS with Homebrew, install the compiled dependencies with:

```sh
brew install gcc gsl boost
```

The setup script defaults to Homebrew paths and tries Homebrew GCC first. If needed, override compiler and dependency paths explicitly:

```sh
CXX=/opt/homebrew/bin/g++-16 \
GSL_INC_PATH=/opt/homebrew/include \
GSL_LIB_PATH=/opt/homebrew/lib \
BOOST_INC_PATH=/opt/homebrew/include \
python -m pip install ./python
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


# Hydrogenic C++ Documentation

This note gives a compact overview of the C++ part of the Hydrogenic-demo workspace. The code builds a small executable, `hydrogenic_atom_demo`, that exercises the hydrogenic atom routines in `Hydrogenic.vX` together with numerical utilities from `Tools`.

## Project layout

| Path | Purpose |
| --- | --- |
| `main.cpp` | Example program that constructs hydrogenic atoms and prints representative atomic data. |
| `Hydrogenic.vX/Atom.*` | Core `Atom`, `Atomic_Shell`, `Electron_Level`, and `Gas_of_Atoms` classes. |
| `Hydrogenic.vX/Oscillator_strength.*` | Bound-bound oscillator strength and transition support. |
| `Hydrogenic.vX/Photoionization_cross_section.*` | Photoionization cross sections and Gaunt factors. |
| `Hydrogenic.vX/Quadrupole_lines_HI.*` | Electric quadrupole line rates for hydrogenic atoms. |
| `Hydrogenic.vX/Rec_Phot_BB.*` and `Recombination_Integrals.*` | Recombination and photon interaction helpers. |
| `Tools/` | Shared constants, integration routines, line profiles, and simple numerical helpers. |
| `Makefile` and `Makefile.in` | Build entry points for the demo executable. |

## What the C++ example does

The demo constructs hydrogenic atoms with a selected number of complete shells. Each shell contains angular momentum levels with `l = 0 ... n - 1`. The code can report:

- level degeneracies, ionization energies, ionization frequencies, total decay rates, and lifetimes;
- selected bound-bound transition frequencies, wavelengths, and Einstein `A21` coefficients;
- electric dipole transitions and optional electric quadrupole transitions;
- photoionization cross sections and Gaunt factors at frequencies above threshold;
- simple rescaling behavior for the fine-structure constant and electron mass.

The current `main.cpp` includes two examples:

- hydrogen with `Z = 1`, 5 shells, quadrupole lines enabled, and Storey-Hummer recombination interpolation enabled;
- hydrogenic oxygen, O VIII, with `Z = 8`, 30 shells, quadrupole lines enabled, and Storey-Hummer recombination interpolation enabled.

## Scientific background

The hydrogenic atom setup was developed for detailed calculations of cosmological recombination. This requires multi-shell hydrogenic atoms with resolved angular momentum sub-states, dipole transitions, quadrupole transitions, photoionization cross sections, and recombination coefficients. The C++ example in `main.cpp` cites the main background papers for this use case.

## References

[1] J. Chluba and R. M. Thomas, "Towards a complete treatment of the cosmological recombination problem", MNRAS 412, 748, 2011. <https://ui.adsabs.harvard.edu/abs/2011MNRAS.412..748C/abstract>

[2] J. Chluba, J. A. Rubino-Martin, and R. A. Sunyaev, "Cosmological hydrogen recombination: populations of the high level sub-states", MNRAS 374, 1310, 2007. <https://ui.adsabs.harvard.edu/abs/2007MNRAS.374.1310C/abstract>

[3] J. Chluba and Y. Ali-Haimoud, "CosmoSpec: Fast and detailed computation of the cosmological recombination radiation from hydrogen and helium", MNRAS 456, 3494, 2016. <https://ui.adsabs.harvard.edu/abs/2016MNRAS.456.3494C/abstract>

## Dependencies

The build expects:

- a C++17 compiler with OpenMP support;
- GSL headers and libraries;
- Boost headers;
- GNU `make`.

On macOS with Homebrew, the typical setup is:

```sh
brew install gcc gsl boost
```

`Makefile.in` currently defaults to:

```make
CXX = /opt/homebrew/bin/g++-16
DEF_INC_PATH = /opt/homebrew/include
DEF_LIB_PATH = /opt/homebrew/lib
```

If your compiler or Homebrew prefix differs, override these variables when invoking `make`.

## Build and run

From the workspace root:

```sh
make
```

Run the demo:

```sh
make run
```

Clean generated build products:

```sh
make clean
```

Update submodules if the project is checked out with submodule dependencies:

```sh
make update-modules
```

Example build override:

```sh
make CXX=/opt/homebrew/bin/g++-16 DEF_INC_PATH=/opt/homebrew/include DEF_LIB_PATH=/opt/homebrew/lib
```

The executable is written to:

```sh
./hydrogenic_atom_demo
```

Intermediate object and dependency files are written under:

```sh
./build/
```

## Python and notebooks

This workspace also has a planned Python part. The Python material will live in a `python/` area and use Jupyter notebooks to further illustrate the same physics: energy levels, line series, photoionization behavior, recombination examples, and comparisons with the C++ output. Those notebooks are intended as exploratory and visualization-oriented companions to the compiled C++ routines.

## Regenerating this PDF

The PDF version of this document can be regenerated from the Markdown source with Pandoc and Typst:

```sh
pandoc Documentation/Hydrogenic_cpp_Documentation.md --from gfm --pdf-engine=typst --output Documentation/Hydrogenic_cpp_Documentation.pdf
```

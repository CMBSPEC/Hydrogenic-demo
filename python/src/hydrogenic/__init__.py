"""Python interface for the Hydrogenic-demo C++ atom routines."""

from ._core import HydrogenicAtom


Atom = HydrogenicAtom


def hydrogen(
    shells: int = 5,
    *,
    include_quadrupole_lines: bool = True,
    recombination_mode: int = 1,
    message_level: int = -1,
) -> HydrogenicAtom:
    """Create a hydrogen atom with the same defaults used by the C++ demo."""
    return HydrogenicAtom(
        shells,
        1,
        1.0,
        include_quadrupole_lines,
        recombination_mode,
        message_level,
    )


def oxygen(
    shells: int = 30,
    *,
    include_quadrupole_lines: bool = True,
    recombination_mode: int = 1,
    message_level: int = -1,
) -> HydrogenicAtom:
    """Create hydrogenic oxygen, O VIII, with the C++ demo defaults."""
    return HydrogenicAtom(
        shells,
        8,
        15.879,
        include_quadrupole_lines,
        recombination_mode,
        message_level,
    )


__all__ = [
    "Atom",
    "HydrogenicAtom",
    "hydrogen",
    "oxygen",
]


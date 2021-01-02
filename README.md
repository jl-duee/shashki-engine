# Shashki-Engine #

Shashki-Engine is an engine library that can be used for russian draughts applications. It is written in C++ and is provided together with a CLI (Command Line Interface) for some simple usage. The project is kicked of by Jean-Luc Düe in 2021 under the GPL-3.0 license.

#

## Features to implement: ##

- Board evaluation functionality
- Engine tree traversal with minimax and alpha/beta pruning
- CLI with some simple visualizations

## Technical improvements to work on: ##

- Code formatting (maybe based on some standard)
- Proper code documentation
- Unit tests
- Optional value check (currently checking twice)
- Move generation completely on stack (no heap allocations)
- Move semantics (decrease number of copying on stack)

#

## License: ##

Please check out the file "COPYING" in the root of the repository.

#

## Setup: ##

These are still some minimal build instructions as the project is in a kick-off stage.

- Clone repository
- Create a "build" directory in the root of the cloned repository
- Run CMake from there

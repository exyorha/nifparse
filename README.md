# nifparse

nifparse is a library implementing a parser for NIF files: 3D model files by
The Elder Scrolls series and many other games, created as a more performant
and space-efficient alternative to niflib.

See nifparse-test for an usage example.

# Building

nifparse may be built using normal CMake procedures, and is generally
intended to included into an outer project as a submodule or by any other
means.

Ruby interpreter is required during build process, but not in runtime.

Please note that nifparse uses git submodules, which should be retrieved
before building.

# Licensing

nifparse is licensed under the terms of the MIT license (see LICENSE).

Please note that while nifparse *itself* is licensed under MIT license, as is
everything else in this repository, GPL-licensed nifxml is compiled into 
a binary representation and linked into the final executable during build
process. As such, that final executable may be considered encumbered by the
terms of the GPL.

Note that nifparse also references the MIT-licensed half-precision float
library by Mike Acton as a submodule.


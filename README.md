# HCAE

Himrich's CPU Architecture and Emulator (HCAE)

(DEV: V1.0 work in progress)

More information can be found [here](docs/introduction.md).
The MIT License details are in [here](LICENSE).

## Installation

### Requirements

 - A C compiler that supports C11 or later
 - GNU Make

Clone with:

```Bash
git clone https://github.com/hzhanfer/HCAE.git # or:
# git clone git@github.com:hzhanfer/HCAE.git
```

To build an HCA emulator, run:

```Bash
make build TARGET=(your target)
```

And to install it, run:

```Bash
sudo make install TARGET=(your target) # the install recipe also runs the build recipe
```

This then installs ``hcae(target)-(version)``, ``hcae(target)-config-(version)``, ``hcae(target)``, ``hcae(target)-config`` into ``/usr/local/bin/``, ``libhcae(target).so.(major version).(minor version).(patch)``, ``libhcae(target).a.(major version).(minor version).(patch)`` ``libhcae(target).so.(major)``, ``libhcae(target).a.(major)``, ``libhcae(target).so``, ``libhcae(target).a`` into ``/usr/local/lib/``, ``src/hcae/*`` into ``/usr/local/include/hcae/(version)/``, and ``src/target/(target)/hcae/*`` into ``/usr/local/include/hcae/(version)/(target)/``.

The installation path differs by environment, if there is an environment variable named ``PREFIX`` it uses it as the base installation path, if it doesn't exist, it uses ``/usr/local/`` by default. This is to support environments such as Termux.

Example:

```Bash
sudo make install TARGET=32v1 # installs hcae32v1 into your machine
```

See additional options with:

```Bash
make help
```


#! /usr/bin/env python3
import argparse
import sys
import glob
from typing import Dict

# Hack to add the libnaomi scripts to the python system path
sys.path.append(r'/opt/toolchains/naomi')

from naomi import NaomiRom


def main() -> int:
    # Create the argument parser
    parser = argparse.ArgumentParser(
        description="Utility for printing information about a ROM file.",
    )
    parser.add_argument(
        'path',
        metavar='PATH',
        type=str,
        help='directory of games we should generate a menu for.',
    )

    # Grab what we're doing
    args = parser.parse_args()

    files = set(glob.glob(args.path + '/*.bin'))
    for f in sorted(files):
        # Grab the rom, parse it
        with open(f, "rb") as fp:
            data = fp.read()

        # First, assume its a Naomi ROM
        naomi = NaomiRom(data)
        if naomi.valid:
            print(f"{naomi.names[NaomiRom.REGION_JAPAN].strip('- '):>32} - ({naomi.date}) - [{naomi.publisher}]")
    print("^")

if __name__ == "__main__":
    sys.exit(main())

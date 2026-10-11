"""Start the pySLDprofile GUI without installing the package.

    python run_gui.py [datafile]
"""

import sys
from pathlib import Path

try:
    import pysldprofile  # noqa: F401  (installed, e.g. pip install -e .[gui])
except ImportError:
    sys.path.insert(0, str(Path(__file__).resolve().parent / "src"))

from pysldprofile.gui import main

if __name__ == "__main__":
    sys.exit(main())

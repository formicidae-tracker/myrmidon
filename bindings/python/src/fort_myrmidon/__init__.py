from __future__ import annotations

import os
from ._fort_myrmidon import *
from ._fort_myrmidon import _installSignalHandler

_installSignalHandler(os.path.dirname(__file__))

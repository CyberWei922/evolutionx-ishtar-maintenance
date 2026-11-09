# Extraction adapter for the reviewed standard-library implementation.
from pathlib import Path
import runpy

def blob_fixup(ctx, file, file_path, *args, **kwargs):
    runpy.run_path(str(Path(__file__).with_name("apply.py")))["apply"](file_path)

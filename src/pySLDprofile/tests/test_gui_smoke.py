import os
import time

import pytest

pytest.importorskip("PySide6")
os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6 import QtWidgets  # noqa: E402

from pysldprofile.gui import MainWindow  # noqa: E402

DATA = os.path.join(os.path.dirname(__file__), "..", "examples", "data", "inhomogeneous_sphere_r5nm.dat")


def test_gui_load_and_fit():
    app = QtWidgets.QApplication.instance() or QtWidgets.QApplication([])
    w = MainWindow()
    w.load_file(DATA)
    assert w.data is not None and len(w.data[0]) == 50
    w.sp_n.setValue(30)
    w.ck_fix.setChecked(True)
    w.ck_auto.setChecked(False)
    w.sp_rmax.setValue(5.0)
    w.on_run()
    t = time.time()
    while (w.worker is not None or w.result is None) and time.time() - t < 120:
        app.processEvents()
        time.sleep(0.02)
    assert w.result is not None
    assert 0.5 < w.result.chi2_red < 1.5
    w.close()

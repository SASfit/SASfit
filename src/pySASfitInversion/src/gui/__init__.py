"""
gui
===

Thin Qt (PySide6) layer over the ``sasfit_inversion`` library. No numerics
live here -- every computation (loading, background fitting/subtraction,
kernel construction, solving) calls straight into the library, so the
library stays usable headless/scriptable/testable on its own. See
``gui.app.MainWindow``.
"""

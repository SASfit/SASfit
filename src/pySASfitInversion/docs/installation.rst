Installation
============

From the project root:

.. code-block:: bash

   pip install -e ".[gui,dev]"

Install just the library, without the Qt/GUI dependencies (``PySide6``,
``matplotlib``), with:

.. code-block:: bash

   pip install -e .

Requirements
------------

- Python >= 3.10
- ``numpy``, ``scipy`` (core library)
- ``PySide6``, ``matplotlib`` (GUI only, the ``[gui]`` extra)
- ``pytest`` (the ``[dev]`` extra, for running ``tests/``)
- ``sphinx``, ``furo``, ``sphinx-autodoc-typehints`` (the ``[docs]`` extra,
  for building this documentation)

Building this documentation
----------------------------

.. code-block:: bash

   pip install -e ".[docs]"
   cd docs
   make html        # Linux/macOS
   make.bat html    # Windows

Open ``docs/_build/html/index.html`` afterwards.

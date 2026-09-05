#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Test harness to check if Robertus SHS and RY Polydisperse Yukawa tabs
can be imported and instantiated, and what error messages they produce.
"""

import sys
import traceback
import tkinter as tk
from tkinter import ttk

def test_tab(module_name, class_name, label):
    """Try to load and instantiate a tab and report what happens."""
    print(f"\n{'='*70}")
    print(f"Testing: {label}")
    print(f"Module: {module_name}.{class_name}")
    print('='*70)
    
    try:
        mod = __import__(module_name, fromlist=[class_name])
        cls = getattr(mod, class_name)
        print(f"✓ Module imported successfully")
        
        # Try to instantiate with a dummy notebook
        root = tk.Tk()
        root.withdraw()
        notebook = ttk.Notebook(root)
        
        try:
            tab = cls(notebook)
            notebook.add(tab, text=label)
            print(f"✓ Tab instantiated and added to notebook successfully")
            root.destroy()
            return True
        except Exception as e:
            print(f"✗ Failed to instantiate tab")
            print(f"\nError: {type(e).__name__}")
            print(f"\nMessage:\n{str(e)[:500]}...\n" if len(str(e)) > 500 else f"\nMessage:\n{e}\n")
            root.destroy()
            return False
            
    except Exception as e:
        print(f"✗ Failed to import module")
        print(f"\nError: {type(e).__name__}")
        print(f"\nMessage:\n{str(e)[:500]}...\n" if len(str(e)) > 500 else f"\nMessage:\n{e}\n")
        return False


if __name__ == "__main__":
    results = {}
    
    tabs_to_test = [
        ("polydisperse_yukawa_tab", "PolydisperseYukawaTab", "Polydisperse Yukawa (reference - should work)"),
        ("robertus_shs_tab", "RobertusSHSTab", "Robertus SHS"),
        ("ry_polydisperse_yukawa_tab", "RYPolydisperseYukawaTab", "RY Polydisperse Yukawa"),
    ]
    
    for module_name, class_name, label in tabs_to_test:
        results[(module_name, class_name)] = test_tab(module_name, class_name, label)
    
    print(f"\n{'='*70}")
    print("Summary")
    print('='*70)
    for (module_name, class_name), success in results.items():
        status = "✓ PASS" if success else "✗ FAIL"
        print(f"{status}: {module_name}.{class_name}")
    
    print(f"\nIf Robertus SHS and RY Polydisperse tabs fail with messages about")
    print(f"missing rshs_* or ry_polydisperse_* functions, you need to build")
    print(f"the core libraries from source.")
    print(f"\nSee the build instructions in: src/pyozGUI/BUILD_ROBERTUS_RY.md")

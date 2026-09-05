#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Final verification: show what error message each tab displays
and verify they handle import errors gracefully.
"""

import sys

# Test if the tabs catch import errors properly
print("="*70)
print("Checking tab error handling")
print("="*70)

# Test Robertus wrapper
print("\n1. Testing robertusWrapper import...")
try:
    from robertusWrapper import RobertusSHS
    print("   ✓ Wrapper loaded successfully")
except Exception as e:
    print(f"   ✗ Error (expected): {type(e).__name__}")
    print(f"   Message: {str(e).split(chr(10))[0]}...")

# Test Robertus SAS layer
print("\n2. Testing robertus_shs_sas import...")
try:
    from robertus_shs_sas import RobertusSHSSAS
    print("   ✓ SAS layer loaded successfully")
except Exception as e:
    print(f"   ✗ Error (expected): {type(e).__name__}")
    print(f"   Message: {str(e).split(chr(10))[0]}...")

# Test Robertus tab with error handling
print("\n3. Testing robertus_shs_tab import error handling...")
try:
    import robertus_shs_tab
    if robertus_shs_tab.IMPORT_ERROR is not None:
        print(f"   ✓ Tab caught import error gracefully")
        print(f"   Captured error: {type(robertus_shs_tab.IMPORT_ERROR).__name__}")
        print(f"   Message: {str(robertus_shs_tab.IMPORT_ERROR).split(chr(10))[0]}...")
    else:
        print("   ✓ Tab imported without errors (wrapper works)")
except Exception as e:
    print(f"   ✗ Unexpected error: {type(e).__name__}: {e}")

# Test RY wrapper
print("\n4. Testing rypolydisperseWrapper import...")
try:
    from rypolydisperseWrapper import RYPolydisperseYukawa
    print("   ✓ Wrapper loaded successfully")
except Exception as e:
    print(f"   ✗ Error (expected): {type(e).__name__}")
    print(f"   Message: {str(e).split(chr(10))[0]}...")

# Test RY tab with error handling
print("\n5. Testing ry_polydisperse_yukawa_tab import error handling...")
try:
    import ry_polydisperse_yukawa_tab
    if ry_polydisperse_yukawa_tab.IMPORT_ERROR is not None:
        print(f"   ✓ Tab caught import error gracefully")
        print(f"   Captured error: {type(ry_polydisperse_yukawa_tab.IMPORT_ERROR).__name__}")
        print(f"   Message: {str(ry_polydisperse_yukawa_tab.IMPORT_ERROR).split(chr(10))[0]}...")
    else:
        print("   ✓ Tab imported without errors (wrapper works)")
except Exception as e:
    print(f"   ✗ Unexpected error: {type(e).__name__}: {e}")

print("\n" + "="*70)
print("Verification complete")
print("="*70)
print("\nExpected behavior:")
print("- Robertus and RY wrappers fail with 'function ... not found' errors")
print("- Tabs catch these errors and show helpful error notices")
print("- GUI remains functional with other tabs")
print("\nTo activate these tabs, build the core libraries per BUILD_ROBERTUS_RY.md")

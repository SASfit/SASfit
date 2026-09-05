import ctypes as ct
import os

lib_path = os.path.join('..', 'plugins', 'robertus_shs', 'lib', 'libsasfit_robertus_shs.dll')
full_path = os.path.abspath(lib_path)
print(f"Loading: {full_path}")

try:
    lib = ct.CDLL(full_path)
    print("✓ Library loaded successfully")
    
    # Try common function names
    names_to_try = [
        'rshs_solve',
        'rshs_structure',
        'ry_structure',
        'sasfit_sd_robertus_shs',
        'sasfit_ff_robertus_shs',
        'init_rshs',
        'sasfit_ff_sd_robertus_shs',
        'sasfit_sd_robertus',
    ]
    
    found = []
    for name in names_to_try:
        try:
            getattr(lib, name)
            found.append(f"✓ {name}")
        except AttributeError:
            found.append(f"✗ {name}")
    
    print("\nFunction search results:")
    for result in found:
        print(f"  {result}")
        
except Exception as e:
    print(f"Error loading: {type(e).__name__}: {e}")

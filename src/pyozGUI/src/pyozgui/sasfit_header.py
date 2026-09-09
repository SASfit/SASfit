# -*- coding: utf-8 -*-
r"""
Parse the parameter table out of a SASfit plugin header.

Each documented function in a SASfit header carries a Doxygen table

    \par Required parameters:
         <table border="0"><tr>
          <td>\b R</td>
          <td>radius</td>
          <td>(default=10)</td>
          <td>[0;+inf)</td>
         </tr><tr>
          ...

whose ROW POSITION is the index into param->p[], including slots the plugin
does not use (those are documented as "dummy"). That is the whole mapping
from name to array slot; no other source is needed. In particular there is no
need to read the #define lines in the .c file -- which are not shipped with
the binary builds anyway.

WHY A TOLERANT PARSER
---------------------
The tables are not uniformly populated. Measured across the sections of
sasfit_sphericalshells.h (build 260905165034):

    ff_spherical_shell_iii   4 rows, label + description + default + domain
    ff_spherical_shell_i     4 rows, but p[1] has a default and NO domain
    ff_sphere                4 rows, complete, with p[1] and p[2] "dummy"
    ff_multilamellar_vesicle 6 rows, p[0]-p[1] complete, p[2] description
                             only, p[3]-p[5] label only
    ff_rnd_multilamellar_vesicle  1 row, label only

So a parser that assumes four fields per row fails on real headers, and one
that assumes a table exists at all fails on some functions entirely.
Everything beyond the label is therefore optional, and absent information is
reported as None rather than guessed. The caller decides what to do about a
missing default -- inventing one silently is how a wrong value ends up in a
fit.

ff_sphere is the most complete of these and is the reference for what a fully
populated table should look like:

    p[0] = R      radius               (default=10)    [0;+inf)
    p[1] = dummy
    p[2] = dummy
    p[3] = eta    scattering_contrast  (default=1e10)  (-inf;+inf)

NOTE the section names carry the Doxygen \ingroup prefix -- "ff_sphere", not
"sphere" -- so a name lookup must strip or match on it.

This module deliberately imports nothing from SASfit or SASstudio: it is a
single regex pass over a text file, so it can be dropped into SASstudio's
Plugin class as an additional parse alongside the existing parameter_labels
without touching anything else.
"""
import os
import re


_SECTION = re.compile(r"/\*\s*#+\s*start\s+(\w+)")
_TABLE = re.compile(r"\\par\s+Required parameters:(.*?)\*/", re.S)
_DEFAULT = re.compile(r"default\s*=\s*([-+0-9.eE]+)")
#Domain as written in the headers: [0;+inf), (-inf;+inf), [0;1] and so on.
_DOMAIN = re.compile(r"([\[\(])\s*([^;]+?)\s*;\s*([^\]\)]+?)\s*([\]\)])")


def _cellText(cell):
    """Strip Doxygen markup and HTML from one <td> cell."""
    txt = cell.split("</td>")[0]
    txt = re.sub(r"\\b|\\f\$|\\f\[|\\f\]|\\a", " ", txt)
    txt = re.sub(r"<[^>]*>", " ", txt)
    txt = txt.replace("*", " ")
    return " ".join(txt.split())


def _number(text):
    if text is None:
        return None
    t = text.strip().lower()
    if t in ("-inf", "-infinity"):
        return float("-inf")
    if t in ("+inf", "inf", "+infinity", "infinity"):
        return float("inf")
    try:
        return float(t)
    except ValueError:
        return None


def parseParameterTable(text):
    """Rows of one \\par Required parameters table.

    Returns a list of dicts, one per p[] slot in order:

        {"index", "label", "description", "default", "min", "max",
         "minInclusive", "maxInclusive", "dummy"}

    Anything the header does not state is None. `dummy` marks a slot the
    plugin documents as unused -- it still occupies an index and must still
    be passed.
    """
    out = []
    for i, row in enumerate(text.split("<tr>")[1:]):
        cells = [_cellText(c) for c in row.split("<td>")[1:]]
        cells = [c for c in cells if c != ""]
        if not cells:
            continue
        entry = {"index": i, "label": cells[0], "description": None,
                 "default": None, "min": None, "max": None,
                 "minInclusive": None, "maxInclusive": None,
                 "dummy": cells[0].lower().startswith("dummy")}
        for cell in cells[1:]:
            m = _DEFAULT.search(cell)
            if m and entry["default"] is None:
                entry["default"] = _number(m.group(1))
                continue
            m = _DOMAIN.search(cell)
            if m and entry["min"] is None:
                entry["minInclusive"] = m.group(1) == "["
                entry["min"] = _number(m.group(2))
                entry["max"] = _number(m.group(3))
                entry["maxInclusive"] = m.group(4) == "]"
                continue
            if entry["description"] is None:
                entry["description"] = cell
        out.append(entry)
    return out


def parseHeader(path):
    """{function name: [parameter dicts]} for every section of a header.

    Functions without a parameter table are included with an empty list, so
    that a caller can tell "documented but no table" from "not in this header
    at all".
    """
    with open(path, "r", errors="replace") as fh:
        text = fh.read().replace("\r\n", "\n")
    result = {}
    marks = [(m.start(), m.group(1)) for m in _SECTION.finditer(text)]
    if not marks:
        m = _TABLE.search(text)
        return {os.path.splitext(os.path.basename(path))[0]:
                (parseParameterTable(m.group(1)) if m else [])}
    marks.append((len(text), None))
    for (start, name), (end, _) in zip(marks, marks[1:]):
        chunk = text[start:end]
        m = _TABLE.search(chunk)
        result[name] = parseParameterTable(m.group(1)) if m else []
    return result


def findParameters(name, pluginDir):
    """Parameter dicts for one function, searching a directory of headers.

    Matches with or without the Doxygen \\ingroup prefix, so both "sphere"
    and "ff_sphere" resolve. Returns None if the function is not found, and
    an empty list if it is found but carries no parameter table -- the two
    cases need different handling by a caller and should not be conflated.
    """
    wanted = {name, f"ff_{name}", f"sq_{name}", f"sd_{name}"}
    if not os.path.isdir(pluginDir):
        return None
    for fn in sorted(os.listdir(pluginDir)):
        if not fn.lower().endswith(".h"):
            continue
        try:
            sections = parseHeader(os.path.join(pluginDir, fn))
        except OSError:
            continue
        for section, params in sections.items():
            if section in wanted:
                return params
    return None


def defaultsAndBounds(params):
    """(values, lower, upper) lists ready to hand to a fit.

    Slots with no documented default get 0.0, which is correct for a dummy
    and deliberately conspicuous for anything else; slots with no documented
    domain get -inf/+inf. Callers that care about the difference should read
    the dicts instead, where absent information is None.
    """
    values, lo, hi = [], [], []
    for p in params:
        values.append(0.0 if p["default"] is None else p["default"])
        lo.append(float("-inf") if p["min"] is None else p["min"])
        hi.append(float("inf") if p["max"] is None else p["max"])
    return values, lo, hi


def describe(params):
    """Human-readable summary, one line per slot."""
    lines = []
    for p in params:
        bits = [f"p[{p['index']}]", p["label"]]
        if p["description"]:
            bits.append(p["description"])
        if p["default"] is not None:
            bits.append(f"default={p['default']:g}")
        if p["min"] is not None:
            lb = "[" if p["minInclusive"] else "("
            rb = "]" if p["maxInclusive"] else ")"
            bits.append(f"{lb}{p['min']}; {p['max']}{rb}")
        lines.append("  ".join(bits))
    return "\n".join(lines)


if __name__ == "__main__":                                 # pragma: no cover
    import sys
    for path in sys.argv[1:]:
        print(f"== {os.path.basename(path)}")
        for fn, params in parseHeader(path).items():
            if not params:
                print(f"   {fn}: (no parameter table)")
                continue
            print(f"   {fn}:")
            for line in describe(params).splitlines():
                print("      " + line)

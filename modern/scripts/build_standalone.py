#!/usr/bin/env python3
"""Rebuild archived v1.0.0 standalone HTML exactly from its source components.

This is a packaging/integrity check; it does not validate serial hardware.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
source = ROOT / "src"
template = (source / "template.html").read_bytes()
style = (source / "style.css").read_bytes()
script = (source / "browser.js").read_bytes()
if template.count(b"{{INLINE_STYLE}}") != 1 or template.count(b"{{INLINE_SCRIPT}}") != 1:
    raise SystemExit("Unexpected source template markers")
rebuilt = template.replace(b"{{INLINE_STYLE}}", style).replace(b"{{INLINE_SCRIPT}}", script)
original = (ROOT / "Incubator_Monitor.html").read_bytes()
if rebuilt != original:
    raise SystemExit("Reconstructed HTML differs from the archived standalone build")
output = ROOT / "dist" / "Incubator_Monitor.html"
output.parent.mkdir(parents=True, exist_ok=True)
output.write_bytes(rebuilt)
print(f"Byte-exact rebuild confirmed: {output} ({len(rebuilt)} bytes)")

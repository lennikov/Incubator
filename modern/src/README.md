# Source layout and reproducible build

The archived v1.0.0 standalone HTML has been losslessly split into:

- `src/template.html`: the unchanged HTML shell with two insertion markers.
- `src/style.css`: the verbatim CSS embedded by the released HTML.
- `src/browser.js`: the verbatim JavaScript embedded by the released HTML.
- `scripts/build_standalone.py`: puts the three source files together; output is a standalone HTML file.

**Source provenance:** These files were extracted from the archived October 2026 single-file application, not reimplemented or minified. The reconstruction preserves whitespace and source text. Review [ARCHITECTURE.md](../ARCHITECTURE.md) and [../README.md](../README.md) for the operational constraints.

## Rebuild

From the repository root:

```bash
python modern/scripts/build_standalone.py
```

This writes `modern/dist/Incubator_Monitor.html`. Compare the rebuilt file with `modern/Incubator_Monitor.html` using `cmp`, `sha256sum` or a binary diff. The script checks exact equality and exits with an error if the rebuilt file differs.

This reproducibility check only validates the packaging process, **not** a hardware deployment. The proprietary reply checksum and the physical alarm system still require manufacturer/hardware testing.

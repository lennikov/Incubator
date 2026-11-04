# Incubator Monitor

Incubator monitoring software maintained by Lennikov Lab.

This private repository preserves the **original 2020 C++Builder/VCL application** at the repository root and adds the **2026 browser modernization** under [`modern/`](modern/). The new implementation does not replace the original project or rewrite its Git history.

## Versions

| Codebase | Location | Description |
| --- | --- | --- |
| Original Windows monitor (2020) | Root `*.cpp`, `*.h`, `*.dfm`, `*.cbproj` | Historical Windows C++Builder application |
| Modern browser monitor v1.0.0 | [`modern/Incubator_Monitor.html`](modern/Incubator_Monitor.html) | Standalone HTML/CSS/JavaScript app, usable offline for simulation |

### Try the modern application

Download `modern/Incubator_Monitor.html` from this repository and open it in desktop Chrome. The default demo simulates readings and alarms. For real incubator access, consult the documentation and validate against the hardware's protocol, device panel and local SOPs.

**Not yet field validated:** This snapshot must not be treated as a certified incubator alarm or as the sole safeguard for cell cultures. In-browser monitoring depends on an open browser session and device connectivity.

For project architecture, limitations, and release information, see [modern/README.md](modern/README.md), [modern/ARCHITECTURE.md](modern/ARCHITECTURE.md) and [modern/RELEASE_NOTES.md](modern/RELEASE_NOTES.md).

## Source and attribution

The original source has been retained unmodified. The 2026 standalone is an inspectable source distribution: the CSS and application JavaScript are embedded directly in the HTML. Separate backend/service and test source packages are preserved in the project backup and should be published after a further integrity review.

No open-source license is granted by this README. Check ownership, third-party dependencies and institutional requirements before public distribution.

# Architecture and annotated source map

## Standalone app

`Incubator_Monitor.html` is intentionally self-contained. It embeds UI markup, stylesheet, and the JavaScript application in one document. No cloud endpoint is required for the simulated demo.

| Area | Responsibility | Important caveat |
| --- | --- | --- |
| HTML UI and settings | Incubator status cards, controls, charts, history and alarm messages | Display status is not equivalent to a verified alarm |
| CSS | Responsive dashboard presentation and visual status states | Does not affect alarm thresholds |
| Serial protocol | ASCII read requests and reply parsing for temperature, CO₂ and water | Reply-checksum calculation unverified; compare against real vendor traffic |
| Browser serial | Device port selection, read/write and polling | Requires supported browser and active page |
| Alarm logic | Out-of-range and missing-reading detection, delay and recovery tracking | Hardware disconnection and power-loss must be separately tested |
| Local archive | Local historical samples and CSV export | Browser data is not a managed backup |
| Simulation | Example measurements and synthetic alarm states | Test data is not evidence of physical device behavior |

## Historical implementation

The repository root preserves the 2020 C++Builder/VCL implementation:
- `WorkThread.cpp` / `.h`: worker polling and monitoring logic.
- `comport.cpp` / `.h`: Windows serial-port communication.
- `SendEmail.cpp` / `.h`: historical email-alarm delivery.
- `main.cpp` / `.dfm` / `.h`: main UI.
- `frame.cpp` / `.dfm` / `.h`: incubator widgets.
- `chart.cpp` / `.dfm` / `.h`: historical chart views.
- `incubator.cbproj`: historical C++Builder project.

The legacy implementation and modern standalone should be validated independently. The 2026 background service and module-level tests are available in the separate backup archive, but are not yet committed here.

## Verification still required

- Verify manufacturer model, serial settings and port-driver behavior.
- Capture and review real temperature, CO₂ and water replies.
- Check alarm delays and recovery behavior with disconnected and malformed sensor readings.
- Use independent monitoring safeguards during testing.

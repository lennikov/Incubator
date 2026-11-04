# v1.0.0 — 2026 browser snapshot

**Build:** `Incubator_Monitor.html` (standalone, offline-capable HTML/CSS/JavaScript)

**Included:** status dashboard, demonstration data, serial-port monitoring interface, environmental thresholds, alarm states, charts and browser-local history.

**Version status:** Recovered and source-inspectable standalone. The separately packaged modernization source/backend has automated tests, but actual incubator hardware, browser/driver combinations, SMTP notifications and unattended operation have not been field validated. This is **not a production-certified release**.

**Availability:** The standalone HTML is committed in `modern/`. GitHub Release assets and an annotated source/service tree are not yet published to this repository.

**Historical source:** Original 2020 Windows C++Builder application retained at repository root.

**Deployment note:** The standalone starts in simulated demo mode. Do not depend on it as the sole safety alarm.

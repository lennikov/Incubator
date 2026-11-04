# GitHub Pages deployment

The public repository contains a root `index.html` which redirects to the modern browser monitor at `modern/Incubator_Monitor.html`. The `.nojekyll` file disables Jekyll processing.

To activate GitHub Pages:

1. Open https://github.com/lennikov/Incubator/settings/pages.
2. Under Build and deployment, select Source **Deploy from a branch**.
3. Select branch **master**, folder **/(root)**, and click **Save**.

Then visit https://lennikov.github.io/Incubator/ (direct app: https://lennikov.github.io/Incubator/modern/Incubator_Monitor.html).

## Operational constraints

The GitHub Pages deployment is **static** and serves only HTML/CSS/JavaScript. It supports the **simulated demo** and, when supported by Chrome and an appropriate serial connection, **direct Web Serial monitoring** with the browser tab open. It **does not run the Python background service and cannot send SMTP/email alarms**. A static GitHub Pages URL cannot provide a durable unattended background polling service.

To monitor Thermo Scientific Heracell VIOS incubators continuously, deploy the local background service on the host computer connected to the instrument, configure email notifications there, and test USB/serial readings and alarm thresholds with actual hardware. Always retain independent incubator safeguards. Do not confuse simulated demonstration values with real instrument data.

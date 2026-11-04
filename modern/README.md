# Incubator Monitor — modern browser interface (v1.0.0)

**Status:** archived standalone build; software tests completed in development, real-hardware validation pending.

## Purpose

Read-only environmental monitoring dashboard for incubator temperature, CO₂ and water status. It is designed to display measurements, monitor limits, and provide exportable local history. It does **not** control incubator settings.

## Current standalone build

- Open [`Incubator_Monitor.html`](Incubator_Monitor.html) in desktop Chrome.
- The startup **demo mode uses simulated readings** (including an alarm example), not a physical incubator.
- Browser serial mode uses the Web Serial API and requires compatible hardware, appropriate browser permissions, and an open tab.
- For unattended operation, the separately packaged Python background-service implementation is preferable; the complete service and tests are in the project backup, but are **not part of this GitHub commit**. Do not confuse the standalone HTML with an installed service.

## Protocol assumptions from the legacy source

The monitored channels use read-only ASCII requests ending in carriage return:

| Channel | Request |
| --- | --- |
| Temperature | `?:2010:00::a6\r` |
| CO₂ | `?:2020:00::c0\r` |
| Water | `?:204a:00::cd\r` |

The expected device reply structure is `!:address:hex_length:payload:two_hex_digits`. The software validates structure and numeric fields but does **not** independently verify the proprietary reply-checksum algorithm. Never interpret mock replies in tests as proof of hardware compatibility. Validate field ordering, scaling, units, water thresholds and checksum against your specific device documentation.

## Source layout

This HTML is a single-file source+build. Its `<style>` block contains CSS and its `<script type="module">` block contains JavaScript including serial transport, state and alarms, charts, local archive and UI handlers. Open the source in an editor to inspect or modify it. See [ARCHITECTURE.md](ARCHITECTURE.md) for annotation and safety boundaries.

## Production limitations

1. Browser-only monitoring stops when the tab closes, sleeps, is discarded, or its device disconnects.
2. Real-world serial responses, thresholds and alert scenarios have not been verified against hardware.
3. Never rely on this standalone build as the sole alarm for valuable cell cultures.
4. Validate temperature, CO₂, water-level and disconnected-device scenarios end-to-end before deployment.

The repository's older C++Builder source is retained at the root for comparison. The source code is not a medical device or diagnostic tool.

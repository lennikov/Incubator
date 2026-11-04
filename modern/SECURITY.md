# Security and operational safeguards

- The monitor is read-only with respect to incubator controls.
- Do not commit serial-port configuration containing personal information, device telemetry, experimental data, SMTP credentials, or local SQLite archives.
- Use independent alarms and your established incubator SOP until physical hardware validation is complete.
- Device protocol responses and alarm logic must be checked against real hardware.
- The browser-only version cannot guarantee continuous acquisition when Chrome or the computer sleeps or stops.
- The separate optional local Python service should bind to loopback only. Never expose its HTTP port publicly without a dedicated authentication, authorization and network review.
- No license/permission to redistribute third-party components is asserted by these files.

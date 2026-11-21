# Security Policy

## Supported Versions

| Version | Supported | Notes |
|---|---|---|
| `0.1.x` | Yes | Active development baseline (`noxc 2.2.38`, pure NOX pixel core) |
| `< 0.1.0` | No | Discovery and pre-core prototypes |

## Security Invariants

Prism treats all image inputs, headers, dimensions, color profiles, and metadata blocks as **untrusted and potentially hostile**:

1. **Checked Arithmetic Before Allocation:** Every `width * height * channels * bytes_per_channel`, row `stride`, and byte offset computation uses overflow-checked 64-bit integer arithmetic (`core/limits.nox`). Signed integer wrap-around is never relied upon.
2. **Resource Budgets (`ResourceLimits`):** Explicit defaults bound maximum dimensions (`32,768 x 32,768`), total pixel count (`100,000,000`), decoded byte size (`512 MiB`), raw input size (`256 MiB`), frame count (`200`), metadata size (`4 MiB`), and chunk size (`64 MiB`).
3. **Path Confinement:** Untrusted file paths must be confined via `nox.path.safe_join`.
4. **No Implicit Network or Code Execution:** Decoders and pipelines never follow external URLs, load arbitrary dynamic libraries, or evaluate code strings.
5. **Atomic / No-Clobber Output:** Failed encoding or saving operations do not corrupt existing destination files.

## Reporting a Vulnerability

Please report suspected vulnerabilities privately via GitHub Security Advisories on the repository rather than public issues. Include:

- The exact `noxc` compiler version (`noxc version`) and backend (`llvm` or `qbe`).
- The target OS and architecture.
- A minimal `.nox` reproduction script or hex/base64 input fixture triggering the issue.
- The expected vs. observed error or memory behavior.

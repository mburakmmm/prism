# Contributing to Prism

Prism follows the binding engineering contract defined in [AGENTS.md](AGENTS.md) and tracks phase gates in [todo.md](todo.md).

## Mandatory Pre-Submission Checks

1. Format all modified `.nox` files:
   ```bash
   noxc fmt <file.nox>
   ```
2. Type-check the facade and strict spawn sharing rules:
   ```bash
   noxc check prism.nox
   noxc check --strict-spawn-shared prism.nox
   ```
3. Run the unit and codec test suites on both backends:
   ```bash
   noxc test
   noxc test --backend qbe
   ```
4. Verify freestanding core compatibility:
   ```bash
   noxc check --profile freestanding core/errors.nox
   noxc check --profile freestanding core/limits.nox
   noxc check --profile freestanding core/pixel.nox
   ```
5. Every new feature requires positive tests, negative/boundary tests, checked arithmetic for buffer offsets, and updated documentation in `docs/capabilities.md`, `docs/roadmap.md`, and `CHANGELOG.md`.

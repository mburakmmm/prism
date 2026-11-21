## Objective
- Phase / Feature (`AGENTS.md` §25):

## Current-State Audit
- Existing modules inspected:
- Tested `noxc` version (`noxc version`):

## Design & Security Impact
- Public API changes:
- Memory, `ResourceLimits`, checked arithmetic, color/alpha, codec, and metadata implications:
- Target platform & freestanding fallback:

## Implementation
- Modified / added modules:
- Rejected alternatives:

## Verification
- [ ] `noxc fmt` applied to all `.nox` files
- [ ] `noxc check prism.nox` & `noxc check --strict-spawn-shared prism.nox`
- [ ] `noxc check --profile freestanding` on pure core/transform modules
- [ ] `noxc test` (LLVM default backend)
- [ ] `noxc test --backend qbe` (QBE backend)
- [ ] Codec golden / negative / fuzz tests updated

## Compatibility & Docs
- Breaking changes:
- Updated `docs/capabilities.md`, `docs/roadmap.md`, `CHANGELOG.md`, and `todo.md`:

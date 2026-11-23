# NOX 2.2.38 Language, Compiler & Stdlib Limitations (Prism Field Report)

> **Compiler Baseline:** `noxc 2.2.38` (`/Users/melihburakmemis/.nox-lang/bin/noxc`)  
> **Related Compatibility Report:** [`docs/nox-compatibility.md`](nox-compatibility.md)  
> **Scope:** Empirically observed compiler, stdlib, and toolchain gaps encountered while implementing Prism (`v0.0.1`–`v0.8.5`), paired with each workaround applied in Prism.

---

## 1. Module Visibility & Export Restrictions (`__all__`)

- **Observed Limitation:** Modules declaring `__all__: list[str] = [...]` fail compilation with `UndefinedFunction` / visibility errors when `__all__` includes:
  1. Module-level `int` or `str` constants (e.g., `PRISM_VERSION`, `MAX_KERNEL_SIZE`), or
  2. Symbols re-exported via `from module import ...` (e.g., a top-level package facade like `prism.nox`).
- **Compiler Impact:** Any module that both restricts exports via `__all__` and exposes configuration constants (`MAX_KERNEL_SIZE`, `MIN_FRAME_DELAY_MS`) or re-exports submodules triggers compile-time rejection.
- **Prism Workaround:** Submodules exporting only classes/functions declare `__all__`; modules exporting scalar constants (`filters/convolution.nox`, `filters/gaussian.nox`, `filters/median.nox`, `filters/morphology.nox`, `animation/frame.nox`) and the top-level facade (`prism.nox`) omit `__all__` so all symbols resolve cleanly across module boundaries.

## 2. Syntax & Grammar Gaps

- **No Trailing Commas in Signatures or Calls:** Trailing commas are accepted inside `list` literals (`[1, 2,]`) and multi-line `from ... import (...)` blocks, but cause `UnexpectedToken` parse errors in `def` parameter lists and function call argument lists. Always format with `noxc fmt` and avoid trailing commas in calls/signatures.
- **Top-Level Imports Only:** `import` and `from ... import ...` statements inside functions or blocks are rejected by the parser; all imports must reside at module top level.
- **No Wildcard `_ = expr` Discard:** `_` is treated as an ordinary untyped identifier in assignments (`_ = fn()` fails without a type annotation). Discarded `void` or ignored return calls must either be invoked directly as expression statements or assigned to an explicitly typed variable.

## 3. String Literals & Binary Data

- **Raw Binary Escapes `\x80`–`\xFF` Rejected in String Literals:** String literals are validated as UTF-8 at parse time and reject escapes `\x80` through `\xFF` (`InvalidEscape` / UTF-8 decode error).
- **Prism Workaround:** Binary magic headers and raw byte fixtures (PNG signature `0x89`, JPEG `0xFF 0xD8`, compressed streams) are constructed via `nox.bytes.from_list([...])`, `nox.bytes.str_from_list([...])`, or `Buffer` / `BinaryWriter`.

## 4. Name Resolution & Cross-Module Signatures

- **Module Name Shadowing (`NXL-003`):** Naming a local variable after any imported module alias (`pixel`, `image`, `limits`, `errors`, `color`, `metadata`, `options`, `storage`, `frame`, `ppm`, `pgm`, `bmp`, `png`, `gif`) causes variable/module resolution conflicts. Prism enforces short local variable names (`px`, `img`, `lim`, `err`, `clr`, `meta`, `opt`, `store`, `frm`).
- **Fluent Method Chaining (`obj.m1().m2()`):** Calling a method directly on the return expression of another call (`PipelineGraph.empty().crop(...)` or `g.flip_horizontal().crop(...)`) can trigger a spurious type error (`call the static/class method '...' through the class name ...: the receiver expression has side effects and would not be evaluated`) when cross-module method/function resolution collides. Bind each intermediate return value to a local variable (`g = g.flip_horizontal()`, `g = g.crop(...)`) rather than chaining across multiple calls in one expression.
- **Complex Class Nullable Fields & Optional Arguments (`NXL-001`, `NXL-005`):** Cross-module optional parameter dropping and complex nullable class fields (`int | None`) are fragile across module boundaries. Prism uses explicit factory constructors (`DecodeOptions.default()`, `ResizeOptions.fit_contain(...)`, `ResourceLimits.default()`) and explicit `has_*` boolean fields (`has_icc_profile`, `has_gps_data`).

## 5. Numeric, Arithmetic & Test Stdlib Gaps

- **`int(float)` Truncation Toward Zero:** Casting `float` to `int` truncates toward zero (`int(-0.3) == 0`), requiring explicit `nox.mathx.floor(x + 0.5)` before `int(...)` casting anywhere negative convolution/filter/resampling intermediates occur.
- **64-bit Signed `int` Wrap-Around:** Plain integer `+`, `-`, `*` wrap silently on 64-bit two's-complement overflow. All image dimension, stride, chunk, and frame work budget arithmetic must go through Prism's `checked_add`, `checked_sub`, and `checked_mul` (`core/limits.nox`).
- **`nox.test` Missing `assert_false`:** `nox.test` exposes `assert_true`, `assert_eq_int`, `assert_eq_str`, etc., but has no `assert_false` helper (`UndefinedFunction: module 'nox.test' has no member named 'assert_false'`). Use `nox.test.assert_true(not cond, msg)`.

## 6. Freestanding Profile & Target Limitations

- **`nox.math` vs. `nox.mathx` Split:** `nox.math` depends on hosted libc math symbols and fails under `noxc check --profile freestanding`. All portable pure-Nox pixel/transform/filter/composite/draw/animation modules must import `nox.mathx` instead of `nox.math`.
- **No Built-In Compression or Image Codecs in Stdlib:** The NOX standard library currently has no built-in `zlib`/`deflate`, `lzw`, `crc32`, `adler32`, or `nox.image` codec package. Prism implements pure-Nox RFC 1950/1951 Zlib/DEFLATE, IEEE 802.3 CRC-32, RFC 1950 Adler-32, Netpbm (`P5`/`P6`), uncompressed 24/32-bit BMP, 8-bit PNG, and GIF87a/GIF89a LZW.
- **Native Dynamic Loading & `wasm32-*` Targets:** `extern def` cannot accept NOX classes, `list`, or `Optional` directly across C ABI boundaries, `ptr_of` pointers are invalidated by list growth, and native shared libraries cannot be dynamically loaded on `wasm32-*` or `--profile freestanding` targets (see [`docs/native-adapters.md`](native-adapters.md)).

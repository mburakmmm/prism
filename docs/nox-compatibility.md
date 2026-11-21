# NOX 2.2.38 Compatibility Report for Prism

> **Audit Date:** 2026-11-21  
> **Compiler Binary:** `/Users/melihburakmemis/.nox-lang/bin/noxc` (`noxc 2.2.38`)  
> **Host Environment:** macOS arm64  
> **Probe Program:** `examples/capability_probe.nox`

## 1. Verified Compiler and Standard Library Capabilities

All capabilities below were compiled and executed on both `--backend llvm` and `--backend qbe`:

1. **Large Fixed-Size Buffers (`nox.buffer.Buffer`, `Span`):**
   - Verified 16 MiB (`16,777,216` bytes) and 64 MiB (`67,108,864` bytes) allocation, byte read/write, `Span` window mutation propagation, and deterministic `IndexError` on out-of-bounds access.
2. **Binary `str` ↔ `Buffer` ↔ `list[int]` Round-Trip (`nox.bytes`, `nox.strings`):**
   - Verified that `nox.bytes.str_from_list`, `nox.bytes.from_str`, and `nox.bytes.to_str` preserve embedded `\x00` (NUL) bytes and arbitrary non-UTF-8 bytes (`0x89, 0xFF, 0xD8, 0xFE`).
   - Verified that `nox.strings.byte_len(s)` accurately reports raw byte length.
3. **Structured Binary Endian Readers/Writers (`nox.binary`):**
   - Verified `BinaryReader`, `BinaryWriter`, and `span_reader` for `u8`, `u16_be`, `u32_le`, and `u64_be`.
4. **64-bit Signed Integer Wrap-Around Protection:**
   - Plain `int` arithmetic in NOX wraps on 64-bit signed two's-complement overflow. Prism's `checked_add`, `checked_sub`, and `checked_mul` in `core/limits.nox` detect overflow prior to multiplication/addition and raise `ResourceLimitError`.
5. **Chunked Filesystem I/O and Path Confinement (`nox.fs`, `nox.path`):**
   - Verified `nox.fs.open_write`, `nox.fs.open_read`, `reader.read_chunk(n)`, `nox.fs.write_string_atomic`, and `nox.path.safe_join` blocking `../../etc/passwd` with `PathError`.
6. **Structured Concurrency and `--strict-spawn-shared`:**
   - Verified `async def` + `spawn` + `Task[T]` + `await` across 4 parallel tile checksum workers on both LLVM and QBE, passing `noxc check --strict-spawn-shared`.
7. **Zero-Copy List Pointer and C FFI (`lowlevel`, `ptr_of`, `extern def`):**
   - Verified `ptr_of(packed_bytes)` on `list[u8]` with C `memset` in a `lowlevel` block on both LLVM and QBE.
8. **Freestanding Profile (`--profile freestanding`):**
   - Verified that `core/errors.nox`, `core/limits.nox`, `core/pixel.nox`, `core/color.nox`, `core/metadata.nox`, `core/options.nox`, `core/storage.nox`, and `core/image.nox` pass `noxc check --profile freestanding` with zero OS dependencies.

## 2. Language Syntax & Semantic Rules Observed in `noxc 2.2.38`

To prevent regressions across all Prism phases, agents must adhere to these verified `noxc 2.2.38` rules:

1. **No Trailing Commas in Function Signatures or Call Arguments:**
   - Trailing commas are valid in `list` literals (`__all__: list[str] = ["a", "b",]`), but cause a parse error (`UnexpectedToken`) in `def` parameter lists or function call argument lists. Always run `noxc fmt` and `noxc check`.
2. **String Literal Hex Escapes `\x80`–`\xFF`:**
   - NOX string literals reject `\x80` through `\xFF` escapes at parse time because source string literals represent valid UTF-8 text. Construct binary byte sequences containing values `128..255` via `nox.bytes.str_from_list([...])` or `Buffer`.
3. **Top-Level Static Imports Only:**
   - All `import` and `from ... import ...` statements must appear at module top level; function-local imports are rejected.
4. **`__all__` Applies to Locally Defined Functions and Classes:**
   - A module declaring `__all__` must list only functions and classes defined in that module itself (not module-level constants or re-exported symbols from other modules). Therefore, submodules (`core/*.nox`, `codecs/*.nox`) declare `__all__`, while the pure re-export facade `prism.nox` omits `__all__` so all re-exported classes, functions, and `PRISM_VERSION` are accessible via `from prism import ...`.
5. **Avoid Module Alias Shadowing (`NXL-003`):**
   - Do not name local variables after imported module names (`pixel`, `image`, `limits`, `errors`, `color`, `metadata`, `options`, `storage`, `ppm`, `pgm`). Use `px`, `img`, `lim`, `err`, `clr`, `meta`, `opt`, `store`.
6. **Avoid `_ = expr` Without Type Annotation:**
   - `_` is treated as an ordinary identifier on assignment; either call void/discarded functions directly as expression statements or annotate `unused: int = ...`.

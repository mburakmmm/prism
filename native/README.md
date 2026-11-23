# Prism Optional Native Codec Boundary (`native/`)

> **Default Build Mode:** Pure-NOX (`"source": "nox"` in `nox.json`)  
> **ABI Header:** [`include/prism_native_abi.h`](include/prism_native_abi.h) (`PRISM_NATIVE_ABI_VERSION = 1`)  
> **Platform & License Matrix:** [`../docs/native-adapters.md`](../docs/native-adapters.md)

## Design Contract

Prism keeps its core (`core/*`, `transforms/*`, `filters/*`, `composite/*`, `draw/*`, and portable `PPM`/`PGM`/`BMP`/`PNG` codecs) **100% pure Nox** with zero mandatory C/C++ runtime dependencies.

Optional native codec backends (JPEG via `libjpeg-turbo`, WebP via `libwebp`, TIFF via `libtiff`) are strictly isolated behind the C ABI defined in [`include/prism_native_abi.h`](include/prism_native_abi.h).

### Guarantees

1. **No Core Contamination:** In standard `"source": "nox"` builds, `codecs/jpeg_adapter.nox`, `codecs/webp_adapter.nox`, and `codecs/tiff_adapter.nox` compile cleanly on all targets (including `--profile freestanding`) without linking external C libraries.
2. **Pre-Flight Pure-NOX Probing:** `probe_jpeg`, `probe_webp`, and `probe_tiff` parse container headers and enforce `ResourceLimits` (`max_input_bytes`, `max_width`, `max_height`, `max_pixels`, `max_decoded_bytes`, `max_metadata_bytes`) in pure Nox *before* any native decode would ever execute.
3. **Deterministic Fallback:** When native adapters are disabled, decode/encode calls still validate options and `ResourceLimits` first, then raise typed `CodecUnavailableError` (or `CorruptDataError` / `ResourceLimitError` / `UnsupportedFormatError` for invalid inputs).
4. **C ABI Memory Ownership & Error Isolation:**
   - All native buffers return a paired `free_fn` pointer and are freed exclusively via `prism_native_free_buffer(&buf)` so caller/callee allocators never mismatch across shared library boundaries.
   - Native errors never cross ABI boundaries as C++ exceptions or `longjmp`; every status uses `PrismNativeStatus` and a fixed 256-byte `PrismNativeError.message` buffer.
   - NOX classes, `list`, and `Optional` values are never passed directly across `extern def`.

## Building Opt-In Native Shims (Future Hosted Extensions)

When linking an opt-in native shim on hosted desktop/server platforms:

1. Compile against `native/include/prism_native_abi.h`.
2. Wrap C entrypoints in a thin shared library (`libprism_native`) linked against audited `libjpeg-turbo` / `libwebp` / `libtiff`.
3. Never enable native dynamic loading on `wasm32-*` or `--profile freestanding` targets.

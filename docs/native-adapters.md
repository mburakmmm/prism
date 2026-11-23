# Prism Native Codec Adapters & ABI Policy

> **Contract Reference:** [`AGENTS.md`](../AGENTS.md) §12 & Phase F8  
> **C ABI Header:** [`native/include/prism_native_abi.h`](../native/include/prism_native_abi.h)  
> **Adapter Readme:** [`native/README.md`](../native/README.md)

## 1. Architecture: Pure-NOX First, Native Opt-In Only

Prism refuses to make external C libraries a mandatory dependency of the core framework:

- **Default `"source": "nox"` distribution:** Ships with pure-Nox magic-byte sniffing, pure-Nox header probing (`probe_jpeg`, `probe_webp`, `probe_tiff`), `EncodeOptions` validation (`quality`, `lossless`, `JpegSubsampling`), and typed `CodecUnavailableError` signaling when native decode/encode backends are not linked.
- **Zero-crash hostile input validation:** Corrupt headers, truncated segments, invalid `VP8`/`VP8L`/`VP8X` FourCCs, cyclic TIFF IFDs, and oversized image dimensions are rejected deterministically with `CorruptDataError`, `UnsupportedPixelFormatError`, or `ResourceLimitError` prior to dispatching to any native decoder.

## 2. Third-Party Library Matrix (Audited Licenses & CVE Hygiene)

| Format | Upstream Library | License | Copyleft? | Primary Security Risks Mitigated |
|---|---|---|---|---|
| **JPEG** | `libjpeg-turbo` (`turbojpeg` / `libjpeg`) | IJG + Modified BSD-3-Clause + zlib | No | Progressive/arith-coded decompression bombs (`max_pixels`, `max_decoded_bytes`), EXIF buffer overflows (pre-stripped/bounded in pure Nox) |
| **WebP** | `libwebp` | BSD-3-Clause | No | Huffman table overreads (e.g., CVE-2023-4863 class) — pin patched `>= 1.3.2`+, pre-check `VP8`/`VP8L`/`VP8X` canvas bounds |
| **TIFF** | `libtiff` | BSD-like (libtiff license) | No | IFD pointer loops, strip/tile integer overflows — bounded pre-flight TIFF IFD0 validation (`max_recursion_depth`) |

## 3. ABI Isolation Rules

1. **Versioned Structs:** Every host/plugin handshake checks `prism_native_abi_version() == PRISM_NATIVE_ABI_VERSION` (`1`).
2. **Flat Scalar & Raw Span Parameters:** Functions accept only `const uint8_t *`, `size_t`, scalar `uint32_t`/`uint8_t`, and plain C POD structs (`PrismNativeLimits`, `PrismNativeBuffer`, `PrismNativeError`).
3. **Matched Deallocation:** Native-allocated pixel/bitstream buffers carry `void (*free_fn)(uint8_t *)` and must be released with `prism_native_free_buffer` inside deterministic `finally`/`defer` cleanup blocks.
4. **Budget Forwarding:** `ResourceLimits` (`max_width`, `max_height`, `max_pixels`, `max_decoded_bytes`, `max_output_bytes`, `max_metadata_bytes`, `max_work_ms`) are translated into `PrismNativeLimits` on every call.

## 4. Platform Distribution Matrix

| Target | Pure-NOX Probe (`JPEG`/`WebP`/`TIFF`) | Opt-In Native Decode/Encode | Status |
|---|---|---|---|
| **macOS (arm64 / x86_64)** | Supported (`--profile freestanding` compatible) | Opt-in C ABI (`libprism_native.dylib`) | Probe verified; ABI defined |
| **Linux (x86_64 / aarch64)** | Supported | Opt-in C ABI (`libprism_native.so`) | Probe in CI; ABI defined |
| **Windows (x86_64)** | Supported | Opt-in C ABI (`prism_native.dll`) | Probe in CI; ABI defined |
| **WASM (`wasm32-*`) & Freestanding** | Supported | Unsupported (`CodecUnavailableError` by design; dynamic libraries unavailable) | Enforced by contract |

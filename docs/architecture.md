# Prism Architecture & ADR-001

## 1. Architectural Overview

Prism is a native image processing, conversion, and I/O framework for the NOX ecosystem. Its architecture strictly separates the pure-NOX pixel/math core from file/network adapters and optional native codec backends.

```text
Application / Nyx / Aether / Aura / Ember / CLI
                         │
                     prism.nox
                         │
          Image / Pixel / Color / Options / Limits
            │                 │
    Codec I/O Adapters   Operation APIs
    sniff/decoder        geometry/filter/draw/composite
            │                 │
            └───────┬─────────┘
                    │
             Pipeline Executor
         eager / tile / stream / parallel
                    │
           Buffer + typed storage
                    │
        optional Native Codec Backend
```

## 2. Module Responsibilities (`v0.1.0` Baseline)

- `core/errors.nox`: Typed exception hierarchy rooted at `PrismError`.
- `core/limits.nox`: 64-bit overflow-checked integer arithmetic (`checked_add`, `checked_sub`, `checked_mul`, `checked_stride`, `checked_buffer_size`) and `ResourceLimits` security budgets.
- `core/pixel.nox`: `PixelFormat`, `ColorSpace`, `AlphaMode` enums and 8-bit `Pixel` value type.
- `core/color.nox`: sRGB ↔ LinearSRGB conversion, straight ↔ premultiplied alpha conversion, and Porter-Duff `source-over` blending.
- `core/metadata.nox`: `ImageMetadata` and `ImageInfo` models with EXIF display orientation validation (`1..8`).
- `core/options.nox`: Typed `DecodeOptions` and `EncodeOptions`.
- `core/storage.nox`: `PixelStorage` managing row-major `nox.buffer.Buffer` memory, checked stride, and pixel read/write.
- `core/image.nox`: Immutable `Image`, controlled `MutableImage`, and zero-copy `ImageView` keeping parent storage alive.
- `codecs/ppm.nox` & `codecs/pgm.nox`: Pure-NOX Netpbm P6 (RGB8) and P5 (GRAY8) header prober, decoder, encoder, and atomic/no-clobber file helpers.
- `prism.nox`: Stable public facade re-exporting verified modules.

**Dependency Rule:** `core/*` has zero dependency on `nox.fs`, `nox.http`, or any application framework (`nyx`, `aether`, `ember`, `aura`). All `core/*` modules compile under `--profile freestanding`.

---

## 3. ADR-001: Pure-NOX Core and Isolated Optional Native Codec Strategy

- **Date:** 2026-11-21
- **Status:** Accepted
- **Compiler Baseline:** `noxc 2.2.38`

### Context

Image processing libraries face a tension between portability/safety (pure managed code running identically on macOS, Linux, Windows, WebAssembly, and freestanding environments) and codec coverage for complex formats (JPEG, WebP, TIFF). Additionally, `noxc publish` enforces package `"source"` labels (`"nox"` vs `"c"` / `"mixed"`).

### Decision

1. **Pure-NOX Core (`"source": "nox"` default):**
   - The entire pixel model (`Image`, `MutableImage`, `ImageView`), checked arithmetic, `ResourceLimits`, geometry/resampling (`F3`), filters (`F6`), drawing/compositing (`F7`), pipeline/tile engine (`F10`), and portable codecs (PPM P6, PGM P5, uncompressed BMP 24/32-bit in `F2`, 8-bit PNG in `F4`, GIF in `F9`) are implemented in pure `.nox`.
   - Every module in `core/*` must pass `noxc check --profile freestanding`.
2. **No Implicit Equivalence Between `nox.gzip` and Zlib:**
   - `nox.gzip` implements RFC 1952 (gzip container), whereas PNG `IDAT` uses RFC 1950 (zlib container wrapping RFC 1951 DEFLATE with Adler-32 checksum). Phase F4 will implement a dedicated, bounded RFC 1950/1951 stream decoder/encoder in pure NOX rather than misusing gzip headers.
3. **Optional Isolated Native Codec Boundary (`F8`):**
   - Lossy/complex codecs (`libjpeg-turbo`, `libwebp`, `libtiff`) are never mandatory core dependencies.
   - When native adapters are not linked, calling a native-only codec raises a typed `CodecUnavailableError`.
   - Any C ABI boundary in `native/` must validate dimensions against `ResourceLimits` *before* allocation, pair native allocations with their own deallocator, and never allow raw C pointers to escape into `Image`.

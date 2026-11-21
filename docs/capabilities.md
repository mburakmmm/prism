# Prism Capabilities Matrix

> **Verified with:** `noxc 2.2.38` (`/Users/melihburakmemis/.nox-lang/bin/noxc`)  
> **Host verified:** macOS arm64 (Apple Silicon) — LLVM and QBE backends  
> **Date:** 2026-11-21  
> **Active Release:** `v0.5.0` (Phases F0–F5 MVP Milestone Complete)

## 1. Core Pixel, I/O, Geometry, PNG & Color/Metadata Capabilities (`v0.5.0`)

| Capability | Status | Module | Backends Verified | Notes |
|---|---|---|---|---|
| Typed Error Hierarchy (`PrismError` + 13 subclasses) | **VERIFIED** | `core/errors.nox` | LLVM, QBE, Freestanding | Catchable by specific class or `PrismError` base |
| 64-bit Checked Arithmetic (`checked_add/sub/mul/stride/buffer_size`) | **VERIFIED** | `core/limits.nox` | LLVM, QBE, Freestanding | Prevents signed `int` wrap-around before allocation |
| Configurable `ResourceLimits` (`default`, `strict_server`, `constrained_wasm`) | **VERIFIED** | `core/limits.nox` | LLVM, QBE, Freestanding | Enforces width, height, pixels, decoded bytes, input/output/metadata budgets |
| `PixelFormat` (`GRAY8`, `RGB8`, `RGBA8`) | **VERIFIED** | `core/pixel.nox` | LLVM, QBE, Freestanding | `GRAY16`, `RGB16`, `RGBA16`, `RGBAF32`, `INDEXED8` typed in enum; gated until F9/F12 |
| `Image` (Immutable), `MutableImage`, `ImageView` (Zero-copy view) | **VERIFIED** | `core/image.nox` | LLVM, QBE, Freestanding | Snapshot isolation and parent storage lifetime preservation verified |
| sRGB ↔ LinearSRGB, Straight ↔ Premultiplied Alpha & Linear Blending | **VERIFIED** | `core/color.nox` | LLVM, QBE, Freestanding | `convert_color_space`, `image_to_premultiplied`, `image_to_straight`, `blend_images_source_over` + ICC capability gating (`ColorProfileError`) |
| Bounded EXIF Parser, Cyclic-IFD Guard, GPS/Serial Strip & ICC Carry | **VERIFIED** | `core/metadata.nox` | LLVM, QBE, Freestanding | `parse_exif_buffer`, `sanitize_exif_buffer`, `build_minimal_exif_orientation` |
| Extension-Independent Magic-Byte Sniffing (`sniff_format`) | **VERIFIED** | `codecs/sniff.nox` | LLVM, QBE, Freestanding | Detects PPM, PGM, BMP, PNG, JPEG, WebP, GIF, TIFF magic bytes |
| Safe File & Memory I/O Adapters (`open_image`, `save_image`, `safe_join`) | **VERIFIED** | `adapters/file.nox`, `adapters/memory.nox` | LLVM, QBE | Atomic write, no-clobber default, and confined path traversal blocking |
| Crop & Border Padding (`STRICT`, `CLIP`, `PAD`; `CONSTANT`, `CLAMP`, `MIRROR`, `WRAP`) | **VERIFIED** | `transforms/crop.nox`, `transforms/border.nox` | LLVM, QBE, Freestanding | Exact sub-region extraction and 4-mode border extension |
| Flip, Transpose, Rotate (`90`/`180`/`270`/arbitrary) & EXIF Orientation 1–8 | **VERIFIED** | `transforms/flip.nox`, `transforms/rotate.nox` | LLVM, QBE, Freestanding | Exact 90-degree fast-paths, orientation tag reset, and expanded arbitrary rotation |
| Resampling (`NEAREST`, `BILINEAR`, `BICUBIC`, `LANCZOS2`, `LANCZOS3`, `AREA`) & Fit Modes (`FILL`, `CONTAIN`, `COVER`, `INSIDE`, `OUTSIDE`, `thumbnail`) | **VERIFIED** | `transforms/resample.nox`, `transforms/resize.nox` | LLVM, QBE, Freestanding | `x+0.5` pixel-center convention, alpha-halo prevention, and optional linear-light mode |
| 2D Affine Transform (`AffineMatrix2D`, `affine_transform`) | **VERIFIED** | `transforms/affine.nox` | LLVM, QBE, Freestanding | Invertible 2x3 matrix with NaN/Inf and singularity rejection |
| Pure-NOX Zlib (RFC 1950), DEFLATE (RFC 1951), CRC-32 & Adler-32 | **VERIFIED** | `codecs/deflate.nox` | LLVM, QBE, Freestanding | Stored (BTYPE 0), Fixed Huffman (BTYPE 1), Dynamic Huffman (BTYPE 2) + LZ77 compressor |
| Pure-NOX 8-bit PNG Engine (`probe_png`, `decode_png`, `encode_png`) | **VERIFIED** | `codecs/png.nox` | LLVM, QBE | Color Types 0, 2, 3 (`PLTE`+`tRNS`), 4, 6; Scanline Filters 0–4; `eXIf`/`iCCP`/`tEXt` hygiene & autorotate |

## 2. Codec Support Matrix (`v0.5.0`)

| Format | Probe | Decode | Encode | Implementation | Phase / Status |
|---|---|---|---|---|---|
| **PPM (P6 binary RGB8)** | **VERIFIED** | **VERIFIED** | **VERIFIED** | Pure NOX (`codecs/ppm.nox`) | F1–F2 / `v0.5.0` |
| **PGM (P5 binary GRAY8)** | **VERIFIED** | **VERIFIED** | **VERIFIED** | Pure NOX (`codecs/pgm.nox`) | F1–F2 / `v0.5.0` |
| **BMP (24/32-bit uncompressed BI_RGB)** | **VERIFIED** | **VERIFIED** | **VERIFIED** | Pure NOX (`codecs/bmp.nox`) | F2 / `v0.5.0` (bottom-up & top-down, 4-byte padding) |
| **PNG (8-bit Gray/RGB/RGBA/Palette+tRNS)** | **VERIFIED** | **VERIFIED** | **VERIFIED** | Pure NOX (`codecs/png.nox`, `codecs/deflate.nox`) | F4–F5 / `v0.5.0` (includes `eXIf` autorotate, private metadata strip & `iCCP` carry) |
| **JPEG (baseline)** | Sniff only | Planned | Planned | Optional Native Adapter (`codecs/jpeg_adapter.nox`) | F8 / `v0.8.0` (currently raises `CodecUnavailableError`) |
| **WebP (lossy/lossless)** | Sniff only | Planned | Planned | Optional Native Adapter (`codecs/webp_adapter.nox`) | F8 / `v0.8.0` (currently raises `CodecUnavailableError`) |
| **GIF (still/animated)** | Sniff only | Planned | Planned | Pure NOX (`codecs/gif.nox`) | F9 / `v0.8.5` |
| **TIFF (baseline)** | Sniff only | Planned | Planned | Optional Native Adapter (`codecs/tiff_adapter.nox`) | F8–F9 (currently raises `CodecUnavailableError`) |

## 3. Platform & Backend Verification Status

| Target / Profile | Core, Transform, Metadata & DEFLATE Kernels | File I/O (`PPM`/`PGM`/`BMP`/`PNG`) | Concurrency (`spawn`) | Status |
|---|---|---|---|---|
| **macOS arm64 (LLVM)** | **VERIFIED** | **VERIFIED** | **VERIFIED** | 8 test suites passed (`noxc 2.2.38`) |
| **macOS arm64 (QBE)** | **VERIFIED** | **VERIFIED** | **VERIFIED** | 8 test suites passed (`noxc 2.2.38`) |
| **Freestanding (`--profile freestanding`)** | **VERIFIED** (`core/*`, `transforms/*`, `codecs/deflate.nox`, `codecs/sniff.nox`, `codecs/registry.nox`) | N/A (No OS FS) | N/A | 19 pure modules pass freestanding type-check |
| **Linux x64 / arm64** | CI Configured | CI Configured | CI Configured | `NOT VERIFIED` locally (workflow in `.github/workflows/ci.yml`) |
| **Windows x64** | CI Configured | CI Configured | CI Configured | `NOT VERIFIED` locally (workflow in `.github/workflows/ci.yml`) |
| **WASM (`wasm32-wasi` / `wasm32-freestanding`)** | Planned (F14) | WASI/VFS only | Mode-dependent | Planned for Phase F14 (`v0.9.8`) |

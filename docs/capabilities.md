# Prism Capabilities Matrix

> **Verified with:** `noxc 2.2.38` (`/Users/melihburakmemis/.nox-lang/bin/noxc`)  
> **Host verified:** macOS arm64 (Apple Silicon) — LLVM and QBE backends  
> **Date:** 2026-11-23  
> **Active Release:** `v0.9.2` (Phases F0–F11 Complete)

## 1. Core Pixel, I/O, Geometry, PNG, Color/Metadata, Filter, Drawing, GIF, Pipeline & Streaming (`v0.9.2`)

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
| Point Enhancement, Effects, Quality Metrics (MSE/MAE/PSNR/SSIM) & Perceptual Hashes | **VERIFIED** | `filters/enhance.nox`, `filters/effects.nox` | LLVM, QBE, Freestanding | `grayscale`, `invert`, `threshold`, `posterize`, `solarize`, `brightness`, `contrast`, `saturation`, `gamma_correct`, `apply_lut`, `split_channels`, `merge_channels`, `sepia`, `pixelate`, `vignette`, `compute_channel_stats`, `histogram_equalize`, `image_psnr`, `image_ssim`, `average_hash`, `difference_hash` |
| 2D Convolution, Separable Gaussian/Box Blur, Sharpen, Edges, Median & Morphology | **VERIFIED** | `filters/convolution.nox`, `filters/gaussian.nox`, `filters/sharpen.nox`, `filters/edges.nox`, `filters/median.nox`, `filters/morphology.nox` | LLVM, QBE, Freestanding | `convolve`, `gaussian_blur`, `box_blur`, `sharpen`, `unsharp_mask`, `sobel`, `prewitt`, `laplacian`, `emboss`, `median_filter`, `erode`, `dilate`, `morphology_open`, `morphology_close` |
| Porter-Duff Modes, W3C Blend Modes, 8-Bit Masking & Multi-Layer Compositing | **VERIFIED** | `composite/alpha.nox`, `composite/blend.nox`, `composite/masks.nox`, `composite/layers.nox` | LLVM, QBE, Freestanding | `PorterDuffMode` (6 modes), `BlendMode` (7 modes), `apply_mask`, `masked_composite`, `paste_image`, `Layer`, `composite_layers` |
| 2D Canvas, Shapes, Bezier Paths, Anti-Aliased Lines, Iterative Flood Fill & Embedded Bitmap Font | **VERIFIED** | `draw/canvas.nox`, `draw/rasterizer.nox`, `draw/shapes.nox`, `draw/paths.nox`, `draw/text.nox` | LLVM, QBE, Freestanding | `Canvas` clipping, `draw_line_aa`, iterative `flood_fill`, rect/rounded-rect/circle/ellipse/polygon/Bezier, 8x8 font `measure_text` & `draw_text` |
| Pure-NOX Zlib (RFC 1950), DEFLATE (RFC 1951), CRC-32 & Adler-32 | **VERIFIED** | `codecs/deflate.nox` | LLVM, QBE, Freestanding | Stored (BTYPE 0), Fixed Huffman (BTYPE 1), Dynamic Huffman (BTYPE 2) + LZ77 compressor |
| Pure-NOX 8-bit PNG Engine (`probe_png`, `decode_png`, `encode_png`) | **VERIFIED** | `codecs/png.nox` | LLVM, QBE | Color Types 0, 2, 3 (`PLTE`+`tRNS`), 4, 6; Scanline Filters 0–4; `eXIf`/`iCCP`/`tEXt` hygiene & autorotate |
| Pure-NOX Header Probing & Opt-In C ABI Boundary (JPEG, WebP, TIFF) | **VERIFIED** | `codecs/jpeg_adapter.nox`, `codecs/webp_adapter.nox`, `codecs/tiff_adapter.nox`, `native/include/prism_native_abi.h` | LLVM, QBE, Freestanding | Pure-NOX `probe_jpeg`/`probe_webp`/`probe_tiff`, option/limit validation & `CodecUnavailableError` fallback when native backends are disabled |
| Animation Timeline & Pure-NOX GIF87a/GIF89a LZW Engine | **VERIFIED** | `animation/frame.nox`, `animation/timeline.nox`, `animation/gif.nox`, `codecs/gif.nox` | LLVM, QBE, Freestanding | `Frame`, `Animation`, `Timeline.render_frames()`, disposal `0..3`, transparency, `NETSCAPE2.0` loop, LZW encode/decode, `max_frames` & budget checks |
| Pipeline DAG, Algebraic Planner, Halo-Aware Tiling & LRU Cache | **VERIFIED** | `pipeline/nodes.nox`, `pipeline/graph.nox`, `pipeline/planner.nox`, `pipeline/tiles.nox`, `pipeline/cache.nox`, `pipeline/executor.nox` | LLVM, QBE, Freestanding | `PipelineGraph`, `optimize_pipeline`, `execute_tiled_node`, `PlanCache` (`max_bytes` LRU), 1,000 random pipeline equivalence verified |
| Windowed Scanline Streaming, Confined SpillManager & Parallel Spawn Batch/Tiles | **VERIFIED** | `pipeline/streaming.nox`, `pipeline/parallel.nox` | LLVM, QBE (`--strict-spawn-shared`) | `StreamImageReader`/`StreamImageWriter` (`with` protocol, atomic commit), `SpillManager` (`max_temp_bytes`), `execute_tiled_node_parallel`, `BatchProcessor` (`FAIL_FAST`/`COLLECT_ERRORS`) |

## 2. Codec Support Matrix (`v0.8.5`)

| Format | Probe | Decode | Encode | Implementation | Phase / Status |
|---|---|---|---|---|---|
| **PPM (P6 binary RGB8)** | **VERIFIED** | **VERIFIED** | **VERIFIED** | Pure NOX (`codecs/ppm.nox`) | F1–F2 / `v0.8.5` |
| **PGM (P5 binary GRAY8)** | **VERIFIED** | **VERIFIED** | **VERIFIED** | Pure NOX (`codecs/pgm.nox`) | F1–F2 / `v0.8.5` |
| **BMP (24/32-bit uncompressed BI_RGB)** | **VERIFIED** | **VERIFIED** | **VERIFIED** | Pure NOX (`codecs/bmp.nox`) | F2 / `v0.8.5` (bottom-up & top-down, 4-byte padding) |
| **PNG (8-bit Gray/RGB/RGBA/Palette+tRNS)** | **VERIFIED** | **VERIFIED** | **VERIFIED** | Pure NOX (`codecs/png.nox`, `codecs/deflate.nox`) | F4–F5 / `v0.8.5` (includes `eXIf` autorotate, private metadata strip & `iCCP` carry) |
| **GIF (still/animated GIF87a/GIF89a)** | **VERIFIED** | **VERIFIED** | **VERIFIED** | Pure NOX (`codecs/gif.nox`, `animation/timeline.nox`) | F9 / `v0.8.5` (LZW, GCE disposal 0..3, transparency, `NETSCAPE2.0` loop) |
| **JPEG (baseline)** | **VERIFIED** | Opt-in Native ABI | Opt-in Native ABI | Pure NOX probe (`codecs/jpeg_adapter.nox`) + C ABI | F8 / `v0.8.5` (`probe_jpeg` active; decode/encode raise `CodecUnavailableError` without native backend) |
| **WebP (VP8/VP8L/VP8X)** | **VERIFIED** | Opt-in Native ABI | Opt-in Native ABI | Pure NOX probe (`codecs/webp_adapter.nox`) + C ABI | F8 / `v0.8.5` (`probe_webp` active; decode/encode raise `CodecUnavailableError` without native backend) |
| **TIFF (baseline LE/BE)** | **VERIFIED** | Opt-in Native ABI | Opt-in Native ABI | Pure NOX probe (`codecs/tiff_adapter.nox`) + C ABI | F8 / `v0.8.5` (`probe_tiff` active; decode/encode raise `CodecUnavailableError` without native backend) |

## 3. Platform & Backend Verification Status

| Target / Profile | Core, Transform, Metadata & DEFLATE Kernels | File I/O (`PPM`/`PGM`/`BMP`/`PNG`/`GIF`) | Concurrency (`spawn`) | Status |
|---|---|---|---|---|
| **macOS arm64 (LLVM)** | **VERIFIED** | **VERIFIED** | **VERIFIED** | 14 test suites passed (`noxc 2.2.38`) |
| **macOS arm64 (QBE)** | **VERIFIED** | **VERIFIED** | **VERIFIED** | 14 test suites passed (`noxc 2.2.38`) |
| **Freestanding (`--profile freestanding`)** | **VERIFIED** (`core/*`, `transforms/*`, `filters/*`, `composite/*`, `draw/*`, `animation/*`, `pipeline/*`, `codecs/deflate.nox`, `codecs/sniff.nox`, `codecs/registry.nox`, `codecs/*_adapter.nox`, `codecs/gif.nox`) | N/A (No OS FS) | N/A | 49 pure modules pass freestanding type-check |
| **Linux x64 / arm64** | CI Configured | CI Configured | CI Configured | `NOT VERIFIED` locally (workflow in `.github/workflows/ci.yml`) |
| **Windows x64** | CI Configured | CI Configured | CI Configured | `NOT VERIFIED` locally (workflow in `.github/workflows/ci.yml`) |
| **WASM (`wasm32-wasi` / `wasm32-freestanding`)** | Planned (F14) | WASI/VFS only | Mode-dependent | Planned for Phase F14 (`v0.9.8`) |

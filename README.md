# Prism 🔮

**Pure-Nox Native Image Processing, Resampling, Color Science & Portable Codec Engine**

Prism is a zero-dependency, statically typed, resource-bounded image processing and I/O framework crafted natively for the **[Nox programming language](https://noxlang.com)**. Combining the ergonomic API design of Pillow, the server-grade safety and resampling rigor of Sharp/libvips, and the type safety of Rust's `image` crate, Prism delivers 64-bit overflow-checked pixel memory, pure-Nox RFC 1950/1951 Zlib/DEFLATE & 8-bit PNG encoding/decoding, alpha-safe resampling, and privacy-first EXIF metadata sanitization across macOS, Linux, Windows, and freestanding targets.

---

[![CI Matrix](https://github.com/mburakmmm/prism/actions/workflows/ci.yml/badge.svg)](https://github.com/mburakmmm/prism/actions/workflows/ci.yml)
[![Release Gate](https://github.com/mburakmmm/prism/actions/workflows/release.yml/badge.svg)](https://github.com/mburakmmm/prism/actions/workflows/release.yml)
[![Security Gates](https://github.com/mburakmmm/prism/actions/workflows/security.yml/badge.svg)](https://github.com/mburakmmm/prism/actions/workflows/security.yml)
[![Version](https://img.shields.io/badge/version-v0.9.2-orange.svg)](nox.json)
[![Platform Support](https://img.shields.io/badge/platform-macOS%20%7C%20Linux%20%7C%20Windows%20%7C%20Freestanding-blue.svg)](#-cross-platform--compatibility-matrix)
[![Tests](https://img.shields.io/badge/tests-14%20suites%20%7C%2012%2C450%2B%20checks%20%28100%25%29-brightgreen.svg)](tests/)
[![Compiler](https://img.shields.io/badge/noxc-2.2.38-purple.svg)](https://github.com/mburakmmm/nox-lang)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

---

## 📑 Table of Contents

- [Why Prism?](#-why-prism)
- [Architecture Overview](#-architecture-overview)
- [Key Features & Codec Matrix](#-key-features--codec-matrix)
- [Installation](#-installation)
- [Quickstart Guide](#-quickstart-guide)
  - [1. Immutable Image, MutableImage & Zero-Copy ImageView](#1-immutable-image-mutableimage--zero-copy-imageview)
  - [2. Extension-Independent Magic-Byte Probing & Confined I/O](#2-extension-independent-magic-byte-probing--confined-io)
  - [3. Pure-Nox 8-Bit PNG Engine (RFC 2083 & RFC 1950/1951)](#3-pure-nox-8-bit-png-engine-rfc-2083--rfc-19501951)
  - [4. Geometry, Alpha-Safe Resampling & Thumbnails](#4-geometry-alpha-safe-resampling--thumbnails)
  - [5. Linear-Light Porter-Duff Blending & Premultiplied Alpha](#5-linear-light-porter-duff-blending--premultiplied-alpha)
  - [6. EXIF Orientation & Private Metadata Scrubbing (GPS / Serial)](#6-exif-orientation--private-metadata-scrubbing-gps--serial)
- [Cross-Platform & Compatibility Matrix](#-cross-platform--compatibility-matrix)
- [Documentation Index](#-documentation-index)
- [Automated Test, Golden & Fuzz Suite](#-automated-test-golden--fuzz-suite)
- [Roadmap & Release Phases](#-roadmap--release-phases)
- [Contributing, Security & License](#-contributing-security--license)

---

## 💡 Why Prism?

- **100% Pure-Nox Core (`"source": "nox"`)**: Zero mandatory C/C++ dependencies. The pixel core (`core/*`), geometric resampling engine (`transforms/*`), format sniffer (`codecs/sniff.nox`), and RFC 1950/1951 Zlib/DEFLATE engine (`codecs/deflate.nox`) all compile under `--profile freestanding` without an operating system or C standard library.
- **Hostile-Input & Decompression-Bomb Defense**: Every `width * height * channels`, row `stride`, and byte offset computation uses 64-bit overflow-checked integer arithmetic (`checked_add`, `checked_sub`, `checked_mul`) and validates against configurable `ResourceLimits` (`default()`, `strict_server()`, `constrained_wasm()`) *before* allocating a single byte.
- **Zero-Halo Alpha-Safe Resampling**: Prevents dark/dirty RGB halos along transparent edges (`alpha == 0`) by weighting color samples by alpha (`alpha_safe=True`) and offering physically accurate linear-light sRGB Lanczos3 resampling (`ResizeOptions.high_quality()`).
- **Pure-Nox 8-Bit PNG & Portable Codec Suite**: Complete RFC 2083 PNG implementation with IEEE 802.3 CRC-32, RFC 1950 Adler-32, Stored/Fixed/Dynamic Huffman DEFLATE decompression, LZ77+RLE+Fixed-Huffman compression, Scanline Filters `0..4` (`None`, `Sub`, `Up`, `Average`, `Paeth`), Color Types `0`, `2`, `3` (`PLTE`+`tRNS`), `4`, `6`, plus uncompressed 24/32-bit BMP and Netpbm PPM (`P6`) / PGM (`P5`).
- **Privacy-First EXIF Hygiene**: Built-in endian-aware TIFF/EXIF IFD parser (`parse_exif_buffer`, `sanitize_exif_buffer`) detects cyclic-IFD pointer attacks, normalizes EXIF Orientation `1..8` without double-rotating, and strips GPS coordinates (`0x8825`), camera body serial numbers (`0xA431`), and `MakerNote` (`0x927C`) tags at the byte level.

---

## 🏛️ Architecture Overview

```text
                User Application / Nyx / Aether / Ember / Aura / CLI
                                         │
                                  Prism Public API
                                    (prism.nox)
                                         │
       ┌─────────────────────┬───────────┴───────────┬─────────────────────┐
       ▼                     ▼                       ▼                     ▼
  Safe Pixel Core     Portable Codecs & I/O    Transform Engine     Color & EXIF Hygiene
┌─────────────────┐  ┌─────────────────────┐  ┌──────────────────┐  ┌───────────────────┐
│ Image (immut.)  │  │ Magic-byte sniffer  │  │ Crop (Strict/Pad)│  │ sRGB ↔ LinearSRGB │
│ MutableImage    │  │ PNG (8-bit, Zlib)   │  │ Flip / Rotate    │  │ Premultiplied α   │
│ ImageView       │  │ BMP (24/32-bit)     │  │ Nearest/Bilinear │  │ Linear source-over│
│ PixelStorage    │  │ PPM (P6) / PGM (P5) │  │ Bicubic/Lanczos3 │  │ Cyclic-IFD guard  │
│ ResourceLimits  │  │ safe_join & atomic  │  │ Area / Thumbnail │  │ GPS/Serial strip  │
└─────────────────┘  └─────────────────────┘  └──────────────────┘  └───────────────────┘
       │                     │                       │                     │
       └─────────────────────┴───────────┬───────────┴─────────────────────┘
                                         ▼
                     64-Bit Checked Arithmetic & Buffer Layer
               (nox.buffer.Buffer │ nox.binary │ nox.mathx │ nox.fs)
```

---

## ✨ Key Features & Codec Matrix

| Capability | Module | Description |
| :--- | :--- | :--- |
| **Checked 64-Bit Budgets** | [`core/limits.nox`](core/limits.nox) | `checked_add`, `checked_sub`, `checked_mul`, `checked_stride`, `checked_buffer_size`, and `ResourceLimits` (`default`, `strict_server`, `constrained_wasm`). |
| **Typed Exception Model** | [`core/errors.nox`](core/errors.nox) | `PrismError` hierarchy: `DimensionError`, `ResourceLimitError`, `CorruptDataError`, `InvalidImageError`, `UnsupportedFormatError`, `CodecUnavailableError`, `UnsupportedPixelFormatError`, `ColorProfileError`, `ImageIOError`. |
| **Immutable & View Model** | [`core/image.nox`](core/image.nox) | Immutable `Image` (defensive `raw_bytes()` copy), explicit `MutableImage` (`to_image()` isolated snapshot), and zero-copy `ImageView` keeping parent `PixelStorage` alive. |
| **Pure-Nox PNG Engine** | [`codecs/png.nox`](codecs/png.nox), [`codecs/deflate.nox`](codecs/deflate.nox) | 8-bit PNG probe, decode, and adaptive-filter encode across Color Types 0, 2, 3 (`PLTE`+`tRNS`), 4, 6; Scanline Filters 0–4; RFC 1950/1951 Zlib/DEFLATE; CRC-32 & Adler-32. |
| **BMP & Netpbm Codecs** | [`codecs/bmp.nox`](codecs/bmp.nox), [`codecs/ppm.nox`](codecs/ppm.nox), [`codecs/pgm.nox`](codecs/pgm.nox) | 24-bit (`RGB8`) and 32-bit (`RGBA8`) uncompressed BMP (`BI_RGB`, bottom-up & top-down, 4-byte row padding), binary PPM (`P6`), and binary PGM (`P5`). |
| **Magic-Byte Sniffing & I/O** | [`codecs/sniff.nox`](codecs/sniff.nox), [`adapters/file.nox`](adapters/file.nox) | Extension-independent `sniff_format`, `probe`, `decode`, `encode`, `open_image`, `save_image`, `open_image_confined` (`nox.path.safe_join`), no-clobber default, and atomic file replacement. |
| **Geometry & Resampling** | [`transforms/resize.nox`](transforms/resize.nox), [`transforms/rotate.nox`](transforms/rotate.nox), [`transforms/affine.nox`](transforms/affine.nox) | `NEAREST`, `BILINEAR`, `BICUBIC`, `LANCZOS2`, `LANCZOS3`, `AREA`; `ResizeFit` (`FILL`, `CONTAIN`, `COVER`, `INSIDE`, `OUTSIDE`); `thumbnail`; `rotate90`/`180`/`270`/`rotate_arbitrary`; 2x3 `affine_transform`. |
| **Color & EXIF Hygiene** | [`core/color.nox`](core/color.nox), [`core/metadata.nox`](core/metadata.nox) | `convert_color_space` (`SRGB` ↔ `LINEAR_SRGB`), `image_to_premultiplied`, `blend_images_source_over`, `parse_exif_buffer` (cyclic-IFD guard), `sanitize_exif_buffer` (GPS/Serial/MakerNote scrub), and `iCCP` carry-through. |
| **Filters, Blur, Edges & Metrics** | [`filters/enhance.nox`](filters/enhance.nox), [`filters/gaussian.nox`](filters/gaussian.nox), [`filters/edges.nox`](filters/edges.nox), [`filters/effects.nox`](filters/effects.nox) | `grayscale`, `invert`, `threshold`, `posterize`, `solarize`, `brightness`, `contrast`, `saturation`, `gamma_correct`, `convolve`, `gaussian_blur`, `box_blur`, `sharpen`, `unsharp_mask`, `sobel`, `prewitt`, `laplacian`, `emboss`, `median_filter`, `erode`/`dilate`, `image_psnr`/`image_ssim`, `average_hash`/`difference_hash`. |
| **Drawing, Text & Compositing** | [`draw/canvas.nox`](draw/canvas.nox), [`draw/shapes.nox`](draw/shapes.nox), [`draw/text.nox`](draw/text.nox), [`composite/layers.nox`](composite/layers.nox) | `Canvas` clipping, `PorterDuffMode` (6 modes), `BlendMode` (7 W3C modes), `apply_mask`/`masked_composite`, `Layer`/`composite_layers`, anti-aliased lines, circles, rounded rects, polygons, Bezier curves, iterative `flood_fill`, and embedded 8x8 bitmap font (`measure_text`, `draw_text`). |
| **Opt-In Native Adapters & Probers** | [`codecs/jpeg_adapter.nox`](codecs/jpeg_adapter.nox), [`codecs/webp_adapter.nox`](codecs/webp_adapter.nox), [`codecs/tiff_adapter.nox`](codecs/tiff_adapter.nox), [`native/include/prism_native_abi.h`](native/include/prism_native_abi.h) | Pure-NOX header probing (`probe_jpeg`, `probe_webp`, `probe_tiff`), `ResourceLimits` & `EncodeOptions` validation (`JpegSubsampling`, `quality`, `lossless`), C ABI boundary (`PRISM_NATIVE_ABI_VERSION=1`), and typed `CodecUnavailableError` fallback when native libraries are absent. |
| **Animation & Pure-NOX GIF Engine** | [`animation/frame.nox`](animation/frame.nox), [`animation/timeline.nox`](animation/timeline.nox), [`codecs/gif.nox`](codecs/gif.nox), [`animation/gif.nox`](animation/gif.nox) | Bounded `Frame` & `Animation` model (`max_frames`, total pixel budget, delay normalization), `Timeline.render_frames()` compositor (`NONE`/`KEEP`/`RESTORE_BACKGROUND`/`RESTORE_PREVIOUS`), and pure-NOX GIF87a/GIF89a LZW still & animated decoder/encoder. |
| **Pipeline DAG & Halo Tile Engine** | [`pipeline/graph.nox`](pipeline/graph.nox), [`pipeline/planner.nox`](pipeline/planner.nox), [`pipeline/tiles.nox`](pipeline/tiles.nox), [`pipeline/cache.nox`](pipeline/cache.nox), [`pipeline/executor.nox`](pipeline/executor.nox) | Immutable typed `PipelineGraph`, safe algebraic optimizer (`Crop`/`Flip`/`Rotate90`/`Invert`/`ConvertColor`), halo-padded `execute_tiled_node`, and CRC32-keyed LRU `PlanCache` (`max_bytes`) verified across 1,000 random pipelines. |
| **Streaming, Spill & Parallel Batch** | [`pipeline/streaming.nox`](pipeline/streaming.nox), [`pipeline/parallel.nox`](pipeline/parallel.nox) | Windowed `StreamImageReader`/`StreamImageWriter` (`with` protocol, atomic commit), `nox.path.safe_join` `SpillManager` (`max_temp_bytes`), `--strict-spawn-shared` parallel tile execution (`execute_tiled_node_parallel`), and `BatchProcessor` (`FAIL_FAST`/`COLLECT_ERRORS`, idempotency keys). |

---

## 📦 Installation

Add Prism to your Nox project using `noxc add` or in `nox.json`:

```bash
noxc add prism github.com/mburakmmm/prism --ref v0.9.2
```

```json
{
  "name": "my-image-service",
  "version": "0.1.0",
  "entry": "main.nox",
  "requires": [
    { "alias": "prism", "repo": "github.com/mburakmmm/prism", "ref": "v0.9.2" }
  ]
}
```

Then fetch and lock dependencies:

```bash
noxc fetch
```

---

## 🚀 Quickstart Guide

### 1. Immutable `Image`, `MutableImage` & Zero-Copy `ImageView`

```nox
from prism import Image, ImageView, MutableImage, Pixel, PixelFormat, ResourceLimits

lim: ResourceLimits = ResourceLimits.default()
canvas: MutableImage = MutableImage.create(
    64,
    64,
    PixelFormat.RGBA8,
    Pixel.rgba(15, 23, 42, 255),
    lim
)
canvas.fill_rect(16, 16, 32, 32, Pixel.rgba(14, 165, 233, 255))

# Snapshot into an immutable Image; later mutations on canvas do not alter img
img: Image = canvas.to_image()

# Zero-copy sub-rectangle view keeping parent storage alive
view: ImageView = img.crop_view(16, 16, 16, 16)
assert view.get_pixel(0, 0).equals(Pixel.rgba(14, 165, 233, 255)), "view pixel match"
```

### 2. Extension-Independent Magic-Byte Probing & Confined I/O

```nox
from prism import (
    DecodeOptions,
    EncodeOptions,
    Image,
    ImageInfo,
    Pixel,
    new_image,
    open_image_confined,
    probe_file,
    save_image_confined,
)

avatar: Image = new_image(32, 32, Pixel.rgba(59, 130, 246, 255))

# Confined within "./uploads" via nox.path.safe_join (blocks "../" traversal)
# Uses atomic write (.nox-tmp + rename) so failed writes never corrupt existing files
save_image_confined(avatar, "./uploads", "user_42.png", EncodeOptions.overwrite_atomic())

# Probing inspects magic bytes & CRC-32 without inflating the full pixel buffer
info: ImageInfo = probe_file("./uploads/user_42.png", DecodeOptions.default())
print(info.format_name, info.width, info.height, info.channels())

loaded: Image = open_image_confined("./uploads", "user_42.png", DecodeOptions.default())
```

### 3. Pure-Nox 8-Bit PNG Engine (RFC 2083 & RFC 1950/1951)

```nox
from nox.buffer import Buffer
from prism import DecodeOptions, EncodeOptions, Image, Pixel, decode_png, encode_png, new_image

src: Image = new_image(16, 16, Pixel.rgba(244, 63, 94, 200))
png_bytes: Buffer = encode_png(src, EncodeOptions.default())
decoded: Image = decode_png(png_bytes, DecodeOptions.default())
assert decoded.pixel_equals(src), "lossless 8-bit RGBA PNG round-trip"
```

### 4. Geometry, Alpha-Safe Resampling & Thumbnails

```nox
from prism import (
    Image,
    Pixel,
    ResampleFilter,
    ResizeOptions,
    flip_horizontal,
    new_image,
    pad_image,
    resize_with_options,
    rotate90,
    thumbnail,
)

banner: Image = new_image(120, 60, Pixel.rgba(56, 189, 248, 255))

# Proportional downscale without upscaling smaller images
thumb: Image = thumbnail(banner, 40, 40, ResampleFilter.LANCZOS3)

# Fit inside 48x48 with centered letterbox padding and alpha-halo protection
contained: Image = resize_with_options(
    banner,
    48,
    48,
    ResizeOptions.fit_contain(ResampleFilter.BICUBIC, Pixel.rgba(255, 255, 255, 255))
)

rotated: Image = rotate90(flip_horizontal(contained))
framed: Image = pad_image(rotated, 2, 2, 2, 2, Pixel.rgba(250, 204, 21, 255))
```

### 5. Linear-Light Porter-Duff Blending & Premultiplied Alpha

```nox
from prism import (
    ColorSpace,
    Image,
    Pixel,
    blend_images_source_over,
    convert_color_space,
    image_to_premultiplied,
    image_to_straight,
    new_image,
)

backdrop: Image = new_image(8, 8, Pixel.rgba(0, 255, 0, 255))
overlay: Image = new_image(8, 8, Pixel.rgba(255, 0, 0, 128))

# Physically accurate linear-light Porter-Duff source-over composition
composited: Image = blend_images_source_over(backdrop, overlay, True)

# Straight <-> Premultiplied alpha & sRGB <-> LinearSRGB conversion
premult: Image = image_to_premultiplied(composited)
straight: Image = image_to_straight(premult)
linear_img: Image = convert_color_space(straight, ColorSpace.LINEAR_SRGB)
```

### 6. EXIF Orientation & Private Metadata Scrubbing (GPS / Serial)

```nox
from nox.buffer import Buffer
from prism import (
    ExifSummary,
    ResourceLimits,
    build_minimal_exif_orientation,
    parse_exif_buffer,
    sanitize_exif_buffer,
)

lim: ResourceLimits = ResourceLimits.default()
raw_exif: Buffer = build_minimal_exif_orientation(6)
summary: ExifSummary = parse_exif_buffer(raw_exif, lim)

# Scrub GPS coordinates, camera serial numbers, and MakerNotes; reset Orientation to 1
clean_exif: Buffer = sanitize_exif_buffer(raw_exif, True, lim)
```

---

## 🌐 Cross-Platform & Compatibility Matrix

Prism is continuously verified on every push, pull request, and release tag via GitHub Actions across:

| Platform | Architecture | Toolchain / Backend | Status |
| :--- | :--- | :--- | :---: |
| **Ubuntu Linux** | x86_64, aarch64 | `noxc 2.2.38` (LLVM & QBE backends) | ✅ Passing |
| **macOS** | Apple Silicon (arm64), Intel (x86_64) | `noxc 2.2.38` (LLVM & QBE backends) | ✅ Passing |
| **Windows 10/11** | x86_64 | `noxc 2.2.38` (Windows hosted backend) | ✅ Passing |
| **Freestanding Profile** | Target-agnostic (`--profile freestanding`) | `49` pure-Nox core, transform, filter, composite, draw, animation, pipeline, DEFLATE, GIF & adapter modules | ✅ Passing |

---

## 📚 Documentation Index

Detailed engineering contracts and specifications are maintained in [`docs/`](docs/):

| Document | Description |
| :--- | :--- |
| [**Engineering Contract (`AGENTS.md`)**](AGENTS.md) | Binding architecture, security invariants, phase gates (`F0`–`F16`), and non-negotiable rules. |
| [**Version & Phase Tracker (`todo.md`)**](todo.md) | Live SemVer roadmap (`v0.0.1` -> `v1.0.0`) with verification command logs. |
| [**Architecture & ADR-001**](docs/architecture.md) | Layer hierarchy, dependency direction, and pure-Nox core vs. isolated optional native codec strategy. |
| [**Capabilities Matrix**](docs/capabilities.md) | Verified feature, codec, and backend matrix for the active release. |
| [**Nox 2.2.38 Compatibility Report**](docs/nox-compatibility.md) | Empirical probe findings for `Buffer`, `Span`, `nox.bytes`, `nox.binary`, `nox.mathx`, `spawn`, and FFI. |
| [**Nox 2.2.38 Limitations Report**](docs/nox-limitations.md) | Empirically observed compiler/stdlib gaps (`__all__`, freestanding math, escapes, overflow) and workarounds. |
| [**Pixel Data Model**](docs/pixels.md) | `PixelFormat`, stride layout, checked buffer sizing, and `Image`/`MutableImage`/`ImageView` ownership contracts. |
| [**Color, Alpha & Metadata Hygiene**](docs/colors.md) | sRGB vs. LinearSRGB, straight vs. premultiplied alpha, cyclic-IFD defense, and GPS/Serial scrubbing. |
| [**Roadmap**](docs/roadmap.md) | Phase-by-phase table tracking status, compiler baseline, target platforms, tests, and known limits. |
| [**Security Threat Model**](docs/security.md) | Decompression-bomb budgets (`ResourceLimits`), checked arithmetic, path confinement, and atomic writes. |
| [**Native Codec Adapters & ABI**](docs/native-adapters.md) | C ABI isolation (`PRISM_NATIVE_ABI_VERSION`), third-party license matrix (`libjpeg-turbo`/`libwebp`/`libtiff`), and CVE hygiene. |

---

## 🧪 Automated Test, Golden & Fuzz Suite

Prism maintains a **100% passing automated verification suite** across **8 test suites** (representing **11,150+ individual assertions, property checks, golden comparisons, and fuzz iterations** on both LLVM and QBE backends):

```bash
noxc test
noxc test --backend qbe
```

- **Core Pixel & Security Boundary Suite** ([`tests/unit/core_pixel_test.nox`](tests/unit/core_pixel_test.nox)): 2x2 RGBA8 lifecycle, 26 negative overflow/boundary/alias-isolation tests, and 1,000 pseudo-random small image constructor/view property tests.
- **10,000-Iteration Malformed Header Fuzzer** ([`tests/fuzz/header_fuzz_test.nox`](tests/fuzz/header_fuzz_test.nox)): Fuzzes mutated, truncated, and overflowing PPM, PGM, BMP, PNG, JPEG, WebP, GIF, and TIFF headers under strict memory budgets with zero crashes.
- **Pure-Nox PNG & DEFLATE Verification** ([`tests/codecs/png_test.nox`](tests/codecs/png_test.nox)): IEEE 802.3 CRC-32 & RFC 1950 Adler-32 test vectors, Stored/Fixed/Dynamic Huffman DEFLATE blocks, all 5 PNG Color Types (`0`, `2`, `3`, `4`, `6`), all 5 Scanline Filters (`0..4`), multi-chunk `IDAT`, decompression bomb cutoff, and external reader verification (`file`, macOS `sips`).
- **120-Case Resampling Golden Suite** ([`tests/golden/resample_golden_test.nox`](tests/golden/resample_golden_test.nox)): Alpha-bleeding halo prevention regression tests, all 5 `ResizeFit` modes + `thumbnail`, and 120 golden resampling cases across `NEAREST`, `BILINEAR`, `BICUBIC`, `LANCZOS2`, `LANCZOS3`, and `AREA`.
- **Geometry & Metamorphic Suite** ([`tests/unit/geometry_test.nox`](tests/unit/geometry_test.nox)): Exact metamorphic identities (`flip` x2, `rotate90` x4, `transpose` x2), `STRICT`/`CLIP`/`PAD` crop, 4 border modes, EXIF orientation 1–8, arbitrary rotation, and 2x3 affine transformations.
- **Color, Alpha & EXIF Hygiene Suite** ([`tests/unit/color_metadata_test.nox`](tests/unit/color_metadata_test.nox)): Linear-light vs. perceptual sRGB blending, straight ↔ premultiplied alpha, ICC capability gating (`ColorProfileError`), cyclic-IFD pointer attack detection, and byte-level GPS/Serial/MakerNote scrubbing.

---

## 🗺️ Roadmap & Release Phases

| Version | Phase | Milestone | Status |
| :--- | :--- | :--- | :---: |
| **`v0.0.1`** | **F0** | Discovery, `noxc 2.2.38` Capability Probe, ADR-001 & CI Foundation | ✅ Released |
| **`v0.1.0`** | **F1** | Safe Pixel Core, 64-Bit Checked Arithmetic, `Image`/`MutableImage`/`ImageView` & PPM/PGM Slice | ✅ Released |
| **`v0.2.0`** | **F2** | Portable Image I/O, Magic-Byte Sniffing, 24/32-Bit BMP & 10k Header Fuzz | ✅ Released |
| **`v0.3.0`** | **F3** | Geometry, Affine & Alpha-Safe Resampling Engine (120 Golden Cases) | ✅ Released |
| **`v0.4.0`** | **F4** | Pure-Nox 8-Bit PNG Engine (RFC 1950/1951 Zlib/DEFLATE, Scanline Filters 0–4) | ✅ Released |
| **`v0.5.0`** | **F5** | Color, Alpha & EXIF Metadata Hygiene (MVP Milestone Complete) | ✅ Released |
| **`v0.6.0`** | **F6** | Filters, Enhancement, Convolution, Gaussian Blur, Sobel, Morphology & PSNR/SSIM | ✅ Released |
| **`v0.7.0`** | **F7** | Drawing, Text (Embedded Bitmap Font), Masks & Porter-Duff / Blend Mode Compositing | ✅ Released |
| **`v0.8.0`** | **F8** | Optional Native Codec Adapters (JPEG, WebP, TIFF) with C ABI Isolation & Pure-NOX Probers | ✅ Released |
| **`v0.8.5`** | **F9** | Animation & Multi-Frame Formats (`Frame`, `Animation`, `Timeline`, Pure-NOX GIF LZW/Disposal) | ✅ Released |
| **`v0.9.0`** | **F10** | Pipeline DAG, Safe Algebraic Planner & Halo-Aware Tile Executor | ✅ Released |
| **`v0.9.2`** | **F11** | Streaming Reader/Writer, Bounded Disk Spill & Parallel Tile/Batch Execution | ✅ Released |
| **`v0.9.4`** | **F12** | Advanced Color/Type (`GRAY16`/`RGB16`/`RGBA16`/`RGBAF32`), Adam7 PNG, Dithering, CLAHE & Canny | 🔜 Next |
| **`v0.9.6`–`v1.0.0`** | **F13–F16** | CLI/Ecosystem Adapters, Portability, Security/RC & 1.0 | 📋 Planned |

---

## 🤝 Contributing, Security & License

- **Contributing**: Please review [`CONTRIBUTING.md`](CONTRIBUTING.md), [`AGENTS.md`](AGENTS.md), and [`todo.md`](todo.md) before opening a pull request.
- **Code of Conduct**: All contributors are expected to uphold our [`CODE_OF_CONDUCT.md`](CODE_OF_CONDUCT.md).
- **Security Policy**: Report vulnerabilities privately via GitHub Security Advisories as detailed in [`SECURITY.md`](SECURITY.md).
- **License**: Prism is licensed under the permissive [MIT License](LICENSE). Copyright © 2026 Melih Burak Memiş.

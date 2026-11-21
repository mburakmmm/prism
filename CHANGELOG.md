# Changelog

All notable changes to Prism are documented in this file. Prism follows Semantic Versioning (`0.x` experimental phase gates leading to `1.0.0`).

## [0.6.0] — 2026-11-21 (Phase F6 Filters, Enhancement & Pixel Kernels)

### Added

- `filters/enhance.nox`: Point enhancement and channel operations (`GrayscaleMode.SRGB_LUMA`, `GrayscaleMode.LINEAR_LUMINANCE`, `grayscale`, `invert`, `threshold`, `posterize`, `solarize`, `brightness`, `contrast`, `saturation`, `gamma_correct`, `apply_lut`, `split_channels`, `merge_channels`).
- `filters/effects.nox`: Photographic effects (`sepia`, `pixelate`, `vignette`), per-channel statistics (`ChannelStats`, `channel_histogram`, `compute_channel_stats`, `histogram_equalize`), image quality comparison metrics (`image_mse`, `image_mae`, `image_psnr`, `image_ssim`), and non-cryptographic perceptual hashes (`average_hash`, `difference_hash`).
- `filters/convolution.nox` & `filters/gaussian.nox`: General odd-sized 2D kernel convolution (`convolve` with `MAX_KERNEL_SIZE = 31` and `BorderMode` support), `gaussian_kernel_1d`, alpha-safe separable 2-pass `box_blur` and `gaussian_blur` (`MAX_BLUR_RADIUS = 32`).
- `filters/sharpen.nox`, `filters/edges.nox`, `filters/median.nox`, `filters/morphology.nox`: `sharpen`, `unsharp_mask`, 3x3 gradient edge detectors (`sobel`, `prewitt`, `laplacian`, `emboss`), non-linear impulse noise filter (`median_filter`), and grayscale/per-channel morphological operators (`erode`, `dilate`, `morphology_open`, `morphology_close`).
- `examples/filters.nox` & `tests/unit/filters_test.nox`: End-to-end filter showcase and unit/golden/boundary tests (including 2-pass separable Gaussian blur vs. 2D reference convolution consistency within 1 LSB and transparent edge halo tests) verified on both LLVM and QBE backends and under `--profile freestanding`.

## [0.5.0] — 2026-11-21 (Phase F5 Color, Alpha & Metadata Hygiene — MVP Complete)

### Added

- `core/color.nox`: Physical linear-light Porter-Duff composition (`blend_source_over_linear`, `blend_images_source_over`), image-level straight ↔ premultiplied alpha conversion (`image_to_premultiplied`, `image_to_straight`), and `convert_color_space` (`SRGB` ↔ `LINEAR_SRGB` with explicit `ColorProfileError` gating for `DISPLAY_P3` / `ICC_PROFILE` / `UNKNOWN`).
- `core/metadata.nox`: Bounded EXIF/TIFF IFD parser (`ExifSummary`, `parse_exif_buffer`) supporting both little-endian (`II*\0`) and big-endian (`MM\0*`) headers, cyclic-IFD pointer attack detection, `max_recursion_depth` enforcement, `build_minimal_exif_orientation`, and `sanitize_exif_buffer` stripping GPS (`0x8825`), Camera Body Serial Number (`0xA431`), MakerNote (`0x927C`), UserComment (`0x9286`), and Artist (`0x013B`) tags.
- `codecs/png.nox`: `eXIf`, `iCCP`, and `tEXt` chunk integration with automatic EXIF orientation normalization (`DecodeOptions.autorotate = True` resetting orientation to `1`), private metadata stripping (`EncodeOptions.strip_private_metadata = True`), and bounded `iCCP` profile carry-through.
- `docs/pixels.md`, `docs/colors.md`, and `tests/unit/color_metadata_test.nox` (verified on both LLVM and QBE backends).

## [0.4.0] — 2026-11-21 (Phase F4 Pure-NOX 8-bit PNG Engine)

### Added

- `codecs/deflate.nox`: Pure-NOX IEEE 802.3 CRC-32 (`crc32_bytes`, `crc32_span`), RFC 1950 Adler-32 (`adler32_bytes`), RFC 1950 Zlib container validator (`CMF`/`FLG`/`FCHECK`/`FDICT` check), RFC 1951 DEFLATE decompressor (`zlib_decompress` supporting Stored Block 0, Fixed Huffman Block 1, Dynamic Huffman Block 2, and strict `max_output_bytes` decompression-bomb cutoff), and RFC 1951 DEFLATE compressor (`zlib_compress` with LZ77 sliding-window hash chain + RLE + Fixed Huffman).
- `codecs/png.nox`: Pure-NOX 8-bit PNG prober (`probe_png`), decoder (`decode_png`, `open_png`), and adaptive-filter encoder (`encode_png`, `save_png`) supporting:
  - Color Type 0 (1/2/4/8-bit Grayscale + optional `tRNS` transparent key),
  - Color Type 2 (8-bit Truecolor RGB + optional `tRNS` transparent key),
  - Color Type 3 (1/2/4/8-bit Indexed Palette `PLTE` + optional `tRNS` alpha table),
  - Color Type 4 (8-bit Grayscale + Alpha),
  - Color Type 6 (8-bit Truecolor RGBA),
  - Scanline Filters 0 (`None`), 1 (`Sub`), 2 (`Up`), 3 (`Average`), and 4 (`Paeth`),
  - Multi-chunk `IDAT` stream assembly, per-chunk CRC-32 verification, and typed errors for 16-bit (`UnsupportedPixelFormatError`) and Adam7 interlaced (`UnsupportedFormatError`) variants.
- `codecs/registry.nox`, `codecs/reader.nox`, `codecs/writer.nox`: Activated PNG probing, decoding, and encoding across `probe`, `decode`, `encode`, `probe_file`, `open_image`, and `save_image`.
- `examples/avatar.nox` & `tests/codecs/png_test.nox`: Full unit, round-trip, scanline filter, decompression bomb, and external macOS `sips`/`file` reader verification.

## [0.3.0] — 2026-11-21 (Phase F3 Geometry & Resampling Engine)

### Added

- `transforms/crop.nox` & `transforms/border.nox`: `CropBoundsPolicy` (`STRICT`, `CLIP`, `PAD`), `crop_image`, `crop_with_policy`, `BorderMode` (`CONSTANT`, `CLAMP`, `MIRROR`, `WRAP`), `sample_with_border`, `pad_image`, and `pad_with_mode`.
- `transforms/flip.nox` & `transforms/rotate.nox`: Exact geometric transformations (`flip_horizontal`, `flip_vertical`, `transpose`, `transverse`, `rotate90`, `rotate180`, `rotate270`), EXIF orientation 1–8 normalizer (`apply_exif_orientation`, `normalize_orientation` with tag reset to prevent double-rotation), and arbitrary-angle rotation (`rotate_arbitrary` with bounding-box expansion and 90-degree fast-path dispatch).
- `transforms/resample.nox` & `transforms/resize.nox`: `ResampleFilter` (`NEAREST`, `BILINEAR`, `BICUBIC`, `LANCZOS2`, `LANCZOS3`, `AREA`) using the `x + 0.5`, `y + 0.5` pixel-center convention, premultiplied-alpha weighting (`alpha_safe=True`) eliminating dark transparent border halos, optional linear-light sRGB resampling (`ResizeOptions.high_quality()`), `ResizeFit` modes (`FILL`, `CONTAIN`, `COVER`, `INSIDE`, `OUTSIDE`), and proportional `thumbnail`.
- `transforms/affine.nox` & `transforms/geometry.nox`: `AffineMatrix2D` (`identity`, `translation`, `scaling`, `rotation_deg`, `invert`) and inverse-mapped `affine_transform` with finite-float (`NaN`/`Inf`) and singular matrix guards.
- `examples/thumbnails.nox`, `tests/unit/geometry_test.nox`, and `tests/golden/resample_golden_test.nox` (metamorphic identities, EXIF orientation 1–8, alpha-halo regression, and 120 golden resampling cases verified on both LLVM and QBE).

## [0.2.0] — 2026-11-21 (Phase F2 Portable Image I/O & Minimal Codec Suite)

### Added

- `codecs/sniff.nox`: Extension-independent magic-byte format detection (`ImageFormat`, `sniff_format`, `format_from_name`, `format_from_extension`) recognizing `PPM` (`P6`), `PGM` (`P5`), `BMP`, `PNG`, `JPEG`, `WEBP`, `GIF`, and `TIFF`, while rejecting ASCII Netpbm `P2`/`P3` with `UnsupportedFormatError`.
- `codecs/bmp.nox`: Pure-NOX 24-bit (`RGB8`) and 32-bit (`RGBA8`) uncompressed BMP (`BI_RGB`) prober, decoder (supporting both bottom-up and top-down negative height headers and 4-byte row padding), and encoder (`probe_bmp`, `decode_bmp`, `encode_bmp`, `open_bmp`, `save_bmp`).
- `codecs/registry.nox`, `codecs/reader.nox`, `codecs/writer.nox`: Unified codec capability table (`CodecCapability`, `list_codec_capabilities`) and byte dispatch (`probe_bytes`, `decode_bytes`, `encode_bytes`, `encode_to_format`) with deterministic `CodecUnavailableError` (for optional native codecs JPEG/WebP/TIFF) and `UnsupportedFormatError`.
- `adapters/memory.nox` & `adapters/file.nox`: Public I/O surface (`probe`, `decode`, `encode`, `probe_file`, `open_image`, `save_image`, `save_image_as`, `open_image_confined`, `save_image_confined`) with `nox.path.safe_join` path traversal protection, no-clobber default, and atomic file replacement.
- `tests/codecs/portable_codecs_test.nox`: End-to-end round-trip tests for 24-bit and 32-bit BMP (including odd row padding and top-down BMP), extension-independent sniffing, no-clobber, atomic preservation on encode failure, and confined path traversal blocking.
- `tests/fuzz/header_fuzz_test.nox`: 10,000-iteration synthetic malformed/truncated/overflowing header fuzz test verified on both LLVM and QBE backends without crashes.

## [0.1.0] — 2026-11-21 (Phase F1 & §29 Initial Autonomous Slice)

### Added

- `core/errors.nox`: Typed exception hierarchy rooted at `PrismError` (`InvalidImageError`, `CorruptDataError`, `UnsupportedFormatError`, `CodecUnavailableError`, `UnsupportedPixelFormatError`, `ColorProfileError`, `ResourceLimitError`, `DimensionError`, `DecodeError`, `EncodeError`, `ImageIOError`, `PipelineError`, `NativeAdapterError`).
- `core/limits.nox`: 64-bit overflow-checked integer arithmetic (`checked_add`, `checked_sub`, `checked_mul`, `checked_stride`, `checked_buffer_size`) and `ResourceLimits` profiles (`default()`, `strict_server()`, `constrained_wasm()`).
- `core/pixel.nox`: `PixelFormat`, `ColorSpace`, `AlphaMode`, and range-checked 8-bit `Pixel` class with BT.709 perceptual luma.
- `core/color.nox`: sRGB ↔ LinearSRGB conversion, straight ↔ premultiplied alpha conversion, and Porter-Duff `source-over` blending.
- `core/metadata.nox` & `core/options.nox`: `ImageMetadata`, `ImageInfo`, `DecodeOptions`, and `EncodeOptions`.
- `core/storage.nox` & `core/image.nox`: `PixelStorage`, immutable `Image`, controlled `MutableImage`, and zero-copy `ImageView` with parent lifetime retention and snapshot isolation.
- `codecs/ppm.nox` & `codecs/pgm.nox`: Netpbm P6 (binary RGB8) and P5 (binary GRAY8) probing, decoding, and atomic/no-clobber encoding.
- `prism.nox`: Stable public facade for `v0.1.0`.
- `examples/create_checkerboard.nox`: 64x64 RGBA8 checkerboard generator with PPM P6 round-trip and synthetic test fixture generation (`tests/fixtures/checkerboard_4x4.ppm`, `tests/fixtures/gradient_4x4.pgm`).
- `tests/unit/core_pixel_test.nox` & `tests/unit/facade_test.nox`: Unit test suite including 2x2 RGBA8 lifecycle, 26 negative security/boundary tests, and 1,000 pseudo-random small image constructor/view property tests verified on both LLVM and QBE backends.

## [0.0.1] — 2026-11-21 (Phase F0 Discovery & NOX 2.2.38 Audit)

### Added

- `nox.json`, `.gitignore`, `LICENSE` (MIT), `SECURITY.md`, `CONTRIBUTING.md`, and cross-platform CI workflows (`.github/workflows/ci.yml`, `.github/workflows/security.yml`).
- `examples/capability_probe.nox`: Verified 16 MiB and 64 MiB `Buffer` allocation, `Span` mutation, `nox.bytes` binary `str` round-trip with NUL and invalid UTF-8 bytes, `nox.binary` endian readers/writers, 64-bit checked arithmetic, `nox.fs` chunked I/O, `nox.path.safe_join` traversal blocking, `spawn` parallel tile workers (`--strict-spawn-shared`), and `lowlevel` `ptr_of` + C FFI `memset` on `noxc 2.2.38` (LLVM and QBE).
- ADR-001 and baseline documentation in `docs/architecture.md`, `docs/capabilities.md`, `docs/nox-compatibility.md`, `docs/roadmap.md`, `docs/security.md`, `tests/fixtures-manifest.json`, and `benchmarks/perf_baseline.json`.

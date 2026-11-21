# Prism Security Architecture & Threat Model

## 1. Threat Model (`AGENTS.md` §20)

Prism assumes all external inputs are controlled by an adversary:

- Encoded image bytes, headers, chunk lengths, and compression streams.
- Declared width, height, stride, channel count, and frame count fields.
- EXIF/TIFF IFD pointers, GPS tags, ICC color profiles, and text comments.
- File paths, output filenames, and remote URLs.
- Concurrent batch and HTTP upload requests.

## 2. Default Resource Budgets (`ResourceLimits`)

Every decode, encode, buffer allocation, and multi-frame operation validates its resource cost against `ResourceLimits` *before* allocating memory:

| Parameter | `ResourceLimits.default()` | `ResourceLimits.strict_server()` | `ResourceLimits.constrained_wasm()` | Purpose |
|---|---:|---:|---:|---|
| `max_width` | 32,768 px | 8,192 px | 4,096 px | Single horizontal axis bound |
| `max_height` | 32,768 px | 8,192 px | 4,096 px | Single vertical axis bound |
| `max_pixels` | 100,000,000 | 25,000,000 | 16,777,216 | Decompression bomb pixel ceiling |
| `max_decoded_bytes` | 512 MiB | 96 MiB | 32 MiB | Total decoded pixel buffer ceiling |
| `max_input_bytes` | 256 MiB | 16 MiB | 8 MiB | Input file/stream size ceiling |
| `max_frames` | 200 | 50 | 24 | Multi-frame / animation count ceiling |
| `max_metadata_bytes` | 4 MiB | 1 MiB | 256 KiB | Combined EXIF/ICC/comment ceiling |
| `max_chunk_bytes` | 64 MiB | 8 MiB | 4 MiB | Single container chunk ceiling |
| `max_recursion_depth` | 32 | 16 | 16 | Parser / IFD nesting limit |
| `max_output_bytes` | 512 MiB | 64 MiB | 32 MiB | Encoded output ceiling |
| `max_temp_bytes` | 1 GiB | 128 MiB | 0 B | Disk spill quota |
| `max_work_ms` | 30,000 ms | 5,000 ms | 5,000 ms | Host cooperative deadline budget |

## 3. Mandatory Security Controls Implemented in `v0.1.0`

1. **64-bit Overflow Prevention (`core/limits.nox`):**
   - Plain 64-bit `int` in NOX wraps around on overflow. `checked_add`, `checked_sub`, `checked_mul`, `checked_stride`, and `checked_buffer_size` verify bounds prior to arithmetic and raise `ResourceLimitError` or `DimensionError`.
2. **Buffer and Alias Isolation (`core/storage.nox`, `core/image.nox`):**
   - `Image` is immutable. `Image.from_buffer` copies and validates caller bytes. `Image.raw_bytes()` returns a defensive copy so external callers cannot mutate internal pixel buffers.
   - `MutableImage.to_image()` clones storage so subsequent mutations to `MutableImage` never alter existing `Image` instances.
   - `ImageView` retains a reference to the parent `PixelStorage` and checks `(offset_x + x, offset_y + y)` bounds on every read.
3. **Atomic and No-Clobber File Output (`codecs/ppm.nox`, `codecs/pgm.nox`):**
   - `EncodeOptions.default()` enforces `overwrite = False` (no-clobber) and `atomic_write = True` (via `nox.fs.write_string_atomic`), ensuring a failed write never corrupts an existing file.

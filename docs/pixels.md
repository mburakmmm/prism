# Prism Pixel Data Model & Memory Contracts

## 1. Supported Pixel Formats (`PixelFormat`)

| Variant | Channels | Bytes/Pixel | Layout | Status in `v0.5.0` |
|---|---:|---:|---|---|
| `GRAY8` | 1 | 1 | 8-bit luminance (`u8`) | **Active in Core** |
| `RGB8` | 3 | 3 | Interleaved `R, G, B` (`u8`) | **Active in Core** |
| `RGBA8` | 4 | 4 | Interleaved `R, G, B, A` (`u8`) | **Active in Core** |
| `GRAY16` | 1 | 2 | 16-bit grayscale | Planned (Phase F12) |
| `RGB16` | 3 | 6 | Interleaved 16-bit RGB | Planned (Phase F12) |
| `RGBA16` | 4 | 8 | Interleaved 16-bit RGBA | Planned (Phase F12) |
| `RGBAF32` | 4 | 16 | Interleaved 32-bit float RGBA | Planned (Phase F12) |
| `INDEXED8` | 1 | 1 | 8-bit palette index (normalized to `RGB8`/`RGBA8` on decode) | Normalized in PNG (F4) / GIF (F9) |

## 2. `Image`, `MutableImage`, and `ImageView` Invariants

1. **Positive Dimensions:** `width > 0` and `height > 0` are enforced at construction; zero or negative dimensions raise `DimensionError`.
2. **Checked Stride & Buffer Allocation:**
   - `stride >= checked_mul(width, bytes_per_pixel)`.
   - `buffer_length >= checked_add(checked_mul(height - 1, stride), checked_mul(width, bytes_per_pixel))`.
   - Every allocation is checked against `ResourceLimits` (`max_width`, `max_height`, `max_pixels`, `max_decoded_bytes`) *before* allocating `nox.buffer.Buffer`.
3. **Immutability & Alias Isolation:**
   - `Image` is immutable. `Image.raw_bytes()` returns a freshly allocated tightly-packed `Buffer` copy so callers cannot mutate internal storage.
   - `MutableImage.to_image()` clones the underlying `PixelStorage`, ensuring later mutations on `MutableImage` never affect previously created `Image` snapshots.
   - `ImageView` holds a strong reference to the parent `PixelStorage` and translates `(x, y)` coordinates with `checked_add(offset_x, x)` and `checked_add(offset_y, y)`.

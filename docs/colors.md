# Prism Color Spaces, Alpha Modes & Metadata Hygiene

## 1. Color Spaces (`ColorSpace`)

- `ColorSpace.SRGB`: Standard 8-bit gamma-encoded sRGB (`IEC 61966-2-1`).
- `ColorSpace.LINEAR_SRGB`: Physically linear sRGB light (`[0.0, 1.0]` mapped to 8-bit or float intermediates).
- `ColorSpace.DISPLAY_P3` / `ColorSpace.ICC_PROFILE`: Wide-gamut / embedded ICC profile tags. Without an active ICC profile transformation engine, `convert_color_space` fails fast with `ColorProfileError` rather than silently claiming a wide-gamut conversion.
- `ColorSpace.UNKNOWN`: Untagged input when `DecodeOptions.assume_srgb == False`.

## 2. Straight vs. Premultiplied Alpha (`AlphaMode`)

- `AlphaMode.OPAQUE`: All pixels have implicit alpha `255` (`GRAY8`, `RGB8`).
- `AlphaMode.STRAIGHT`: Default for `RGBA8`; RGB channels are stored independent of alpha.
- `AlphaMode.PREMULTIPLIED`: RGB channels are pre-scaled by `alpha / 255.0` with integer rounding (`(c * a + 127) // 255`).
- **Resampling & Blending:**
  - `ResizeOptions` defaults to `alpha_safe = True`, which weights RGB samples by alpha so transparent (`alpha == 0`) pixels never bleed dark halos into adjacent opaque pixels.
  - `blend_source_over_linear` and `blend_images_source_over(dst, src, linear_light=True)` perform Porter-Duff `source-over` in linear light to avoid dark gamma midpoints.

## 3. EXIF Orientation & Private Metadata Hygiene (`core/metadata.nox`)

- **EXIF Orientation (1–8):**
  - `parse_exif_buffer` parses little-endian (`II*\0`) and big-endian (`MM\0*`) TIFF/EXIF headers with cyclic-IFD detection and `max_recursion_depth` enforcement.
  - When `DecodeOptions.autorotate == True` (default), `decode_png` applies `apply_exif_orientation(img, orientation)` and resets `img.metadata.orientation = 1`, preventing double-rotation on subsequent saves.
- **Privacy & GPS Scrubbing (`strip_private_metadata`):**
  - `sanitize_exif_buffer` and `EncodeOptions.default()` (`strip_private_metadata = True`) strip `GPSInfoIFDPointer` (`0x8825`), GPS coordinates, `MakerNote` (`0x927C`), `BodySerialNumber` (`0xA431`), `UserComment` (`0x9286`), `Artist` (`0x013B`), and PNG `tEXt` comments.
- **ICC Profile Carry-Through:**
  - Embedded `iCCP` chunks are validated against `ResourceLimits.max_metadata_bytes` and carried in `ImageMetadata.icc_profile` independently of private EXIF stripping.

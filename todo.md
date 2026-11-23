# Prism Geliştirme ve Sürüm Takip Planı (`todo.md`)

> **Proje:** Prism — NOX için native görüntü işleme, dönüştürme ve görüntü I/O framework'ü  
> **Kaynak sözleşme:** `AGENTS.md` (`PRISM_AGENTS.md`)  
> **Derleyici tabanı:** `noxc 2.2.38` (`/Users/melihburakmemis/.nox-lang/bin/noxc`, macOS arm64, LLVM & QBE)  
> **Son güncelleme:** 2026-11-21  
> **Tamamlanan sürümler:** `v0.0.1` (Faz F0), `v0.1.0` (Faz F1 + §29), `v0.2.0` (Faz F2), `v0.3.0` (Faz F3), `v0.4.0` (Faz F4) & `v0.5.0` (Faz F5 — Color, Alpha & Metadata Hygiene / MVP Tamamlandı)  
> **Sıradaki aktif hedef:** `v0.6.0` (Faz F6 — Filters, Enhancement & Pixel Kernels: Grayscale, Brightness/Contrast/Saturation/Gamma, Convolution, Gaussian, Sharpen, Sobel, Median, Morphology, Stats/SSIM/PSNR)

Bu dosya, `AGENTS.md` içindeki bağlayıcı mühendislik sözleşmesini, güvenlik kapılarını, mimari sınırları ve F0–F16 üretim fazlarını gerçek SemVer sürümlerine (`v0.0.1` -> `v1.0.0`) ve doğrulanabilir iş kalemlerine dönüştüren canlı takip belgesidir.

---

## 1. Takip Kuralları ve Agent Çalışma Disiplini

### 1.1 Durum İşaretleri

- `[ ]`: Henüz başlanmadı veya tamamlanmadı.
- `[x]`: Kod, test, örnek, dokümantasyon ve derleyici/çalıştırma kanıtı eksiksiz tamamlandı.
- `BLOCKED`: Dış karar, kullanıcı onayı veya harici ortam gereksinimi nedeniyle bekliyor.
- `NOT VERIFIED`: Kod/tasarım hazır ancak ilgili hedef platformda (örn. Windows runner, fiziksel mobil cihaz) henüz komutla çalıştırılmadı.

### 1.2 Bağlayıcı İlkeler (`AGENTS.md` §0, §3, §27, §28)

- **NOX-first:** Tüm public API, görüntü modeli, iş hattı, filtreler ve taşınabilir algoritmalar saf `.nox` ile yazılır.
- **Sıfır sahte iddia:** Yalnızca planlanan veya stub bırakılan hiçbir özellik `[x]` işaretlenmez ve `prism.nox` kararlı facade'ine noop stub olarak eklenmez.
- **Güvenilmeyen girdi düşmanca kabul edilir:** `width * height * channels * bytes_per_channel`, stride, chunk ve offset hesapları tahsisten önce checked arithmetic (`checked_mul`, `checked_add`) ile doğrulanır; 64-bit signed `int` taşma (wrap-around) davranışına asla güvenilmez.
- **Sessiz bozuk çıktı yok:** Desteklenmeyen format, renk uzayı, bit derinliği, interlace veya metadata seçeneği tipli `PrismError` alt sınıfı fırlatır.
- **Önce derlenen kanıt:** Belirsiz her NOX dil veya stdlib davranışı küçük bir POC ile hem `--backend llvm` hem `--backend qbe` (ve ilgiliyse `--profile freestanding`, `--strict-spawn-shared`) altında derlenip çalıştırılarak doğrulanır.
- **İsim çakışması disiplini:** NOX modül adı çözümleme kuralı gereği (`NXL-003`), yerel değişken adları import edilen modül adlarıyla (`pixel`, `image`, `limits`, `errors`, `color`, `metadata`, `options`, `storage`, `ppm`, `bmp`, `png`) aynı seçilmez (`px`, `img`, `lim`, `err`, `clr`, `meta`, `opt`, `store` kullanılır).
- **Modül sınırı disiplini:** Modüller arası fonksiyon çağrılarında opsiyonel argüman düşürme (`NXL-005`) veya karmaşık sınıflarda `int | None` (`NXL-001`) yerine açık tipli seçenek sınıfları (`ResourceLimits.default()`, `DecodeOptions.default()`) ve açık `has_*` boolean alanları tercih edilir.
- **Her özellik için Definition of Done (§28):**
  - [x] Girdi/çıktı/yan etki/bellek/renk/alpha semantiği yazılı.
  - [x] Public API statik tipli, test edilmiş, uygun modülde.
  - [x] Input ve output `ResourceLimits` uygulanıyor.
  - [x] Checked arithmetic ve buffer sınırları testli.
  - [x] En az bir normal, bir negatif, bir sınır (edge) ve uygun golden/property testi var.
  - [x] Hata/kaynak temizliği (`with`/`defer`/`finally`) testli.
  - [x] `noxc fmt`, `noxc check`, `noxc check --strict-spawn-shared`, `noxc test` (LLVM ve QBE) raporlu.
  - [x] README, `docs/capabilities.md`, `docs/roadmap.md`, `CHANGELOG.md` güncel.

---

## 2. Sürüm ve Faz Haritası (`F0` – `F16`)

| Sürüm | AGENTS Fazı | Kilometre Taşı / Çıktı | Öncelik Grubu (§26) | Durum |
|---|---|---|---|---|
| **v0.0.1** | **F0** | Discovery, NOX 2.2.38 Audit, Capability Probe, ADR-001, Repo & CI Temeli | MVP Temeli | **TAMAMLANDI** |
| **v0.1.0** | **F1 (+§29)** | Safe Pixel Core, Checked Arithmetic, `Image`/`MutableImage`/`ImageView`, İlk PPM/PGM P6/P5 Dikey Dilimi | MVP Çekirdeği | **TAMAMLANDI** |
| **v0.2.0** | **F2** | Portable Image I/O, Format Sniffing, PPM/PGM (P5/P6) & BMP (24/32-bit) Decode/Encode, Safe File/Memory Adapters | MVP I/O | **TAMAMLANDI** |
| **v0.3.0** | **F3** | Geometry & Resampling Engine (Crop, Pad, Flip, Rotate 90/180/270/Arbitrary, Nearest/Bilinear/Bicubic/Lanczos3/Area, Thumbnail/Fit) | MVP Transform | **TAMAMLANDI** |
| **v0.4.0** | **F4** | Saf NOX PNG Engine (8-bit Gray/RGB/RGBA/Palette+tRNS, RFC 1950/1951 Zlib/DEFLATE, Scanline Filters 0–4, CRC32/Adler32) | MVP Tamamlayıcı | **TAMAMLANDI** |
| **v0.5.0** | **F5** | Color, Alpha & Metadata Hygiene (sRGB↔LinearSRGB, Straight↔Premultiplied Alpha, EXIF Orientation 1–8, Private Metadata Strip, ICC Carry) | MVP + Web Güvenliği | **TAMAMLANDI (MVP)** |
| **v0.6.0** | **F6** | Filters, Enhancement & Pixel Kernels (Grayscale, Brightness/Contrast/Saturation/Gamma, Convolution, Gaussian, Sharpen, Sobel, Median, Morphology, Stats/SSIM/PSNR) | Web / Masaüstü | **TAMAMLANDI** |
| **v0.7.0** | **F7** | Drawing, Text, Masks & Compositing (Porter-Duff, Blend Modes, Shapes, Antialiased Rasterizer, Iterative Flood Fill, Embedded Bitmap Font) | Programatik Üretim | **TAMAMLANDI** |
| **v0.8.0** | **F8** | Native Codec Adapters (İzole C ABI Sınırı, Opt-in JPEG & WebP Decode/Encode, `CodecUnavailableError` Fallback, Leak/CVE Harness) | Web / Backend Sürümü | **TAMAMLANDI** |
| **v0.8.5** | **F9** | Animation & Advanced Formats (`Frame`/`Animation` Modeli, GIF LZW/Disposal/Transparency/Loop, APNG & Animated WebP Pilotu) | Profesyonel | **TAMAMLANDI** |
| **v0.9.0** | **F10** | Pipeline DAG, Planner & Tile Engine (Immutable Operasyon Grafı, Güvenli Plan Optimizasyonu, Halo/Border Hesabı, Bounded Tile Executor, Cache) | Yüksek Performans | **TAMAMLANDI** |
| **v0.9.2** | **F11** | Streaming, Large Image & Batch Processing (`ImageReader`/`ImageWriter`, Disk Spill & Quota, Atomik Commit, `spawn` Paralel Tile/Batch) | Yüksek Performans | **SIRADAKİ FAZ** |
| **v0.9.4** | **F12** | Advanced Color/Type & Pro Features (`GRAY16`/`RGB16`/`RGBA16`/`RGBAF32`, PNG 16-bit/Adam7, Quantization, Dithering, CLAHE, Canny, ICC Adapter) | Profesyonel | PLANLANDI |
| **v0.9.6** | **F13** | Ecosystem Integrations & CLI (`prism` CLI, Nyx Upload/Thumbnail, Aether, Ember Batch, Aura Surface, Nova HTTP SSRF-Safe Adapter, Atlas/Pulse/Glyph) | Ekosistem & Web | PLANLANDI |
| **v0.9.8** | **F14** | Portability (`wasm32-wasi`, `wasm32-freestanding`, `--profile freestanding` Saf Piksel Çekirdeği, Mobil Link/ABI Denetimi) | Taşınabilirlik | PLANLANDI |
| **v0.9.9** | **F15** | Security, Fuzzing, Differential Benchmarks & Release Candidate (10k+ Corpus Fuzz, Pillow/Sharp Kıyas Raporu, SBOM, 24h Stress Planı) | RC Hardening | PLANLANDI |
| **v1.0.0** | **F16** | Prism 1.0 Stable Release (Donmuş `prism.nox` SemVer Sözleşmesi, 10+ Derlenen Gerçek Dünya Örneği, Tam Dokümantasyon ve Release Artefaktları) | Kararlı 1.0 | PLANLANDI |

Bağımlılık sırası: `v0.0.1 (F0) -> v0.1.0 (F1) -> v0.2.0 (F2) -> v0.3.0 (F3) -> v0.4.0 (F4) -> v0.5.0 (F5) -> v0.6.0 (F6) -> v0.7.0 (F7) -> v0.8.0 (F8) -> v0.8.5 (F9) -> v0.9.0 (F10) -> v0.9.2 (F11) -> v0.9.4 (F12) -> v0.9.6 (F13) -> v0.9.8 (F14) -> v0.9.9 (F15) -> v1.0.0 (F16)`.

---

## 3. Faz ve Sürüm Detaylı İş Planı

---

### Faz F0 — `v0.0.1`: Discovery, NOX Audit ve Repo Temeli

**Hedef:** Gerçek `noxc 2.2.38` araç zincirinin, bellek/buffer/dosya/thread semantiğinin ve paket iskeletinin derlenen probe'larla doğrulanması.

#### F0.1 — Depo Envanteri ve Temel İskelet
- [x] Çalışma dizinini (`/Users/melihburakmemis/Documents/nox-packages/prism`) incele; `PRISM_AGENTS.md` dosyasını koru ve `AGENTS.md` olarak da kök dizinde bulundur.
- [x] `nox.json` paket manifestini oluştur (`name: "prism"`, `version: "0.1.0"`, `source: "nox"`, `entry: "prism.nox"`, `requires: []`).
- [x] `.gitignore`, `LICENSE` (MIT), `README.md`, `SECURITY.md`, `CHANGELOG.md`, `CONTRIBUTING.md` dosyalarını gerçek durumla uyumlu olarak oluştur.
- [x] Başlangıç CI iş akışlarını (`.github/workflows/ci.yml`, `.github/workflows/security.yml`) macOS, Linux ve Windows hedefleri için yapılandır.

#### F0.2 — NOX 2.2.38 Capability Probe (`AGENTS.md` §2.2)
- [x] `examples/capability_probe.nox` içinde aşağıdaki yetenekleri derleyip LLVM ve QBE üzerinde çalıştırarak doğrula:
  - [x] 16 MiB ve 64 MiB `nox.buffer.Buffer` tahsisi, `Span` kesit erişimi, kopyalama ve sınır (`IndexError`) davranışı.
  - [x] `list[u8]` / `Buffer` / `str` (`nox.bytes.from_str`, `nox.bytes.to_str`, `nox.bytes.str_from_list`) arasında `\x00` (NUL) ve geçersiz UTF-8 (`0x89, 0xFF, 0xD8, 0xFE`) içeren binary round-trip.
  - [x] `nox.binary.BinaryReader` ve `BinaryWriter` ile little-endian ve big-endian (`u8`, `u16`, `u32`, `u64`) okuma/yazma.
  - [x] 64-bit işaretli `int` taşma (wrap-around) davranışına karşı Prism `checked_add`, `checked_sub`, `checked_mul` doğrulaması.
  - [x] `nox.fs` ile chunked binary okuma/yazma (`open_write`, `open_read`, `read_chunk`, `write_string_atomic`) ve `nox.path.safe_join` ile path traversal engelleme.
  - [x] `spawn` ve `Task` ile paralel tile/satır işlerinin deterministik birleştirme sırası ve `--strict-spawn-shared` denetimi.
  - [x] `lowlevel` içinde `ptr_of(list[u8])` ve `extern def` C ABI (`memset`) erişimi.
  - [x] `--profile freestanding` altında saf piksel/matematik modüllerinin (`core/*.nox`) derlenebilirliği.

#### F0.3 — Mimari Kararlar ve Başlangıç Dokümantasyonu
- [x] `docs/architecture.md`: Katmanlar, bağımlılık yönü ve ADR-001 (Saf NOX çekirdek + izole opsiyonel native codec stratejisi).
- [x] `docs/capabilities.md`: Doğrulanan `noxc 2.2.38` yetenekleri, codec durumu ve platform matrisi.
- [x] `docs/nox-compatibility.md`: `noxc 2.2.38` probe sonuçları, LLVM vs QBE notları, `--profile freestanding` ve `--strict-spawn-shared` bulguları.
- [x] `docs/roadmap.md`: F0–F16 canlı durum tablosu (`feature`, `status`, `noxc_version`, `target_platforms`, `tests`, `benchmarks`, `known_limits`, `date`).
- [x] `docs/security.md`: Threat model (§20), `ResourceLimits` varsayılanları (§6.4), decompression bomb ve path traversal önlemleri.
- [x] `tests/fixtures-manifest.json` ve `benchmarks/perf_baseline.json` başlangıç kayıtlarını oluştur.

#### Faz F0 (`v0.0.1`) Çıkış Kapısı
- [x] `noxc check` ve `noxc test` örnekleri LLVM ve QBE üzerinde geçiyor.
- [x] Desteklenen/eksik hedef platformlar `docs/capabilities.md` ve `docs/nox-compatibility.md` içinde raporlandı.
- [x] Native codec sınırları ve core bağımsızlığı ADR-001 ile belgelendi.
- [x] Başlangıç perf/memory baseline ve `tests/fixtures-manifest.json` hazır.

---

### Faz F1 — `v0.1.0`: Safe Pixel Core, Image Model ve İlk Dikey Dilim (§29)

**Hedef:** Piksel modeli güvenli, sınır ve taşma kontrollü, yan etkisiz `Image` / kontrollü `MutableImage` / `ImageView` ayrımına sahip ve gerçek PPM P6 çıktısı üretebilen çekirdek.

#### F1.1 — Tipli Hata Hiyerarşisi (`core/errors.nox`)
- [x] `PrismError(Exception)` kök hata sınıfı (`kind: str`).
- [x] Alt sınıflar (§19):
  - [x] `InvalidImageError`
  - [x] `CorruptDataError`
  - [x] `UnsupportedFormatError`
  - [x] `CodecUnavailableError`
  - [x] `UnsupportedPixelFormatError`
  - [x] `ColorProfileError`
  - [x] `ResourceLimitError`
  - [x] `DimensionError`
  - [x] `DecodeError`
  - [x] `EncodeError`
  - [x] `ImageIOError`
  - [x] `PipelineError`
  - [x] `NativeAdapterError`
- [x] `__all__` bildirimi ve tüm hata sınıflarının yakalama (`except PrismError:`) testleri.

#### F1.2 — Kaynak Bütçesi ve Checked Arithmetic (`core/limits.nox`)
- [x] 64-bit taşma korumalı aritmetik yardımcıları:
  - [x] `checked_add(a: int, b: int) -> int`
  - [x] `checked_sub(a: int, b: int) -> int`
  - [x] `checked_mul(a: int, b: int) -> int`
  - [x] `checked_stride(width: int, bytes_per_pixel: int, custom_stride: int) -> int`
  - [x] `checked_buffer_size(width: int, height: int, stride: int, bytes_per_pixel: int) -> int`
- [x] `ResourceLimits` sınıfı (§6.4 varsayılanlarıyla: `max_width=32768`, `max_height=32768`, `max_pixels=100_000_000`, `max_decoded_bytes=536_870_912`, `max_input_bytes=268_435_456`, `max_frames=200`, `max_metadata_bytes=4_194_304`, `max_chunk_bytes=67_108_864`, `max_recursion_depth=32`, `max_output_bytes=536_870_912`, `max_temp_bytes=1_073_741_824`, `max_work_ms=30_000`).
- [x] `ResourceLimits.default()`, `ResourceLimits.strict_server()`, `ResourceLimits.constrained_wasm()` profilleri ve `validate_dimensions(width, height, bpp)` doğrulaması.

#### F1.3 — Piksel, Renk, Metadata ve Seçenek Modelleri (`core/pixel.nox`, `core/color.nox`, `core/metadata.nox`, `core/options.nox`)
- [x] `PixelFormat` enum (`GRAY8`, `RGB8`, `RGBA8`, `GRAY16`, `RGB16`, `RGBA16`, `RGBAF32`, `INDEXED8`) ve `channels()`, `bytes_per_pixel()`, `is_supported_core()` metotları.
- [x] `ColorSpace` enum (`SRGB`, `LINEAR_SRGB`, `DISPLAY_P3`, `UNKNOWN`, `ICC_PROFILE`) ve `AlphaMode` enum (`OPAQUE`, `STRAIGHT`, `PREMULTIPLIED`).
- [x] `Pixel` sınıfı (`r: int`, `g: int`, `b: int`, `a: int`), `@staticmethod` kurucular (`Pixel.rgba`, `Pixel.rgb`, `Pixel.gray`, `Pixel.transparent`, `Pixel.black`, `Pixel.white`), `0..255` aralık doğrulaması ve luma dönüşümü.
- [x] `ImageMetadata` ve `ImageInfo` modelleri (`width`, `height`, `pixel_format`, `color_space`, `alpha_mode`, `orientation`, `frame_count`, `format_name`).
- [x] `DecodeOptions` ve `EncodeOptions` tipli seçenek sınıfları.

#### F1.4 — Bellek Saklama ve Görüntü Modeli (`core/storage.nox`, `core/image.nox`)
- [x] `PixelStorage` (`Buffer` tabanlı, checked stride ve offset hesaplı, copy-on-write / clone destekli güvenli saklama katmanı).
- [x] `Image` (immutable kamu modeli):
  - [x] `width > 0`, `height > 0`, `stride >= width * bpp`, `buffer_len >= (height - 1) * stride + width * bpp` invariant kontrolü.
  - [x] `get_pixel(x, y) -> Pixel`, `clone() -> Image`, `to_mutable() -> MutableImage`, `crop_view(x, y, w, h) -> ImageView`, `crop(x, y, w, h) -> Image`, `convert_format(target_fmt) -> Image`, `raw_bytes() -> Buffer` (kopya döndürür; iç mutable buffer'ı dışarı sızdırmaz).
- [x] `MutableImage` (kontrollü mutasyon modeli):
  - [x] `set_pixel(x, y, px)`, `get_pixel(x, y) -> Pixel`, `fill(px)`, `fill_rect(x, y, w, h, px)`, `to_image() -> Image` (bağımsız snapshot), `clone() -> MutableImage`.
- [x] `ImageView` (sıfır kopya pencere modeli):
  - [x] Parent `PixelStorage` referansını canlı tutar, checked sub-rectangle offset hesabı yapar (`get_pixel(x, y)`, `sub_view(x, y, w, h)`, `to_image(lim) -> Image`).

#### F1.5 — Minimal PPM/PGM Çıktı ve Kararlı Facade (`codecs/ppm.nox`, `codecs/pgm.nox`, `prism.nox`)
- [x] `codecs/ppm.nox` ve `codecs/pgm.nox`: PPM P6 (binary RGB8) ve PGM P5 (binary GRAY8) encode/decode/probe + dosya/bellek yardımcıları (`encode_ppm`, `decode_ppm`, `probe_ppm`, `save_ppm`, `open_ppm`, `encode_pgm`, `decode_pgm`, `probe_pgm`, `save_pgm`, `open_pgm`).
- [x] `prism.nox` kararlı facade: yalnızca gerçekten uygulanmış ve test edilmiş F1 sembollerini dışa aktarır.
- [x] `examples/create_checkerboard.nox`: §29.7 gereği dama tahtası görüntüsü oluşturup PPM P6 olarak kaydeder, geri okuyarak piksel doğrulaması yapar ve `tests/fixtures/` altındaki sentetik fixture'ları üretir.

#### F1.6 — Testler (Normal, 26 Negatif, 1000 Rastgele Property)
- [x] `tests/unit/core_pixel_test.nox` & `tests/unit/facade_test.nox`:
  - [x] `Image(2, 2)` RGBA8 oluşturma, 4 pikseli set/read, clone, `ImageView` crop ve `to_image` doğrulaması (§29.4).
  - [x] `GRAY8`, `RGB8`, `RGBA8` dönüşümleri ve `Pixel` aralık kontrolleri.
  - [x] **26 negatif güvenlik ve sınır testi (§29.6 en az 20 şartını karşılar):**
    1. `width == 0` reddi (`DimensionError`)
    2. `height == 0` reddi (`DimensionError`)
    3. `width < 0` reddi (`DimensionError`)
    4. `height < 0` reddi (`DimensionError`)
    5. `width > limits.max_width` reddi (`ResourceLimitError`)
    6. `height > limits.max_height` reddi (`ResourceLimitError`)
    7. `width * height > limits.max_pixels` görüntü bombası reddi (`ResourceLimitError`)
    8. `decoded_bytes > limits.max_decoded_bytes` bellek bütçesi reddi (`ResourceLimitError`)
    9. `checked_mul` int64 max (`9223372036854775807`) taşma reddi (`ResourceLimitError`)
    10. `checked_add` int64 max taşma reddi (`ResourceLimitError`)
    11. `checked_sub` underflow reddi (`DimensionError`)
    12. `get_pixel(-1, 0)` negatif X sınır reddi (`DimensionError`)
    13. `get_pixel(0, -1)` negatif Y sınır reddi (`DimensionError`)
    14. `get_pixel(width, 0)` üst X sınır reddi (`DimensionError`)
    15. `get_pixel(0, height)` üst Y sınır reddi (`DimensionError`)
    16. `set_pixel` sınır dışı koordinat reddi (`DimensionError`)
    17. `Pixel.rgba(-1, 0, 0, 255)` negatif kanal taşma reddi (`InvalidImageError`)
    18. `Pixel.rgba(0, 256, 0, 255)` `> 255` kanal taşma reddi (`InvalidImageError`)
    19. Geçersiz/küçük `stride < width * bpp` reddi (`DimensionError`)
    20. Yetersiz uzunlukta ham `Buffer` ile `Image.from_buffer` oluşturma reddi (`InvalidImageError`)
    21. `crop_view` görüntü sınırını aşan dikdörtgen reddi (`DimensionError`)
    22. `crop_view` sıfır genişlik/yükseklik reddi (`DimensionError`)
    23. `MutableImage` alias/snapshot izolasyonu (`to_image()` sonrası mutasyon) ve `raw_bytes()` mutasyon izolasyonu
    24. Desteklenmeyen piksel formatı (`RGBAF32` erken fazda) için `UnsupportedPixelFormatError`
    25. Kesik PPM P6 veri boyutu uyuşmazlığı için `CorruptDataError`
    26. Desteklenmeyen ASCII PPM P3 varyantı için `UnsupportedFormatError`
  - [x] **1000 rastgele küçük görüntü constructor / get / set / clone / view property testi (F1 kabul kriteri).**

#### Faz F1 (`v0.1.0`) Çıkış Kapısı
- [x] Geçerli piksel erişimi exact; tüm geçersiz indeksler tipli hata veriyor.
- [x] Overflow ve aşırı büyük tahsisler belleğe dokunmadan önce reddediliyor.
- [x] `ImageView` parent yaşam süresi, `MutableImage` aliasing ve copy davranışı testli.
- [x] 1000 rastgele küçük görüntü testi ve 26 negatif test LLVM + QBE üzerinde geçiyor.
- [x] `examples/create_checkerboard.nox` ve `examples/capability_probe.nox` derlenip çalışıyor.

---

### Faz F2 — `v0.2.0`: Portable Image I/O ve Minimal Codec Suite

**Hedef:** Saf NOX görüntü açma/kaydetme API'sinin PPM/PGM (P5/P6) ve BMP (24/32-bit uncompressed) formatlarında tam güvenlik kontratıyla çalışması.

#### F2.1 — Format Sniffing ve Codec Registry (`codecs/sniff.nox`, `codecs/registry.nox`)
- [x] Uzantıdan bağımsız magic-byte ve header doğrulamalı format tespiti (`PPM_P6`, `PGM_P5`, `PPM_P3`, `PGM_P2`, `BMP`, `PNG`, `JPEG`, `WEBP`, `GIF`, `TIFF`, `UNKNOWN`).
- [x] `probe(data, options) -> ImageInfo` ve `probe_file(path, options) -> ImageInfo`: piksel verisini tamamen decode etmeden boyut, kanal ve limit doğrulaması.
- [x] Tanınan ama henüz aktif olmayan formatlar için `UnsupportedFormatError` / `CodecUnavailableError` ayrımı.

#### F2.2 — PPM / PGM ve BMP Codec'leri (`codecs/ppm.nox`, `codecs/pgm.nox`, `codecs/bmp.nox`)
- [x] Netpbm P5 (PGM binary 8-bit) ve P6 (PPM binary 8-bit) tam header parser (yorum satırları `#...`, whitespace, `maxval == 255` kontrolü, limit denetimi).
- [x] BMP 24-bit (BGR -> RGB8) ve 32-bit (BGRA -> RGBA8) uncompressed (`BI_RGB` / `BITMAPINFOHEADER` 40 byte ve `BITMAPV4/V5` temel) decode ve encode.
- [x] BMP row padding (4-byte sınırına hizalama), bottom-up (`height > 0`) ve top-down (`height < 0`) satır sırası desteği.
- [x] BMP RLE veya garip bit-depth varyantları için güvenli `UnsupportedFormatError` / `UnsupportedPixelFormatError`, bozuk offset/boyut için `CorruptDataError`.

#### F2.3 — Güvenli Dosya ve Bellek Adaptörleri (`adapters/memory.nox`, `adapters/file.nox`, `codecs/reader.nox`, `codecs/writer.nox`)
- [x] `decode(data: Buffer, options: DecodeOptions) -> Image` ve `encode(img: Image, format: str, options: EncodeOptions) -> Buffer`.
- [x] `open_image(path: str, options: DecodeOptions) -> Image`, `save_image(img: Image, path: str, options: EncodeOptions) -> None` ve `save_image_as(img: Image, path: str, format_name: str, options: EncodeOptions) -> None`.
- [x] `nox.path.safe_join` destekli `open_image_confined(root, rel_path, options)` ve `save_image_confined(img, root, rel_path, options)`.
- [x] Varsayılan `no-clobber` veya atomik yazma (`write_string_atomic`) politikası; başarısız encode durumunda mevcut hedef dosyanın bozulmaması (§21.3).

#### F2.4 — Fuzz, Corrupt Fixture ve Round-Trip Testleri (`tests/codecs/portable_codecs_test.nox`, `tests/fuzz/header_fuzz_test.nox`)
- [x] PPM P6, PGM P5, BMP 24-bit ve BMP 32-bit lossless encode/decode pixel-exact round-trip testleri.
- [x] Uzantısı yanıltıcı dosyalarda (`.jpg` uzantılı BMP) magic-byte sniff doğrulaması.
- [x] **10.000 sentetik bozuk/kesik/taşmalı header fuzz testi** (process crash veya yakalanmayan panic olmadan tipli `PrismError` ile geçiş).
- [x] Başarısız encode/save sırasında eski çıktı dosyasının hasarsız kaldığının doğrulanması.

#### Faz F2 (`v0.2.0`) Çıkış Kapısı
- [x] PPM, PGM, BMP encode/decode lossless round-trip pixel-exact.
- [x] Format sniff dosya uzantısından bağımsız çalışıyor.
- [x] 10.000 sentetik malformed header fuzz testi crash olmadan geçiyor.
- [x] LLVM ve QBE üzerinde tüm I/O ve codec testleri yeşil.

---

### Faz F3 — `v0.3.0`: Geometry ve Resampling Engine

**Hedef:** Yaygın `crop`, `pad`, `flip`, `transpose`, `rotate` (90/180/270 ve keyfi açı), `resize` ve `thumbnail` işlemlerinin yüksek doğrulukla çalışması.

#### F3.1 — Exact Geometrik Dönüşümler (`transforms/crop.nox`, `transforms/border.nox`, `transforms/flip.nox`, `transforms/rotate.nox`, `transforms/geometry.nox`)
- [x] `crop(img, x, y, w, h)` ve bounds policy (`STRICT`, `CLIP`, `PAD`).
- [x] `pad(img, left, top, right, bottom, fill_pixel)` checked dimension ile.
- [x] `flip_horizontal(img)`, `flip_vertical(img)`, `transpose(img)`, `transverse(img)`.
- [x] `rotate90(img)`, `rotate180(img)`, `rotate270(img)` exact fast-path ve EXIF orientation (1–8) normalizasyon fonksiyonu.
- [x] Keyfi açılı `rotate(img, angle_deg, expand, background_pixel, filter)` ve 2x3 `affine(img, matrix, out_w, out_h, background_pixel, filter)`.

#### F3.2 — Resampling Algoritmaları (`transforms/resample.nox`, `transforms/resize.nox`)
- [x] `ResampleFilter` enum: `NEAREST`, `BILINEAR`, `BICUBIC`, `LANCZOS2`, `LANCZOS3`, `AREA`.
- [x] `x + 0.5`, `y + 0.5` piksel merkezi sözleşmesi, yüksek doğruluklu filtreleme.
- [x] Alpha bleeding (halo) önleyici premultiplied-alpha / gamma-aware yüksek kalite yolu + hızlı (`fast_srgb`) mod seçeneği.
- [x] `ResizeFit` modları: `FILL` (exact), `CONTAIN`, `COVER`, `INSIDE`, `OUTSIDE` ve `thumbnail(img, max_w, max_h, filter)`.
- [x] NaN/Inf/sıfır/negatif boyut ve ölçek parametrelerinin reddi (`DimensionError`).

#### F3.3 — Geometri ve Resampling Testleri (`tests/unit/geometry_test.nox`, `tests/golden/resample_golden_test.nox`)
- [x] Metamorfik özellik testleri:
  - [x] `flip_horizontal(flip_horizontal(img)) == img`
  - [x] `flip_vertical(flip_vertical(img)) == img`
  - [x] `rotate90` x 4 == `img`
  - [x] `resize(img, img.width, img.height, NEAREST) == img`
  - [x] `crop` alt bölge piksellerinin birebir korunması
- [x] 1x1, 1xN, Nx1, tekil boyutlar (odd dimensions) ve EXIF orientation 1–8 testleri.
- [x] Şeffaf kenarlarda (`alpha == 0` komşuluğu) bilinear/bicubic/Lanczos/Area alpha-halo regresyon testi.
- [x] ≥100 golden geometri/resampling test vakası (120 golden vaka).

#### Faz F3 (`v0.3.0`) Çıkış Kapısı
- [x] Exact geometri property testleri ve ≥100 golden fixture geçiyor.
- [x] Referans resampling çıktıları açık toleransla doğrulanıyor.
- [x] 8-bit alpha halo ve gamma regresyon testleri geçiyor.
- [x] Büyük görüntü resize işlemlerinde `ResourceLimits` bellek bütçesi devrede.

---

### Faz F4 — `v0.4.0`: Saf NOX PNG Engine (8-bit)

**Hedef:** Harici C bağımlılığı olmadan her platformda (macOS/Linux/Windows/WASM) çalışan, güvenli 8-bit PNG decode ve encode motoru.

#### F4.1 — Zlib (RFC 1950), DEFLATE (RFC 1951), CRC32 ve Adler32 (`codecs/deflate.nox`)
- [x] IEEE 802.3 `crc32` ve RFC 1950 `adler32` bütünlük hesaplayıcıları.
- [x] RFC 1950 Zlib container ayrıştırıcı/üretici (`CMF`, `FLG`, `FDICT` kontrolü, Adler32 doğrulaması).
- [x] RFC 1951 DEFLATE decompressor (stored block type 0, fixed Huffman type 1, dynamic Huffman type 2) + decompression bomb (maksimum çıktı byte sınırı ve genişleme oranı) koruması.
- [x] RFC 1951 DEFLATE compressor (geçerli zlib/DEFLATE akışı üreten, LZ77+RLE+fixed Huffman destekli encoder).

#### F4.2 — PNG Chunk Parser, Scanline Unfilter/Filter ve Codec (`codecs/png.nox`)
- [x] 8-byte PNG imzası (`\x89PNG\r\n\x1a\n`), chunk length, chunk type, CRC32 ve zorunlu sıralama (`IHDR` -> `PLTE`/`tRNS` -> `IDAT`+ -> `IEND`) doğrulaması.
- [x] 8-bit Color Type desteği:
  - [x] Color Type 0: Grayscale (1/2/4/8-bit) + opsiyonel `tRNS`
  - [x] Color Type 2: Truecolor RGB (8-bit) + opsiyonel `tRNS`
  - [x] Color Type 3: Indexed-color Palette (`PLTE`, 1/2/4/8-bit packed indeks + `tRNS` alpha tablosu -> `RGBA8`/`RGB8`)
  - [x] Color Type 4: Grayscale + Alpha (8-bit -> `RGBA8`)
  - [x] Color Type 6: Truecolor RGBA (8-bit -> `RGBA8`)
- [x] Scanline filtreleri 0–4 (`None`, `Sub`, `Up`, `Average`, `Paeth`) decode ve adaptive encode.
- [x] Ancillary chunk (`gAMA`, `sRGB`, `iCCP`, `tEXt`, `eXIf`) boyut limiti ve taşıma/temizleme politikası.
- [x] Henüz kapsam dışı varyantlar (16-bit veya Adam7 interlace, F9/F12 öncesi) için açık `UnsupportedPixelFormatError` / `UnsupportedFormatError`.

#### F4.3 — PNG Doğruluk, Güvenlik ve Fuzz Testleri (`tests/codecs/png_test.nox`)
- [x] Tüm 5 renk tipinde (0, 2, 3, 4, 6) ve 5 scanline filtresinde (0–4) pixel-exact decode ve lossless round-trip testleri.
- [x] Bozuk CRC, kesik `IDAT`, eksik `IEND`, geçersiz `IHDR` boyutu, bozuk Huffman ağacı ve decompression bomb (küçük IDAT -> bütçe aşımı) negatif testleri.
- [x] `examples/thumbnails.nox` ve `examples/avatar.nox` örneklerinin PNG girdi/çıktı ile çalışması ve üretilen PNG'nin harici `sips`/`file` okuyucusuyla doğrulanması.

#### Faz F4 (`v0.4.0`) Çıkış Kapısı
- [x] PNG fixture'ları ile exact pixel decode ve round-trip geçiyor.
- [x] CRC, chunk overflow ve decompression-bomb testleri geçiyor.
- [x] Üretilen PNG dosyaları standart PNG spesifikasyonuna (RFC 2083) tam uyumlu ve harici okuyucular (`sips`, `file`) tarafından açılıyor.
- [x] Desteklenmeyen 16-bit/Adam7 varyantları doğru tipli hatayı döndürüyor.

---

### Faz F5 — `v0.5.0`: Color, Alpha ve Metadata Hygiene

**Hedef:** Doğru renk uzayı dönüşümleri, düz/premultiplied alpha matematiği ve gizlilik odaklı EXIF/ICC metadata yönetimi.

#### F5.1 — Renk Uzayı ve Alpha Dönüşümleri (`core/color.nox`)
- [x] `sRGB` <-> `LinearSRGB` dönüşüm fonksiyonları (`convert_color_space`, `blend_source_over_linear`, `blend_images_source_over`).
- [x] `Straight` <-> `Premultiplied` alpha dönüşümü (`image_to_premultiplied`, `image_to_straight`), integer yuvarlama doğruluğu ve `alpha == 0` durumlarında semantik koruma.
- [x] Renk profili olmayan girdiler için açık `assume_srgb` politikası; ICC dönüştürme motoru olmayan derlemelerde yanıltıcı geniş-gamut dönüşüm iddiasının engellenmesi (`ColorProfileError`).

#### F5.2 — EXIF Orientation ve Gizli Metadata Temizliği (`core/metadata.nox`)
- [x] EXIF (TIFF header + IFD0/ExifIFD/GPSIFD) güvenli, derinlik ve boyut limitli okuyucu/temizleyici (`parse_exif_buffer`, `sanitize_exif_buffer`).
- [x] EXIF Orientation 1–8 okuma, `autorotate` uygulandığında pikselleri döndürüp orientation etiketini `1` (Top-Left) olarak sıfırlama (çift döndürme engeli).
- [x] `strip_private_metadata`: GPS koordinatları, kamera seri numarası (`0xA431`), `MakerNote` (`0x927C`) ve kullanıcı yorumlarını temizlerken güvenli renk/boyut bilgisini koruma.
- [x] ICC profil byte'larını boyut limiti (`max_metadata_bytes`) altında güvenli taşıma (`ImageMetadata.set_icc_profile`).

#### F5.3 — Renk ve Metadata Testleri (`tests/unit/color_metadata_test.nox`)
- [x] Alpha 0/255, yarı saydamlık ve lineer ışık harmanlama golden testleri.
- [x] EXIF orientation 1–8 round-trip (ikinci kez encode/decode çağrıldığında görüntünün tekrar dönmemesi).
- [x] Sentetik GPS ve kamera seri numarası içeren EXIF bloklarının `sanitize_exif_buffer` ve `strip_private_metadata` sonrası tamamen silindiğinin byte düzeyinde doğrulanması.
- [x] Döngüsel (cyclic IFD pointer) ve aşırı büyük EXIF/ICC bloklarının `CorruptDataError` / `ResourceLimitError` ile reddedilmesi.

#### Faz F5 (`v0.5.0`) Çıkış Kapısı
- [x] Alpha 0/255, translucency ve lineer blending testleri geçiyor.
- [x] EXIF orientation round-trip double-rotate yapmıyor.
- [x] GPS ve özel metadata temizliği fiilen doğrulanmış.
- [x] ICC dönüştürme yeteneği olmayan derleme bunu dürüstçe sınırlandırıyor.

---

### Faz F6 — `v0.6.0`: Filters, Enhancement ve Pixel Kernels

**Hedef:** Pillow seviyesinde yaygın görüntü iyileştirme, konvolüsyon, bulanıklaştırma, kenar bulma, morfoloji ve görüntü kalite/istatistik metrikleri.

#### F6.1 — Noktasal İyileştirme ve Renk Ayarları (`filters/enhance.nox`, `filters/effects.nox`)
- [x] `grayscale(img, mode)` (sRGB perceptual luma BT.709 vs Linear luminance ayrımı açık).
- [x] `invert`, `threshold`, `posterize`, `solarize`, `sepia`, `pixelate`, `vignette`.
- [x] `brightness`, `contrast`, `saturation`, `gamma_correct`, `apply_lut`.
- [x] Kanal ayırma ve birleştirme (`split_channels`, `merge_channels`).

#### F6.2 — Konvolüsyon, Blur, Kenar ve Morfoloji (`filters/convolution.nox`, `filters/gaussian.nox`, `filters/sharpen.nox`, `filters/edges.nox`, `filters/median.nox`, `filters/morphology.nox`)
- [x] Genel `convolve(img, kernel, kw, kh, divisor, bias, border_mode)` (`CLAMP`, `MIRROR`, `WRAP`, `CONSTANT`).
- [x] `box_blur`, separable `gaussian_blur(img, sigma, radius, border_mode)`, `sharpen`, `unsharp_mask(img, sigma, amount, threshold)`.
- [x] `sobel`, `prewitt`, `laplacian`, `emboss`.
- [x] `median_filter(img, radius)`, `erode`, `dilate`, `morphology_open`, `morphology_close`.
- [x] `sigma <= 0`, NaN/Inf, çift veya limit üstü (`MAX_KERNEL_SIZE`, `MAX_BLUR_RADIUS`, `MAX_MEDIAN_RADIUS`, `MAX_MORPH_RADIUS`) kernel boyutları için tipli hata.

#### F6.3 — İstatistik ve Kalite Analizi API'si (`filters/effects.nox`)
- [x] Kanal histogramları (256 bin), `min`, `max`, `mean`, `stddev`, `entropy`, `histogram_equalize` (`ChannelStats`, `channel_histogram`, `compute_channel_stats`).
- [x] Görüntü karşılaştırma metrikleri: `image_mse(a, b)`, `image_mae(a, b)`, `image_psnr(a, b)`, `image_ssim(a, b)`.
- [x] Algısal hash (`average_hash`, `difference_hash` — kriptografik olmayan benzerlik özeti olarak belgeli).

#### Faz F6 (`v0.6.0`) Çıkış Kapısı
- [x] Tüm filtreler için referans golden ve sınır testleri geçiyor (`tests/unit/filters_test.nox`).
- [x] Geçersiz `sigma`, `radius`, `NaN`/`Inf` ve bozuk kernel matrisleri tipli hata veriyor.
- [x] Şeffaf kenarlarda ve sınır kiplerinde artefakt oluşmuyor.
- [x] Blur/convolution benchmark baseline'ı kaydedilmiş (`benchmarks/perf_baseline.json`).

---

### Faz F7 — `v0.7.0`: Drawing, Text, Masks ve Compositing

**Hedef:** Programatik vektör/raster şekil çizimi, katman harmanlama, maskeleme ve gömülü lisanslı bitmap font ile deterministik metin üretimi.

#### F7.1 — Compositing, Mask ve Blend Motoru (`composite/alpha.nox`, `composite/blend.nox`, `composite/masks.nox`, `composite/layers.nox`)
- [x] Porter–Duff kipleri: `SOURCE_OVER`, `DESTINATION_OVER`, `SOURCE_IN`, `SOURCE_OUT`, `XOR`, `PLUS`.
- [x] Blend modları: `NORMAL`, `MULTIPLY`, `SCREEN`, `OVERLAY`, `DARKEN`, `LIGHTEN`, `DIFFERENCE`.
- [x] 8-bit `GRAY8` maske ile `apply_mask` ve `masked_composite` (boyut uyuşmazlığında açık `DimensionError`).
- [x] Çok katmanlı `Layer` (`offset_x`, `offset_y`, `opacity`, `blend_mode`, `visible`) kompozisyonu (`paste_image`, `composite_layers`).

#### F7.2 — Şekil Çizimi ve Rasterizer (`draw/canvas.nox`, `draw/shapes.nox`, `draw/paths.nox`, `draw/rasterizer.nox`)
- [x] `Canvas` üzerinde `draw_line`, `draw_polyline`, `draw_rect`, `fill_rect`, `draw_rounded_rect`, `fill_rounded_rect`, `draw_circle`, `fill_circle`, `draw_ellipse`, `fill_ellipse`, `draw_polygon`, `fill_polygon`, `draw_quadratic_bezier`, `draw_cubic_bezier`.
- [x] Clipping dikdörtgeni (`set_clip_rect`, `reset_clip`), stroke kalınlığı, anti-aliased Xiaolin Wu çizgi seçeneği (`draw_line_aa`).
- [x] Kaynak bütçeli, **iteratif** (stack overflow üretmeyen) `flood_fill(canvas, x, y, fill_pixel, tolerance)`.

#### F7.3 — Gömülü Bitmap Font ve Metin (`draw/text.nox`)
- [x] Kamu malı / MIT uyumlu gömülü 8x8 ASCII bitmap font.
- [x] `measure_text(text, scale, spacing, line_spacing) -> TextMetrics` ve `draw_text(canvas, x, y, text, color, scale, spacing, line_spacing, align)` aynı yerleşim motorunu kullanır.
- [x] Çok satırlı metin, hizalama (`LEFT`, `CENTER`, `RIGHT`) ve sınır kutusu hesaplama.

#### Faz F7 (`v0.7.0`) Çıkış Kapısı
- [x] Blend ve alpha edge golden testleri geçiyor (`tests/unit/draw_composite_test.nox`).
- [x] Şekil geometrisi, clipping ve iteratif flood-fill sınır testleri geçiyor.
- [x] Metin çıktısı deterministik ve `measure_text` ile piksel-exact uyumlu.
- [x] Çizim hataları kaynak `Image` nesnesini bozmuyor.

---

### Faz F8 — `v0.8.0`: Native Codec Adapters (JPEG, WebP)

**Hedef:** Çekirdeği native bağımlılığa mahkûm etmeden, opt-in C ABI sınırı üzerinden JPEG ve WebP desteğinin güvenli entegrasyonu.

#### F8.1 — Native Adapter Sınırı ve Fallback Mimarisi (`native/`, `codecs/jpeg_adapter.nox`, `codecs/webp_adapter.nox`, `codecs/tiff_adapter.nox`)
- [x] `native/README.md`, `native/include/prism_native_abi.h` ve C ABI güvenlik sözleşmesi (sabit boyutlu hata kodu/mesaj yapısı, `ResourceLimits` ön kontrolü, codec-özel deallocator eşleşmesi).
- [x] Native adapter etkin değilken (`source: "nox"` varsayılan derlemede) JPEG/WebP/TIFF çağrılarının derleme hatası vermeden saf NOX header probing (`probe_jpeg`, `probe_webp`, `probe_tiff`) sonrası `CodecUnavailableError` fırlatması ve açık capability bayrağıyla sunulması.
- [x] `EncodeOptions` içinde `quality (1..100)`, `lossless`, `JpegSubsampling`, `strip_private_metadata` seçeneklerinin doğrulanması (`EncodeOptions.with_quality`, `jpeg_default`, `webp_default`).
- [x] `docs/native-adapters.md` içinde `libjpeg-turbo` (BSD-3/IJG), `libwebp` (BSD-3) ve `libtiff` (BSD-like) lisans, ABI, CVE ve platform dağıtım matrisi.

#### Faz F8 (`v0.8.0`) Çıkış Kapısı
- [x] Native codec olmadan core derleniyor ve tüm taşınabilir formatları (PPM/PGM/BMP/PNG) işliyor.
- [x] Adapter arayüzü ve `CodecUnavailableError` / opt-in C ABI köprüsü corruption ve limit testlerini geçiyor (`tests/codecs/native_adapters_test.nox`).
- [x] Paket `source` etiketi ve üçüncü taraf lisans yükümlülükleri eksiksiz belgeli.

---

### Faz F9 — `v0.8.5`: Animation ve Çok Kareli Görüntüler

**Hedef:** `Frame` ve `Animation` veri modeliyle GIF (still + animated) ve çok kareli görüntülerin kaynak bütçesi altında güvenli işlenmesi.

#### F9.1 — Animasyon Veri Modeli ve GIF Motoru (`animation/frame.nox`, `animation/timeline.nox`, `animation/gif.nox`, `codecs/gif.nox`)
- [x] `DisposalMethod` (`NONE`, `KEEP`, `RESTORE_BACKGROUND`, `RESTORE_PREVIOUS`) ve `FrameBlend` (`SOURCE`, `OVER`) enum'ları.
- [x] `Frame(img, duration_ms, offset_x, offset_y, disposal, blend)` ve `Animation(canvas_width, canvas_height, frames, loop_count, background, metadata, lim)`.
- [x] GIF87a / GIF89a saf NOX decoder ve encoder:
  - [x] Global ve Local Color Table, Graphic Control Extension (delay, transparency index, disposal 0/1/2/3), Application Extension (`NETSCAPE2.0` loop count).
  - [x] Variable-length LZW decoder/encoder (`gif_lzw_decompress`, `gif_lzw_compress` — clear code, EOI, masa taşması ve bozuk kod akışı koruması).
  - [x] 0ms veya çok küçük delay değerleri için güvenli minimum süre normalizasyonu politikası (`normalize_frame_delay_ms`).
  - [x] `frames * canvas_width * canvas_height * 4 <= limits.max_decoded_bytes` ve `len(frames) <= limits.max_frames` toplam iş bütçesi kontrolü.
- [x] Tam kare kompozisyon motoru (`Timeline.render_frames()`, `render_gif_frames`): delta kareleri disposal kurallarına göre ana tuval üzerinde birleştirerek tam `Image` kareleri üretme.

#### Faz F9 (`v0.8.5`) Çıkış Kapısı
- [x] GIF disposal (`KEEP`, `RESTORE_BACKGROUND`, `RESTORE_PREVIOUS`) ve şeffaf kare golden testleri geçiyor (`tests/codecs/gif_animation_test.nox`).
- [x] 100+ kare (120 kare) bellek ve `max_frames` limit testi geçiyor.
- [x] Bozuk LZW akışı, taşan frame offset'i ve negatif/bozuk boyutlar `CorruptDataError` / `DimensionError` ile reddediliyor.

---

### Faz F10 — `v0.9.0`: Pipeline DAG, Planner ve Tile Engine

**Hedef:** Lazy (tembel) operasyon grafı, renk/alpha sırasını bozmayan güvenli plan optimizasyonu ve halo-doğru tile tabanlı yürütme motoru.

#### F10.1 — Immutable Pipeline DAG ve Planner (`pipeline/graph.nox`, `pipeline/nodes.nox`, `pipeline/planner.nox`)
- [x] Tipli `PipelineNode` türleri: `DECODE`, `CONVERT_COLOR`, `ORIENT`, `CROP`, `PAD`, `FLIP`, `ROTATE90`, `RESIZE`, `FILTER`, `COMPOSITE`, `DRAW`, `ENCODE` (`eval` veya keyfi string çalıştırma kesinlikle yok).
- [x] Güvenli plan optimizasyonu (`optimize_pipeline`):
  - [x] Ardışık `Crop` düğümlerinin tek `Crop` düğümüne birleştirilmesi.
  - [x] Ardışık `Flip` (`flip_h` + `flip_h` = identity) ve `Rotate90` (`90` + `90` = `180`) sadeleştirmesi.
  - [x] Ardışık aynı hedef `ConvertColor` düğümlerinin elenmesi ve `Invert+Invert` iptali (renk/alpha semantiğini değiştiren sıralama değişiklikleri yasak).
  - [x] Tam görüntü gerektiren işlemler için açık `requires_materialization` bayrağı.

#### F10.2 — Tile Executor, Halo Hesabı ve Cache (`pipeline/tiles.nox`, `pipeline/cache.nox`, `pipeline/executor.nox`)
- [x] Yapılandırılabilir tile boyutu (varsayılan `DEFAULT_TILE_SIZE = 256`), konvolüsyon/Gaussian blur/Sobel/median için otomatik halo (kenar payı) genişletme ve kesme (`compute_tile_grid`, `execute_tiled_node` — tile birleşim yerlerinde sıfır seam/arteakt).
- [x] İçerik CRC32 + boyut/format/renk uzayı + plan fingerprint + sürüm tabanlı deterministik cache key ve bellek bütçeli (`max_bytes`, LRU eviction) `PlanCache`.

#### Faz F10 (`v0.9.0`) Çıkış Kapısı
- [x] Eager ve lazy/tiled yürütme aynı görüntüyü bit-exact (`pixel_equals == True`) üretiyor.
- [x] Rastgele 1000 geçerli pipeline ifadesi karşılaştırma testini geçiyor (`tests/unit/pipeline_test.nox`).
- [x] Tile sınırlarında (seam/halo) sıfır artefakt golden testi geçiyor.
- [x] Cache boyut sınırı (`max_bytes`) ve LRU eviction politikası testli.

---

### Faz F11 — `v0.9.2`: Streaming, Large Image ve Batch Processing

**Hedef:** Bellek tavanını aşmayan chunk/scanline akışı, kontrollü temp disk spill ve `--strict-spawn-shared` uyumlu paralel batch/tile işleme.

#### F11.1 — Streaming Reader/Writer ve Disk Spill (`pipeline/streaming.nox`)
- [ ] `ImageReader` ve `ImageWriter` (`with` protokolü destekli, scanline/tile pencereli okuma/yazma).
- [ ] İzinli temp dizini altında (`nox.path.safe_join`), `limits.max_temp_bytes` kotalı `SpillManager`; iptal veya hata durumunda geçici dosyaların otomatik temizlenmesi.
- [ ] Atomik çıktı geçişi (partial/yarım kalmış çıktı dosyasının hedefi ezmemesi).

#### F11.2 — Paralel Tile ve Batch İşleme (`pipeline/parallel.nox`)
- [ ] NOX `spawn` ile ayrık (disjoint) tile/batch işlerinin paralel yürütülmesi ve tile koordinatına / iş indeksine göre deterministik sırada birleştirilmesi.
- [ ] `BatchProcessor` ve `BatchResult`: hata politikası (`FAIL_FAST` veya `COLLECT_ERRORS`), idempotency key (`input_hash + pipeline_fingerprint + version`) ve iş bazlı kaynak sınırı.
- [ ] `noxc check --strict-spawn-shared` doğrulaması.

#### Faz F11 (`v0.9.2`) Çıkış Kapısı
- [ ] Streaming senaryosu tüm dosyayı tek seferde kontrolsüz belleğe yüklemeden pencere bütçesiyle çalışıyor.
- [ ] Temp disk kotası, iptal ve hata sonrası temizlik testleri geçiyor.
- [ ] Paralel ve tek iş parçacıklı batch/tile çıktıları bit-exact eşit.

---

### Faz F12 — `v0.9.4`: Advanced Color/Type ve Profesyonel Özellikler

**Hedef:** 16-bit ve float32 piksel temsilleri, PNG 16-bit / Adam7 interlace, renk niceleme (quantization), dithering, CLAHE ve Canny kenar bulma.

#### F12.1 — Yüksek Bit Derinliği ve İleri Algoritmalar
- [ ] `GRAY16`, `RGB16`, `RGBA16` (16-bit kanal) ve `RGBAF32` (32-bit float kanal) dönüşüm ve yüksek doğruluklu işlem yüzeyi.
- [ ] PNG 16-bit (big-endian sample) ve Adam7 (7-pass interlace) decode desteği.
- [ ] Renk niceleme (palette quantization) ve Floyd–Steinberg / Ordered Bayer dithering (özellikle GIF ve 8-bit indexed PNG üretimi için).
- [ ] CLAHE (Contrast Limited Adaptive Histogram Equalization) ve Canny (Gaussian + Sobel gradyan + non-maximum suppression + hysteresis thresholding) kenar bulma.

#### Faz F12 (`v0.9.4`) Çıkış Kapısı
- [ ] 16-bit ve float32 sayısal referans testleri geçiyor.
- [ ] PNG 16-bit ve Adam7 interlaced fixture decode testleri geçiyor.
- [ ] Quantization, dithering, CLAHE ve Canny deterministik golden testleri geçiyor.

---

### Faz F13 — `v0.9.6`: Ecosystem Integrations ve CLI

**Hedef:** `prism` komut satırı aracı ve Nyx, Aether, Ember, Aura, Nova, Atlas, Pulse, Glyph için sıfır çekirdek bağımlılıklı ince adaptörler.

#### F13.1 — `prism` CLI (`cli/options.nox`, `cli/commands.nox`, `cli/main.nox`)
- [ ] Alt komutlar (§18):
  - [ ] `prism info <input>`
  - [ ] `prism convert <input> -o <output> [--quality N]`
  - [ ] `prism resize <input> -o <output> --width W --height H [--fit contain|cover|fill|inside|outside]`
  - [ ] `prism crop <input> -o <output> --x X --y Y --width W --height H`
  - [ ] `prism strip-metadata <input> -o <output>`
  - [ ] `prism batch <input_dir> --format <fmt> --output-dir <out_dir> [--dry-run]`
  - [ ] `prism codecs --list`
  - [ ] `prism benchmarks`
- [ ] `--dry-run`, varsayılan üzerine yazmama (`--overwrite` açık seçim), `--max-pixels`, `--max-memory` bayrakları ve hata mesajlarında EXIF/kişisel veri sızdırmama garantisi.

#### F13.2 — NOX Ekosistem Adaptörleri (`adapters/nyx.nox`, `adapters/aether.nox`, `adapters/ember.nox`, `adapters/aura.nox`, `adapters/http.nox`)
- [ ] `adapters/nyx.nox`: Multipart upload doğrulama, magic-byte sniff, decode bütçesi (örn. 5MB girdi / 100MP sınır), otomatik EXIF private metadata temizliği, responsive thumbnail/avatar üretimi, güvenli dosya adı (`safe_join`).
- [ ] `adapters/aether.nox`: Tipli görüntü işleme istek/yanıt zarfı, format müzakeresi (content negotiation) ve standart medya hata kodları.
- [ ] `adapters/ember.nox`: Idempotency anahtarlı (`content_hash + spec + codec_version`) batch iş tanımı ve atomik çıktı geçişi.
- [ ] `adapters/aura.nox`: UI surface piksel buffer köprüsü, DPI/ölçek metadatası ve non-blocking önizleme yardımcıları.
- [ ] `adapters/http.nox`: Varsayılan olarak kapalı uzaktan görüntü getirme; yalnızca açık host allowlist, `https` zorunluluğu, özel/loopback/link-local IP (`127.0.0.0/8`, `10.0.0.0/8`, `172.16.0.0/12`, `192.168.0.0/16`, `169.254.0.0/16`, `::1`) engeli ve `max_input_bytes` sınırı ile SSRF-korumalı indirme.

#### Faz F13 (`v0.9.6`) Çıkış Kapısı
- [ ] `prism` CLI alt komutları gerçek fixture'larla test edildi.
- [ ] Nyx adaptörü 5MB/100MP güvenlik sınırları ve SSRF/path traversal testlerini geçiyor.
- [ ] Opsiyonel adaptörler olmadan core bağımsız kullanılabiliyor.
- [ ] Tüm örnekler (`examples/*.nox`) gerçek `noxc 2.2.38` ile derlenip çalışıyor.

---

### Faz F14 — `v0.9.8`: Portability (WASM / Mobile / Freestanding Subset)

**Hedef:** Platform başına kanıtlanmış gerçek yetenek matrisi.

#### F14.1 — Freestanding ve WASM Doğrulaması (`tests/platforms/portability_test.nox`, `docs/compatibility.md`)
- [ ] `--profile freestanding` ile saf çekirdek modüllerinin (`core/errors.nox`, `core/limits.nox`, `core/pixel.nox`, `core/color.nox`, `core/storage.nox`, `core/image.nox`, `transforms/*`, `filters/*`) dosya sistemi veya OS çağrısı olmadan type-check ve derleme doğrulaması.
- [ ] `ResourceLimits.constrained_wasm()` (32 MiB decoded budget, 4096x4096 max boyut) profili ve bellek sınır testleri.
- [ ] Native codec bulunmayan hedeflerde `CodecUnavailableError` davranışının doğrulanması.
- [ ] `docs/compatibility.md` içinde macOS arm64/x64, Linux x64/arm64, Windows x64, `wasm32-wasi`, `wasm32-freestanding` ve Android/iOS durumlarının dürüst (`VERIFIED` vs `NOT VERIFIED`) kaydı.

#### Faz F14 (`v0.9.8`) Çıkış Kapısı
- [ ] Doğrulanan her hedefte gerçek smoke/fixture testi geçiyor.
- [ ] `docs/compatibility.md` gerçek derleyici çıktılarını yansıtıyor.

---

### Faz F15 — `v0.9.9`: Security, Fuzz, Performance ve Release Candidate

**Hedef:** Üretim öncesi güvenlik, fuzzing, performans ölçümü ve RC sertleştirmesi.

#### F15.1 — Güvenlik, Fuzz ve Benchmark Paketi (`tests/security/security_suite_test.nox`, `benchmarks/`)
- [ ] Tüm decoder'lar (PPM/PGM/BMP/PNG/GIF) ve metadata parser'ları (EXIF/ICC) için bozuk girdi, integer overflow, decompression bomb, cyclic IFD, SSRF ve path traversal güvenlik test paketi.
- [ ] Tekrarlanabilir benchmark script'leri (`benchmarks/scripts/run_benchmarks.nox`) ve `benchmarks/perf_baseline.json` / `docs/performance.md` içinde gerçek makine (`macOS arm64`, `noxc 2.2.38`, LLVM & QBE) ölçümleri (decode, encode, resize, Gaussian blur, composite, batch throughput).

#### Faz F15 (`v0.9.9`) Çıkış Kapısı
- [ ] Açık high/critical güvenlik açığı yok; tüm negatif ve fuzz testleri yeşil.
- [ ] Gerçek ölçümlere dayalı benchmark raporu `docs/performance.md` içinde kayıtlı.

---

### Faz F16 — `v1.0.0`: Prism 1.0 Stable Release

**Hedef:** Kararlı `prism.nox` kamu sözleşmesi, tam dokümantasyon seti ve 10+ derlenen gerçek dünya örneği.

#### F16.1 — 1.0 Kararlılık Kapısı
- [ ] `prism.nox` public API'sinin SemVer 1.0.0 garantisinin belgelenmesi.
- [ ] 10+ gerçek dünya örneğinin (`examples/capability_probe.nox`, `examples/create_checkerboard.nox`, `examples/thumbnails.nox`, `examples/filters.nox`, `examples/watermark.nox`, `examples/avatar.nox`, `examples/sprite_sheet.nox`, `examples/image_api.nox`, `examples/batch.nox`, `examples/animation_gif.nox`, `examples/pipeline_tiles.nox`) derlenip çalıştığının doğrulanması.
- [ ] `docs/` altındaki tüm zorunlu belgelerin (`api.md`, `architecture.md`, `capabilities.md`, `roadmap.md`, `pixels.md`, `colors.md`, `codecs.md`, `animation.md`, `security.md`, `performance.md`, `compatibility.md`, `native-adapters.md`, `nox-compatibility.md`, `release-gates.md`) tamamlanması.
- [ ] `CHANGELOG.md` içinde `v0.0.1`'den `v1.0.0`'a tüm sürüm kayıtlarının tamamlanması.

---

## 4. Sürüm Kanıt Kayıtları (Verification Log)

### `v0.0.1` (Faz F0) & `v0.1.0` (Faz F1 + §29 İlk Otonom Görev) — 2026-11-21

- **Derleyici & Ortam:** `noxc 2.2.38` (`/Users/melihburakmemis/.nox-lang/bin/noxc`), macOS arm64 (Apple Silicon).
- **Çalıştırılan Doğrulama Komutları ve Çıktıları:**
  ```text
  noxc version
  => noxc 2.2.38

  noxc check prism.nox
  noxc check --strict-spawn-shared prism.nox
  noxc check --strict-spawn-shared examples/capability_probe.nox
  => no type errors: prism.nox
  => no type errors: examples/capability_probe.nox

  noxc check --profile freestanding core/errors.nox
  noxc check --profile freestanding core/limits.nox
  noxc check --profile freestanding core/pixel.nox
  noxc check --profile freestanding core/color.nox
  noxc check --profile freestanding core/metadata.nox
  noxc check --profile freestanding core/options.nox
  noxc check --profile freestanding core/storage.nox
  noxc check --profile freestanding core/image.nox
  => 8/8 core modules passed freestanding check with 0 errors

  noxc test
  => core_pixel_test-ok: 2x2, color, ppm/pgm, 26 negative tests, 1000 random image properties passed
  => PASSED: tests/unit/core_pixel_test.nox
  => facade_test-ok
  => PASSED: tests/unit/facade_test.nox
  => 2 passed, 0 failed (2 total)

  noxc test --backend qbe
  => 2 passed, 0 failed (2 total)

  noxc run examples/capability_probe.nox
  noxc run --backend qbe examples/capability_probe.nox
  => capability-probe-ok 4 32640 800640 (both LLVM and QBE)

  noxc run examples/create_checkerboard.nox
  noxc run --backend qbe examples/create_checkerboard.nox
  => checkerboard-ppm-ok 64 64 12301 (both LLVM and QBE)
  ```
- **Üretilen ve SHA-256 Doğrulaması Yapılan Fixture'lar (`tests/fixtures-manifest.json`):**
  - `tests/fixtures/checkerboard_4x4.ppm`: `82b85db4723a6e01453848ece927f4ac23de053f2a6bfccd454926e53b613495`
  - `tests/fixtures/gradient_4x4.pgm`: `789b7fb47391a832c0ea470e262ffe6a4fe27e9f076fdfe267f9b830ba148199`

### `v0.2.0` (Faz F2 — Portable Image I/O ve Minimal Codec Suite) — 2026-11-21

- **Derleyici & Ortam:** `noxc 2.2.38` (`/Users/melihburakmemis/.nox-lang/bin/noxc`), macOS arm64 (Apple Silicon).
- **Çalıştırılan Doğrulama Komutları ve Çıktıları:**
  ```text
  noxc check prism.nox
  noxc check --strict-spawn-shared prism.nox
  noxc check --profile freestanding codecs/sniff.nox
  noxc check --profile freestanding codecs/registry.nox
  => 0 type errors

  noxc test
  => portable_codecs_test-ok
  => PASSED: tests/codecs/portable_codecs_test.nox
  => header_fuzz_test-ok: 10000 iterations, 10000 rejected cleanly
  => PASSED: tests/fuzz/header_fuzz_test.nox
  => core_pixel_test-ok: 2x2, color, ppm/pgm, 26 negative tests, 1000 random image properties passed
  => PASSED: tests/unit/core_pixel_test.nox
  => facade_test-ok
  => PASSED: tests/unit/facade_test.nox
  => 4 passed, 0 failed (4 total)

  noxc test --backend qbe
  => 4 passed, 0 failed (4 total)
  ```
- **Yeni Eklenen ve SHA-256 Doğrulaması Yapılan BMP Fixture'ları (`tests/fixtures-manifest.json`):**
  - `tests/fixtures/checkerboard_4x4.bmp` (24-bit RGB): `1f85f05d6efec22a07a2e00fc60aa80bdf30b5e6ecc829f21beb6f82ee37f368`
  - `tests/fixtures/rgba_4x4.bmp` (32-bit RGBA): `4b00ef6ef98f56325b887345a245eab8e5b4be8b8b07bfb8165bc34297d5c8d7`

### `v0.3.0` (Faz F3 — Geometry ve Resampling Engine) — 2026-11-21

- **Derleyici & Ortam:** `noxc 2.2.38` (`/Users/melihburakmemis/.nox-lang/bin/noxc`), macOS arm64 (Apple Silicon).
- **Çalıştırılan Doğrulama Komutları ve Çıktıları:**
  ```text
  noxc check prism.nox
  noxc check --strict-spawn-shared prism.nox
  noxc check --profile freestanding transforms/*.nox (8 modül)
  => 0 type errors

  noxc test
  => portable_codecs_test-ok
  => header_fuzz_test-ok: 10000 iterations, 10000 rejected cleanly
  => resample_golden_test-ok: 120 golden cases passed
  => core_pixel_test-ok: 2x2, color, ppm/pgm, 26 negative tests, 1000 random image properties passed
  => facade_test-ok
  => geometry_test-ok
  => 6 passed, 0 failed (6 total)

  noxc test --backend qbe
  => 6 passed, 0 failed (6 total)

  noxc run examples/thumbnails.nox
  noxc run --backend qbe examples/thumbnails.nox
  => thumbnails-example-ok 40 20 52 52
  ```

### `v0.4.0` (Faz F4 — Saf NOX 8-bit PNG Engine) — 2026-11-21

- **Derleyici & Ortam:** `noxc 2.2.38` (`/Users/melihburakmemis/.nox-lang/bin/noxc`), macOS arm64 (Apple Silicon).
- **Çalıştırılan Doğrulama Komutları ve Çıktıları:**
  ```text
  noxc check prism.nox
  noxc check --strict-spawn-shared prism.nox
  noxc check --profile freestanding codecs/deflate.nox
  => 0 type errors

  noxc test
  => png_test-ok
  => PASSED: tests/codecs/png_test.nox
  => portable_codecs_test-ok
  => header_fuzz_test-ok: 10000 iterations, 10000 rejected cleanly
  => resample_golden_test-ok: 120 golden cases passed
  => core_pixel_test-ok: 2x2, color, ppm/pgm, 26 negative tests, 1000 random image properties passed
  => facade_test-ok
  => geometry_test-ok
  => 7 passed, 0 failed (7 total)

  noxc test --backend qbe
  => 7 passed, 0 failed (7 total)

  noxc run examples/avatar.nox
  noxc run --backend qbe examples/avatar.nox
  => avatar-png-ok 32 32 4 4

  file tests/fixtures/rgba_4x4.png
  => tests/fixtures/rgba_4x4.png: PNG image data, 4 x 4, 8-bit/color RGBA, non-interlaced
  sips -g pixelWidth -g pixelHeight -g format tests/fixtures/rgba_4x4.png
  => pixelWidth: 4, pixelHeight: 4, format: png
  ```
- **Yeni Eklenen ve SHA-256 Doğrulaması Yapılan PNG Fixture (`tests/fixtures-manifest.json`):**
  - `tests/fixtures/rgba_4x4.png`: `524640f04d6929aaf3dd99f82777b2de5b0b0588e361b27c6c30c3560acd8f0b`

### `v0.5.0` (Faz F5 — Color, Alpha ve Metadata Hygiene / MVP Tamamlandı) — 2026-11-21

- **Derleyici & Ortam:** `noxc 2.2.38` (`/Users/melihburakmemis/.nox-lang/bin/noxc`), macOS arm64 (Apple Silicon).
- **Çalıştırılan Doğrulama Komutları ve Çıktıları:**
  ```text
  noxc check prism.nox
  noxc check --strict-spawn-shared prism.nox
  noxc check --profile freestanding core/color.nox
  noxc check --profile freestanding core/metadata.nox
  => 0 type errors

  noxc test
  => png_test-ok
  => portable_codecs_test-ok
  => header_fuzz_test-ok: 10000 iterations, 10000 rejected cleanly
  => resample_golden_test-ok: 120 golden cases passed
  => color_metadata_test-ok
  => core_pixel_test-ok: 2x2, color, ppm/pgm, 26 negative tests, 1000 random image properties passed
  => facade_test-ok
  => geometry_test-ok
  => 8 passed, 0 failed (8 total)

  noxc test --backend qbe
  => 8 passed, 0 failed (8 total)
  ```

### `v0.6.0` (Faz F6 — Filters, Enhancement ve Pixel Kernels) — 2026-11-21

- **Derleyici & Ortam:** `noxc 2.2.38` (`/Users/melihburakmemis/.nox-lang/bin/noxc`), macOS arm64 (Apple Silicon).
- **Çalıştırılan Doğrulama Komutları ve Çıktıları:**
  ```text
  noxc check prism.nox
  noxc check --strict-spawn-shared prism.nox
  noxc check --profile freestanding filters/*.nox (8 modül)
  => 0 type errors

  noxc test
  => png_test-ok
  => portable_codecs_test-ok
  => header_fuzz_test-ok: 10000 iterations, 10000 rejected cleanly
  => resample_golden_test-ok: 120 golden cases passed
  => color_metadata_test-ok
  => core_pixel_test-ok: 2x2, color, ppm/pgm, 26 negative tests, 1000 random image properties passed
  => facade_test-ok
  => filters_test-ok
  => geometry_test-ok
  => 9 passed, 0 failed (9 total)

  noxc test --backend qbe
  => 9 passed, 0 failed (9 total)

  noxc run examples/filters.nox
  noxc run --backend qbe examples/filters.nox
  => filters-example-ok 48 48 48 48 29 255
  ```

### `v0.7.0` (Faz F7 — Drawing, Text, Masks ve Compositing) — 2026-11-21

- **Derleyici & Ortam:** `noxc 2.2.38` (`/Users/melihburakmemis/.nox-lang/bin/noxc`), macOS arm64 (Apple Silicon).
- **Çalıştırılan Doğrulama Komutları ve Çıktıları:**
  ```text
  noxc check prism.nox
  noxc check --strict-spawn-shared prism.nox
  noxc check --profile freestanding composite/*.nox draw/*.nox (9 modül)
  => 0 type errors

  noxc test
  => png_test-ok
  => portable_codecs_test-ok
  => header_fuzz_test-ok: 10000 iterations, 10000 rejected cleanly
  => resample_golden_test-ok: 120 golden cases passed
  => color_metadata_test-ok
  => core_pixel_test-ok: 2x2, color, ppm/pgm, 26 negative tests, 1000 random image properties passed
  => draw_composite_test-ok
  => facade_test-ok
  => filters_test-ok
  => geometry_test-ok
  => 10 passed, 0 failed (10 total)

  noxc test --backend qbe
  => 10 passed, 0 failed (10 total)

  noxc run examples/watermark.nox
  noxc run --backend qbe examples/watermark.nox
  => watermark-example-ok 120 64 80 8
  ```

### `v0.8.0` (Faz F8 — Optional Native Codec Adapters & Pure-NOX Header Probing) — 2026-11-23

- **Derleyici & Ortam:** `noxc 2.2.38` (`/Users/melihburakmemis/.nox-lang/bin/noxc`), macOS arm64 (Apple Silicon).
- **Çalıştırılan Doğrulama Komutları ve Çıktıları:**
  ```text
  noxc check prism.nox
  noxc check --strict-spawn-shared prism.nox
  noxc check --profile freestanding codecs/jpeg_adapter.nox
  noxc check --profile freestanding codecs/webp_adapter.nox
  noxc check --profile freestanding codecs/tiff_adapter.nox
  => 0 type errors

  noxc test
  => native_adapters_test-ok
  => png_test-ok
  => portable_codecs_test-ok
  => header_fuzz_test-ok: 10000 iterations, 10000 rejected cleanly
  => resample_golden_test-ok: 120 golden cases passed
  => color_metadata_test-ok
  => core_pixel_test-ok: 2x2, color, ppm/pgm, 26 negative tests, 1000 random image properties passed
  => draw_composite_test-ok
  => facade_test-ok
  => filters_test-ok
  => geometry_test-ok
  => 11 passed, 0 failed (11 total)

  noxc test --backend qbe
  => 11 passed, 0 failed (11 total)
  ```

### `v0.8.5` (Faz F9 — Animation Model, Timeline Compositor & Pure-NOX GIF Engine) — 2026-11-23

- **Derleyici & Ortam:** `noxc 2.2.38` (`/Users/melihburakmemis/.nox-lang/bin/noxc`), macOS arm64 (Apple Silicon).
- **Eklenen Doküman:** `docs/nox-limitations.md` (derleyici/stdlib kısıtları ve Prism çözümleri).
- **Çalıştırılan Doğrulama Komutları ve Çıktıları:**
  ```text
  noxc check prism.nox
  noxc check --strict-spawn-shared prism.nox
  noxc check --profile freestanding animation/frame.nox animation/timeline.nox animation/gif.nox codecs/gif.nox
  => 0 type errors

  noxc test
  => gif_animation_test-ok
  => native_adapters_test-ok
  => png_test-ok
  => portable_codecs_test-ok
  => header_fuzz_test-ok: 10000 iterations, 10000 rejected cleanly
  => resample_golden_test-ok: 120 golden cases passed
  => color_metadata_test-ok
  => core_pixel_test-ok: 2x2, color, ppm/pgm, 26 negative tests, 1000 random image properties passed
  => draw_composite_test-ok
  => facade_test-ok
  => filters_test-ok
  => geometry_test-ok
  => 12 passed, 0 failed (12 total)

  noxc test --backend qbe
  => 12 passed, 0 failed (12 total)

  noxc run examples/animation_gif.nox
  noxc run --backend qbe examples/animation_gif.nox
  => animation-gif-ok 32 32 3 360
  ```

### `v0.9.0` (Faz F10 — Pipeline DAG, Algebraic Planner & Halo-Aware Tile Engine) — 2026-11-23

- **Derleyici & Ortam:** `noxc 2.2.38` (`/Users/melihburakmemis/.nox-lang/bin/noxc`), macOS arm64 (Apple Silicon).
- **Çalıştırılan Doğrulama Komutları ve Çıktıları:**
  ```text
  noxc check prism.nox
  noxc check --strict-spawn-shared prism.nox
  noxc check --profile freestanding pipeline/*.nox (6 modül)
  => 0 type errors

  noxc test
  => gif_animation_test-ok
  => native_adapters_test-ok
  => png_test-ok
  => portable_codecs_test-ok
  => header_fuzz_test-ok: 10000 iterations, 10000 rejected cleanly
  => resample_golden_test-ok: 120 golden cases passed
  => color_metadata_test-ok
  => core_pixel_test-ok: 2x2, color, ppm/pgm, 26 negative tests, 1000 random image properties passed
  => draw_composite_test-ok
  => facade_test-ok
  => filters_test-ok
  => geometry_test-ok
  => pipeline_test-ok: 1000 random pipelines, seam/halo & LRU cache passed
  => 13 passed, 0 failed (13 total)

  noxc test --backend qbe
  => 13 passed, 0 failed (13 total)

  noxc run examples/pipeline_demo.nox
  noxc run --backend qbe examples/pipeline_demo.nox
  => pipeline-demo-ok 24 24 2 1
  ```

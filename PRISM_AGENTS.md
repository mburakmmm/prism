# AGENTS.md — PRISM

> **Proje:** Prism — NOX için native görüntü işleme, dönüştürme ve görüntü I/O framework'ü  
> **Doküman tipi:** Kodlama ajanları için bağlayıcı mühendislik sözleşmesi  
> **Durum:** Yeni proje / başlangıç tasarımı; burada anlatılan özellikler uygulanmış kabul EDİLMEZ  
> **Dil:** Bu dosya Türkçe; kamusal API, hata türleri ve kod belgeleri İngilizce  
> **Misyon:** Pillow'un ulaşılabilir API'sini, Sharp/libvips'in performans yaklaşımını ve tip güvenli native görüntü işleme mimarisini NOX ekosistemine kazandırmak  
> **Kapsam:** Görüntü okuma/yazma, renk ve piksel modeli, dönüşümler, filtreleme, çizim, metadata, animasyon, streaming, optimizasyon, platform ve framework entegrasyonları

---

## 0. Bu Dosyanın Otoritesi ve Kullanım Protokolü

Bu dosya, Prism üzerinde çalışan tüm insan ve AI kodlama ajanları için ana geliştirme sözleşmesidir. Her görevde:

1. Bu dosyayı, depo README'sini, `docs/architecture.md`, `docs/capabilities.md`, `docs/roadmap.md`, `docs/security.md` ve görevle ilgili modülleri oku. Dosya henüz yoksa ilk uygun fazda oluştur; varmış gibi davranma.
2. Önce mevcut implementasyonu denetle: kullanıcı kodunu, testleri ve sürüm sözleşmesini incele. Bir özelliği yalnızca planlandı diye **yapılmış** sayma.
3. Kaynak NOX yeteneklerini, kullanılan derleyicinin **gerçek sürümü** ile kontrol et. Belgeler ve `noxc` davranışı çelişirse küçük bir derleme/çalıştırma testiyle gerçeği doğrula.
4. İlgili fazın kabul kriterlerini, güvenlik kapılarını ve geriye uyumluluk koşullarını belirlemeden kod yazma.
5. En küçük çalışan dikey dilimi uygula: türler → çekirdek → test → dokümantasyon → örnek → benchmark.
6. `noxc check`, `noxc test`, gerektiğinde LLVM/QBE ve platform testlerini çalıştır. Çalıştırılamayan testin nedenini yaz; sahte başarı iddiasında bulunma.
7. Kullanıcının açık onayı olmadan proje sınırlarını değiştirme, ana çekirdeğe harici native bağımlılık ekleme, mevcut public API'yi kırma ya da ana dili NOX dışına taşıma.
8. Her tamamlanan görevde `docs/roadmap.md`, `docs/capabilities.md`, CHANGELOG ve güvenlik/performance notlarını güncelle.

### 0.1 Başvuru sırası / karar hiyerarşisi

1. Güvenlik ve veri bütünlüğü kuralları (§3, §8, §22).
2. Çalışan mevcut Prism kodu ve açık kamu API sözleşmeleri (varsa).
3. NOX derleyicisi ile doğrulanmış dil ve standart kütüphane davranışı.
4. Bu AGENTS.md'nin mimari ve faz kapıları.
5. İncelenen referans kütüphanelerin semantiği (Pillow/Sharp/libvips/ImageMagick), fakat **API birebir kopyalanmayacak**.

Çelişki tespit edildiğinde, kullanıcı verisini veya güvenliği etkileyecek varsayımı sessizce seçme. Karar gerekçesini kaydet; gerekiyorsa açık tasarım kararı iste.

### 0.2 Kaynaklar — agent tarafından öncelikle incelenecek

- NOX kaynak kodu: https://github.com/mburakmmm/nox-lang
- NOX belgeleri: https://noxlang.com/
- NOX dil referansı: https://github.com/mburakmmm/nox-lang/tree/main/docs/language
- NOX standart kütüphanesi: https://github.com/mburakmmm/nox-lang/tree/main/docs/stdlib
- NOX hedefleri ve derleyici: https://github.com/mburakmmm/nox-lang/tree/main/docs/tools
- Nyx framework: https://github.com/mburakmmm/nyx
- NOX paket yönetimi: https://github.com/mburakmmm/nox-lang/blob/main/docs/tools/packages.md
- Referans davranışlar: Pillow, Sharp, libvips, ImageMagick, PNG, JPEG, GIF, WebP, TIFF, ICC, EXIF ve ilgili standart dokümantasyonu (yalnızca araştırma/test referansı; gizli runtime bağımlılığı değil).

**Depo tespiti:** 21 Kasım 2026 kaynak incelemesinde `mburakmmm/prism` deposu doğrulanamadı. Agent, ilk adımda depoyu veya yerel çalışma dizinini keşfetsin; **mevcut olduğunu, boş olduğunu veya belirli bir sürümde bulunduğunu varsaymasın**. Henüz repo yoksa aşağıdaki yapı ve hedeflerle başlasın.

---

## 1. Ürün Tanımı ve Başarı Ölçütü

Prism, NOX ile geliştirilen uygulamaların görüntüleri güvenli, hızlı, platformlar arası ve öngörülebilir şekilde işlemesini sağlayan **bağımsız** bir kütüphanedir.

Çekirdek kullanım alanları:

- Web servislerinde görsel yükleme, thumbnail, avatar, responsive image, yeniden sıkıştırma.
- Masaüstü/mobil uygulamalarda fotoğraf düzenleme ve dönüştürme.
- Görüntü filtreleme, renk ayarı, katman/composite, sprite ve asset hazırlama.
- E-posta, rapor, belge ve dijital içerik üretimi için raster görüntü üretme.
- Batch, tile, streaming ve pipeline tabanlı yüksek hacimli işleme.
- Görüntü doğrulama, metadata temizliği, güvenli dönüştürme.
- Deneysel WASM ve mobil ortamlarda mevcut capability sınırları içinde çalışma.

**Kıyas referansları:** Pillow (ergonomi), Sharp (server kullanım akışı), libvips (tile/streaming/bellek odaklı mühendislik), ImageMagick (format ve işlem kapsamı), Rust image (tip/kullanım güvenliği). Prism'in bunlarla tam özellik ya da performans eşitliği sağladığı **kanıtlanmadan** iddia edilmez.

### 1.1 Kapsama dahil

Raster görüntüler, piksel işleme, codec adaptörleri, renk dönüşümü, metadata, animasyon, pipeline, disk/ağdan kontrollü girdi, CLI ve framework adaptörleri.

### 1.2 Bilinçli kapsam dışı

- Photoshop/GIMP ölçeğinde tam etkileşimli görüntü düzenleyici uygulaması.
- OpenCV'nin tüm bilgisayarlı görü, nesne tanıma, SLAM ve AI/ML kapsamı.
- RAW kamera fotoğraflarının eksiksiz demosaic profilleri (bağımsız, ileriki opsiyonel adaptör).
- Video transcoding ve ses işleme (ayrı medya sistemi gerekir).
- PDF/SVG'nin tam rasterizasyonu (karmaşık ve riskli bağımsız adapter; ilk sürümde yok).
- GPU/CUDA/Metal/Vulkan zorunluluğu; sonradan opsiyonel backend yapılabilir.
- Güvenilmeyen kullanıcının sisteme keyfi native plugin veya kod yüklemesi.

---

## 2. NOX Gerçek Yetenekleri — Başlangıç Tabanı

21 Kasım 2026'da `nox-lang` GitHub dokümantasyonu üzerinden teyit edilen taban:

| Yetenek | Tespit | Prism kararı |
|---|---|---|
| Statik tipler, sınıflar, generics, enum/match | Belgelenmiş | API'de açık türler ve kontrollü varyantlar |
| `nox.buffer.Buffer`, `Span` | Belgelenmiş | Güvenli byte saklama ve kesit erişimi |
| `nox.binary.BinaryReader/Writer` | Belgelenmiş | Header ve binary container ayrıştırma |
| `nox.bytes` ve `list[u8]` | Belgelenmiş | Byte-exact I/O; UTF-8 stringi görüntü datası gibi yorumlama |
| `nox.fs.open_read` / chunked I/O | Belgelenmiş | Yüksek hacimli akış işlemleri |
| `nox.path.safe_join` | Belgelenmiş | Dosya yolu kısıtlama |
| `nox.thread`, `spawn`, kanallar | Belgelenmiş | Kontrollü paralel tile ve batch işleri |
| `extern def`, `ptr`, NNI | Belgelenmiş | İsteğe bağlı native codec FFI sınırı |
| `nox.http`, `nox.wasm` hedefleri | Belgelenmiş | Ekosistem entegrasyonu, fakat hedefe özel limitlerle |
| `nox.test`, `noxc test`, LLVM/QBE | Belgelenmiş | Çapraz backend testleri |
| Yerleşik `nox.image` ile evrensel codec | Doğrulanmadı | Var sayılmayacak; codec katmanı Prism'e ait |

**Sürüm kuralı:** Sabit bir `noxc` sürümünü körlemesine kabul etme. `noxc version`, kaynak `CHANGELOG` ve CI'da doğrulanan sürüm kaydedilir. `noxc 2.2.x` üzerinde çalışan API, daha eski sürümlerde otomatik desteklenmiş sayılmaz.

### 2.1 Özel kısıtlar

- NOX statik tiplidir; Python `Any`, reflection tabanlı sınırsız dinamik dispatch, runtime monkeypatching veya `eval` kullanma.
- `list[u8]` byte-packed, `Buffer` sabit boyutludur; büyük veride gerçek profil almadan bunların birbirine göre daha hızlı olduğunu varsayma.
- `nox.binary` byte-order aware; dosya codec'lerinde endianness otomatik değildir, açıkça tanımla.
- `ptr_of` yalnızca izinli liste türlerinde ve `lowlevel` sınırı içinde; büyüyen listelerin eski pointer'ları geçersizleşebilir. Pointer saklama yasaktır.
- `extern def` ile native koda sınıf/list/Optional doğrudan verilemez; işlevleri C ABI kurallarına göre sarmala.
- Native dinamik kütüphaneler WASM'de kullanılamaz; mobil hedeflerde FFI/linker koşulları ayrıca doğrulanmalı.
- `--target wasm32-*` desteği, bütün codec'lerin ve OS servislerinin de aynı hedefte çalıştığı anlamına gelmez.
- Freestanding modda filesystem ve birçok hosted yetenek yoktur; yalnızca saf matematik/piksel çekirdeği hedeflenir.
- NOX derleyicisi değişikliği, Prism'in bir özelliği için **varsayılan çözüm değildir**. Ayrı minimal reproducer ve gerekçe olmadan derleyiciye müdahale etme.

### 2.2 F0 capability proof-of-concept

İlk fazda `examples/capability_probe.nox` ve `docs/nox-compatibility.md` hazırlanmalı. Aşağıdaki noktalar **derlenip çalıştırılarak** doğrulanmalı:

- 16 MiB ve 256 MiB `Buffer` tahsis, sınır hataları, kopyalama ve serbest bırakma davranışı.
- `list[u8]` / `Buffer` / `str` arasında binari veri round-trip (NUL ve invalid UTF-8 dahil).
- 64-bit tam sayı çarpım taşması için Prism checked arithmetic helper.
- Çok büyük satır sayısıyla heap/GC/ARC davranışı.
- `nox.fs` chunked okuma/yazma ve Windows binary round-trip.
- `spawn` ile paralel tile işlerinin birleştirme sırası ve `--strict-spawn-shared` kontrolü.
- `extern def` ve `ptr_of` prototipinin en az macOS/Linux/Windows uygun CI ortamında derlenmesi.
- Mobil/WASM hedeflerinde bir örnek kernel'in import edilip çalışması; çalışmıyorsa açık neden raporu.

---

## 3. Değişmez İlkeler (Non-Negotiable)

1. **NOX-first:** Tüm public API, görüntü modeli, iş hattı, filtreler ve portable algoritmalar `.nox` dosyalarıyla yazılır. Native kütüphaneler yalnızca onaylı, izole codec/accelerator adaptöründe.
2. **Güvenilmeyen girdiler düşmanca kabul edilir:** Boyut/çarpım/offset ve codec limitleri decode başlamadan doğrulanır.
3. **Sessiz bozuk çıktı yok:** Desteklenmeyen format, renk uzayı, metadata veya animasyon seçenekleri `Unsupported...Error` üretir; kayıplı dönüşümler açıkça bildirilir.
4. **Açık kaynak/kullanıcı güveni:** Açıklanmamış ağ erişimi, telemetri, otomatik görüntü yükleme veya API üzerinden keyfi kod çalıştırma yasaktır.
5. **Kaynak bütçesi:** Bellek, CPU süresi, boyut, frame sayısı, decoding ratio, metadata ve temp dosya limitleri yapılandırılabilir, güvenli varsayılanları vardır.
6. **Taşma koruması:** `width * height * channels * bytes_per_channel`, stride ve buffer offset hesabı kontrollü yapılır. İşaretli `int` wrap davranışına güvenme.
7. **RGB/alpha doğruluğu:** sRGB, lineer ışık, düz/premultiplied alpha ve gamma semantiğini karıştırma; doğru dönüşümü ifade eden tür/etiket kullan.
8. **Deterministik çekirdek:** Aynı giriş/ayarlar/versiyon CPU yolağında aynı piksel sonuçlarını üretmeli; platforma göre tolerans gerektiren float/nativeler açıkça belgelenir.
9. **Hata güvenliği:** Başarısız işlem orijinal `Image` verisini veya çıktı dosyasını bozmaz; atomik yazma kullanılabiliyorsa kullan, garanti düzeyini belgele.
10. **Kaynak yaşam döngüsü:** Tüm file/codec/native handle/temp buffer'lar başarısızlıkta da bırakılır. Çift serbest bırakma ve use-after-free yasaktır.
11. **Codec güvenliği:** Native decode kütüphaneleri testli, sürüm/paket lisansı doğrulanmış ve güncellenebilir olmalıdır. Native codec zorunlu core bağımlılığı olamaz.
12. **Ölçümsüz performans iddiası yok:** Benchmark ortamı, veri kümesi, sürüm, p50/p95 ve peak RSS kaydedilmeden 'Sharp'tan hızlı' gibi iddia yazılmaz.
13. **Geriye uyum:** Public API değişikliği semver'e tabidir. Experimental işlevler ayrıca işaretlenir.
14. **Yetki minimizasyonu:** HTTP/network, dosya sistemi, üçüncü taraf plugin veya font erişimi yalnızca ilgili adaptöre aittir.
15. **Lisans disiplini:** Copyleft/codec patent/dağıtım yükümlülükleri incelenmeden native codec paketleme kararı verilmez.
16. **Tests before done:** Her yeni özellik davranış, negatif, limit, fuzz/mutasyon ve varsa diferansiyel testle birlikte gelir.

---

## 4. Mimari ve Paket Sınırları

Önerilen yapı (mevcut depo varsa uyarlama gerekçelendirilecek):

```text
prism/
  AGENTS.md
  README.md  LICENSE  SECURITY.md  CHANGELOG.md  CONTRIBUTING.md
  nox.json  nox.lock
  prism.nox                   # kararlı public facade
  core/
    image.nox                 # Image, ImageView, MutableImage, plane metadata
    pixel.nox                 # PixelFormat, Pixel, pixel read/write
    storage.nox               # owner, strides, planes, Buffer adapters
    limits.nox                # DecodeLimits, WorkBudget, checked arithmetic
    errors.nox                # tiplenmiş hata hiyerarşisi
    color.nox                 # sRGB/linear/YCbCr/alpha
    metadata.nox              # güvenli metadata modeli
    options.nox               # ortak typed seçenekler
  codecs/
    registry.nox  sniff.nox  reader.nox  writer.nox
    ppm.nox  pgm.nox  bmp.nox   # taşınabilir temel codec'ler
    png.nox                   # fazlı saf NOX veya doğrulanmış adapter
    jpeg_adapter.nox          # opsiyonel native
    webp_adapter.nox          # opsiyonel native
    gif.nox                   # parsing/animation seçeneklerine bağlı
    tiff_adapter.nox          # opsiyonel, limitli
  transforms/
    geometry.nox  resize.nox  affine.nox  resample.nox
    flip.nox  crop.nox  border.nox  rotate.nox
  filters/
    convolution.nox  gaussian.nox  sharpen.nox  edges.nox
    median.nox  morphology.nox  enhance.nox  effects.nox
  draw/
    canvas.nox  shapes.nox  paths.nox  rasterizer.nox  text.nox
  composite/
    alpha.nox  layers.nox  masks.nox  blend.nox
  animation/
    frame.nox  timeline.nox  gif.nox  apng.nox  webp.nox
  pipeline/
    graph.nox  nodes.nox  planner.nox  executor.nox  tiles.nox
    cache.nox  parallel.nox  streaming.nox
  adapters/
    file.nox  memory.nox  http.nox
    nyx.nox  aether.nox  aura.nox  ember.nox
  native/                     # yalnızca opt-in codec/fast adapter
    README.md  include/  build/  bindings/
  cli/
    main.nox  commands.nox  options.nox
  examples/
    thumbnails.nox  filters.nox  watermark.nox  avatar.nox
    sprite_sheet.nox  image_api.nox  batch.nox
  tests/
    unit/  codecs/  properties/  fuzz/  security/
    golden/  differential/  integration/  platforms/
    fixtures/  fixtures-manifest.json
  benchmarks/
    README.md  perf_baseline.json  scripts/  results/
  docs/
    api.md  architecture.md  capabilities.md  roadmap.md
    pixels.md  colors.md  codecs.md  animation.md  security.md
    performance.md  compatibility.md  native-adapters.md
    nox-compatibility.md  release-gates.md
  .github/workflows/
    ci.yml  security.yml  release.yml
```

### 4.1 Katman diyagramı

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

**Bağımlılık yönü:** Çekirdek `nyx`, `aether`, `aura`, `ember` veya herhangi bir uygulama framework'üne bağımlı değildir. Adapter'lar çekirdeğe bağımlı olur; tersine değil.

### 4.2 Modül sorumlulukları

- `core` veri modelini, renk ve güvenliği sahiplenir; codec ayrıntılarını bilmez.
- `codecs` belirli biçimi decode/encode eder; geometrik filtreleri yeniden yazmaz.
- `transforms`, `filters`, `draw`, `composite` piksel çekirdeğini kullanır; dosya I/O'suna bağımlı değildir.
- `pipeline` iş planlar; görüntü etkilerini kopyalamaz.
- `native` platforma göre etkinleşebilen, bağımsız ve belgelenmiş ABI sınırıdır.
- `adapters` framework'lere ince uyarlama sağlar; ortak `Image` türüne bağlanır.
- `cli` kütüphane API'sini kullanır; ikinci gizli görüntü motoru içermez.

---

## 5. Genel API Tasarım İlkeleri

- API statik tiplemeye uygun, tutarlı ve tahmin edilebilir olmalı.
- `Image` okuma ve transform operasyonları **varsayılan olarak yan etkisiz**: yeni `Image` / kontrollü immutable view döndürür.
- Mutasyon ayrı `MutableImage`/`Canvas` tipi veya açık `*_inplace` metotları ile yapılır.
- `ImageView` ortak owner'ı yaşatır; parent release ile dangling pointer oluşmaz.
- `close()` gerektiren native/lazy/stream resource açıkça belgelenir; mümkünse `with`/`defer`/`finally` kullanılır.
- Örtük I/O ve network çağrısı yok. `load_file` ile `decode_bytes`, `fetch_url` birbirinden ayrıdır.
- Hata türleri stabil; exception metni stabil sözleşme değildir.
- String option'ları yerine enum/sealed typed option mümkünse tercih edilir; her enum değeri için explicit default.
- `None` ve null handle semantiği açık olmalı; `Image` geçerli bir görüntüde boş pointer saklamamalı.
- Public `__all__` kullanan modüller NOX sürümünün buna uyumlu olduğuna dair test içerir.

### 5.1 Taslak örnek kullanım — mevcut derleyici için doğrulanmadan yayınlanmaz

```nox
# ÖRNEK HEDEF API — pseudocode-ish NOX; test olmadan 'çalışır' sayılmaz
from prism import open_image, save_image, ResizeOptions, Image

img: Image = open_image("input.png")
thumb: Image = img.resize(320, 320, ResizeOptions.fit_contain())
save_image(thumb, "thumb.png")
```

```nox
# ÖRNEK HEDEF API
from prism import new_image, Pixel, Image

canvas: Image = new_image(512, 512, Pixel.rgba(255, 255, 255, 255))
result: Image = canvas.rotate(15.0)
```

**Kural:** Taslak sözdizimi Nox'ta unsupported factory/overload/staticmethod gerektiriyorsa NOX'a uygun çağrı API'sine dönüştür. Örnekleri gerçekte derlemeden public dokümantasyona taşıma.

### 5.2 Temel stable public facade (önerilen)

- `Image`, `MutableImage`, `ImageView`, `Pixel`, `PixelFormat`, `ColorSpace`, `AlphaMode`.
- `DecodeOptions`, `EncodeOptions`, `ResourceLimits`, `ImageInfo`, `ImageMetadata`.
- `decode(data, options)`, `encode(image, format, options)`.
- `open_image(path, options)`, `save_image(image, path, options)`.
- `probe(data, options) -> ImageInfo`, `probe_file(path, options)`.
- `new_image(width, height, pixel)`.
- `resize`, `thumbnail`, `crop`, `rotate`, `flip`, `transpose`, `pad`, `affine`.
- `blur`, `sharpen`, `contrast`, `brightness`, `saturation`, `grayscale`, `threshold`, `convolve`.
- `composite`, `blend`, `mask`, `draw_*`, `text_*`.
- `Pipeline`, `Frame`, `Animation`, `BatchResult` (gelişmiş fazlar).

İsimlerin ve imzaların kesinleşmesi **faz kapısında** kaynak derleme testlerine tabidir. Özellikleri hazır değilken facade'ye 'noop' stub olarak ekleme.

---

## 6. Piksel Veri Modeli ve Bellek Sözleşmeleri

### 6.1 Minimum güvenilir temsiller

- `GRAY8`: bir kanal, `u8`.
- `RGB8`: üç kanal, interleaved `u8`.
- `RGBA8`: dört kanal, interleaved `u8`, varsayılan **straight alpha**.
- `GRAY16`, `RGB16`, `RGBA16`: daha sonraki faz, byte order açık.
- `RGBAF32` ve `FLOAT32` planar/sıralı seçenek: yalnızca sayısal test ve performans karşılaştırması sonrası.
- `INDEXED8`/palette: animasyon/GIF için; gerektiğinde RGBA8'e normalize edilir.
- `YCbCr`/CMYK: codec giriş formatı; açık profile göre görüntü modeline dönüştürme veya typed unsupported error.

### 6.2 `Image` invariant'ları

1. `width > 0`, `height > 0`; sıfır boyutlu bitmap kamu API'sinde reddedilir. Boş batch/çizim yüzeyleri ayrı türle ifade edilebilir.
2. `channels` ve bytes-per-pixel, PixelFormat sözleşmesinden türetilir; kullanıcıdan gelen bağımsız tutarsız değer kabul edilmez.
3. `stride >= width * bytes_per_pixel`, tüm çarpımlar önce checked; `buffer_length >= (height-1)*stride + min_row_bytes`.
4. Kanal yerleşimi, endian, renk ve alpha modu `ImageInfo` üzerinden bilinir.
5. `PixelBuffer` owner ve view yaşam döngüsü açık, eski buffer'ı sızdırmaz.
6. Pikseller satır-major düzende, origin varsayılan sol üst. EXIF orientation `display_orientation` olarak ayrı tutulabilir; dönüştürülmeden top-left normalize edilmiş gibi bildirilmez.
7. Transparent pixel RGB değerleri korunma politikası belgelidir; alpha sıfırken bilinçsizce sıfıra indirme veya rasterizasyonla renk kaybını gizleme.
8. `Image` dışarıya mutable raw buffer kaçırmaz; mutable API'nin alias, copy-on-write ve concurrency kuralları açık.

### 6.3 Stride ve hesaplama

- Girdi offset hesaplarında `checked_mul` / `checked_add` gerekir; Nox `int` wrap riski var.
- `width * height` sınırı, decoded bytes limiti ve pixel budget ayrı ayrı denetlenir.
- Buffer'a bakmadan önce stride/plane/bit-depth boyutlarını doğrula.
- Cropped view'da yeni allocation zorunlu değil; fakat parent lifetime korunur ve offset hesabı checked.
- Mutating overlap operations kaynak ve hedef örtüşmesini destekliyorsa snapshot gerekir; aksi halde açık hata.
- SIMD/FFI'de alignment, aliasing ve pointer lifetime sözleşmesi ayrıca doğrulanır.
- Endian ve row padding hiçbir işlemde sessiz kayıp yaşamaz.

### 6.4 `ResourceLimits` varsayılan politika

Önerilen başlangıç güvenlik değerleri, gerçek bench/testle yeniden kalibre edilebilir:

| Parametre | Önerilen başlangıç limiti | Not |
|---|---:|---|
| max_width / max_height | 32,768 px | Tek eksen |
| max_pixels | 100,000,000 | Geniş görüntü bombası |
| max_decoded_bytes | 512 MiB | Çok kanallı/multi-frame ayrı hesap |
| max_input_bytes | 256 MiB | İstek ve dosya girişinde |
| max_frames | 200 | Animasyon |
| max_metadata_bytes | 4 MiB | EXIF/ICC/comment toplamı |
| max_chunk_bytes | 64 MiB | Container chunk sınırı |
| max_recursion_depth | 32 | Format/dahili parser yapısı |
| max_output_bytes | 512 MiB | Encoder sonrası |
| max_temp_bytes | 1 GiB | Pipeline spill; explicit opt-in |
| max_work_ms | 30,000 | Server side default; host deadline |

**Bunlar örnek başlangıç limitleridir**, daha küçük sunucu profilleri veya mobil/WASM profilleri zorunlu olabilir. Limitler API'de açıkça seçilebilir, güvenli varsayılanlar hiçbir koşulda sınırsız değildir. Çoklu frame işlemlerinde toplam decoded work hesabı yapılır.

---

## 7. Renk, ICC, EXIF ve Alpha Politikası

### 7.1 Renk uzayı

- Varsayılan 8-bit raster çıktısı `sRGB` ile etiketlenir **yalnızca** gerçekten sRGB kabulü/normalizasyonu yapılmışsa.
- `ColorSpace`: `sRGB`, `LinearSRGB`, `DisplayP3` (gelişmiş ICC fazı), `Unknown`, `ICCProfile` referansı.
- Renk profili olmayan girdide `assume_srgb` gibi açık bir politika tanımla; sessizce geniş gamut görüntüyü sRGB'ye çevirmiş gibi gösterme.
- Brightness, contrast, saturation, blend ve resampling'in hangi uzayda yapıldığı belgelenir.
- Fiziksel doğruluk isteyen blending/resampling için lineer ışık kullan; legacy perceptual sRGB yaklaşımı varsa ayrı mod.
- ICC desteği olmadan Display P3/CMYK gibi profillerin doğru renk dönüştürüldüğünü iddia etme.

### 7.2 Alpha

- `Straight` ve `Premultiplied` açık iki mod; dönüşümde integer rounding, zero-alpha ve gamma etkileşimi test edilir.
- Porter–Duff `source-over` başlangıç sözleşmesidir. Gelişmiş blend modları için clipping ve alpha hesapları belgelenecek.
- Görselin renk kanalları ve alpha kanalı birbiriyle yanlış ölçekle toplanmaz.
- Transparent padding/border/resize için alpha bleeding (halo) testleri zorunlu.

### 7.3 Orientation ve metadata

- EXIF orientation 1–8 için doğru transpose/rotate ve width/height swap test edilir.
- `autorotate` seçeneği uygulanınca EXIF Orientation eski dönüşümü yeniden uygulatmayacak şekilde güncellenir/silinir.
- ICC profilini taşımak ve ICC dönüşümünü gerçekleştirmek iki **ayrı** kabiliyettir.
- Metadata rewrite güvenli allowlist + strip varsayılanlarıyla tasarlanır.
- GPS, kamera seri numarası ve üretici notları gibi gizli bilgilerin temizlenmesi için `strip_private_metadata` modu sunulur.
- Aşırı büyük/bozuk ICC/EXIF girişlerine resource limit uygulanır.

---

## 8. Codec Stratejisi ve Format Matrisi

### 8.1 Öncelikler

1. Saf NOX'ta veri modeli ve doğru piksel işlemleri.
2. Saf NOX'ta basit, test edilebilir formatlar: Netpbm PPM/PGM ve basit BMP varyantı.
3. PNG decode/encode: mümkünse saf NOX ve mevcut sıkıştırma kabiliyetleri ile; zlib/DEFLATE encode/decode gerçek desteği **önce** capability testinde doğrulanır. Eksikse bağımsız, audit edilebilir çözüm veya opsiyonel codec adapter; asla gzip ile zlib akışını eşdeğer sanma.
4. JPEG/WebP: güvenilir native adaptör, sabit ABI ve güvenli limitler; saf NOX decoder ancak açık gerekçe ve test fonu varsa.
5. GIF: format decode + frame/disposal, palette ve animasyon zamanlaması; encoder sonra.
6. TIFF: kontrollü subset/opsiyonel libtiff, çok sayfalı ve metadata limitleri.
7. APNG, animated WebP, AVIF, HEIF, JPEG XL: ileriki opsiyonel adapter ve lisans/patent/platform incelemesi.
8. SVG ve PDF rasterizasyonu: ayrı, sandbox'lı adapter olmadan core'a alınmaz.

### 8.2 Hedef matrisi

| Format | Decode | Encode | İlk yöntem | Aşama |
|---|---|---|---|---|
| PPM/PGM (P5/P6) | Evet | Evet | Saf NOX | Temel |
| BMP (24/32-bit uncompressed) | Evet | Evet | Saf NOX | Temel |
| PNG (8-bit RGB/RGBA/Gray) | Evet | Evet | Saf NOX / onaylı adapter | Çekirdek |
| PNG 16-bit / interlaced | Sonra | Sonra | Kapsam genişlemesi | İleri |
| JPEG baseline | Evet | Evet | Native adapter | Codec |
| JPEG progressive/CMYK | Sonra | Sonra | Native codec'in güvenli yetenekleri | İleri |
| WebP lossy/lossless | Evet | Evet | Native adapter | Codec |
| GIF still/animated | Evet | Sonra | Saf NOX veya adapter, aşamalı | Animasyon |
| TIFF baseline + multi-page | Sonra | Sonra | Native adapter | İleri |
| AVIF / HEIF / JPEG XL | Opsiyonel | Opsiyonel | Ayrı adapter | Gelecek |
| SVG / PDF | Core dışında | Core dışında | İzole rasterizer | Gelecek |

**Her hücre hedeftir, tamamlanmış özellik değildir.** Release notunda format–bit-depth–alpha–animation varyantları tek tek belirtilir.

### 8.3 Format detection ve decoder güvenlik kontratı

- Formatı MIME/uzantıdan değil, **magic bytes + tam header doğrulaması** ile belirle; isteğe bağlı caller hint yalnızca tutarlılık doğrulaması.
- Codec hatalarında `InvalidImageError`, `UnsupportedFormatError`, `ResourceLimitError`, `CorruptDataError`, `CodecUnavailableError` gibi ayrım yap.
- Aynı veriyi birden çok codec'in kabul etmesi halinde deterministik ve güvenli precedence uygula.
- `probe` işleminde tamamı decode edilmez; ancak header boyutlarının doğruluğu ve metadata maliyeti denetlenir.
- Decoder, kesilmiş/truncated, CRC hatalı, duplicate chunk, overflow, invalid offset, milyarlarca frame, loop, cyclic IFD, aşırı expansion gibi girdileri reddeder.
- Decoder, kullanıcı kontrolündeki dış URL/yerel dosyayı kendiliğinden takip etmez.
- Encoder her zaman `EncodeOptions` ile kalite, chroma subsampling, metadata, alpha ve lossless ayarlarını sözleşmeye bağlar.
- Dosya uzantısı/format uyuşmazlığında açık format tercihi esas; sessizce yanlış uzantıyla yazma opsiyonu varsayılan olamaz.
- Parser fuzz ve negatif testleri, test dosyasını önceden tamamen RAM'e yükleme zorunluluğu olmadan tasarlanır.

### 8.4 PNG teknik maddeleri

- PNG signature, IHDR/IDAT/IEND sırası, chunk length ve CRC doğrulanır.
- Color type 0/2/3/4/6, bit depth kombinasyonları aşamaya göre açık allowlist; desteklenmeyenler typed error.
- Scanline filter 0–4 doğru uygulanmalı; filter geçmiş satır ve kanal byte-width sözleşmesine uymalı.
- Adam7 interlace ilk sürümde yoksa açık reddetme ve test.
- Palette + tRNS, gamma/sRGB/iCCP ve ancillary chunks ayrı test edilir; metadata taşınmadan önce limit.
- IDAT parçalarının birleştirilmesi için streaming decoder, deflate wrapper formatı ve checksum doğrulanmalı.
- IDAT genişletme oranı ve toplam decoded work sınırlandırılır.
- Karşılaştırma PNG fixture corpusuyla exact byte değil, pixel-equivalence + gerekli metadata testidir.

### 8.5 JPEG / WebP / TIFF native adapter sözleşmesi

- Kütüphaneler ismen değil **gerçek paket/ABI sürümleriyle** pinlenir.
- Adapters ayrı modül, açık feature/capability check, eksikse `CodecUnavailableError`.
- Input ve output raw buffer'lar C ABI üzerinden sınırlandırılır; native code'dan gelen pointer/length çifti trusted değildir.
- Deallocator hangi kütüphaneden geldiğiyle eşleştirilir; `malloc` ve codec-özel `free` karıştırılmaz.
- C callback exception'ları native sınırı aşmaz; hata kodu ve küçük fixed payload üzerinden raporlanır.
- Native boyut/piksel bilgisi Prism tarafından tekrar checked arithmetic ile doğrulanır.
- Decoder kesilmesi/timeout/cancellation native kütüphanede güvenli değilse ayrı process isolation hedeflenir; basit thread iptali güvenli sayılmaz.
- Codec daha önce bilinen CVE içeren sürümde tutulmaz; güncelleme ve regression protokolü vardır.
- Codec'lerin lisansları (özellikle libjpeg-turbo, libwebp, libtiff, libvips ve transitif bileşenleri) release matrisinde belgelenir.

---

## 9. Transform Engine: Geometri ve Resampling

### 9.1 Geometrik fonksiyonlar

- `crop(x,y,w,h)` bounds policy: strict/reject, clip veya pad açık option.
- `resize(w,h,filter)` ve `thumbnail(max_w,max_h)`; `contain`, `cover`, `fill`, `inside`, `outside` davranışları tek tek tanımlanır.
- `rotate(angle, expand, center, background)`; özellikle 90°/180°/270° fast path.
- `flip_horizontal`, `flip_vertical`, `transpose` ve EXIF orientation dönüşümleri.
- `pad(left,top,right,bottom,color)` checked dimension ve alpha.
- `affine` (2x3), perspektif (3x3; ayrı ileri faz), border modes.
- Raster origin, pixel-center convention ve koordinat/angle birimi dokümante edilir.

### 9.2 Resampling algoritmaları

1. Nearest neighbor — indeks yuvarlama sözleşmesi.
2. Bilinear — renk + alpha güvenli, boundary handling.
3. Bicubic — kernel tanımı ve negatif lob/clamping.
4. Lanczos2/Lanczos3 — sinc kernel + support ve sample sayısı.
5. Area/box — downsample için örtüşme oranı.

- Piksel merkezleri `x + 0.5`, `y + 0.5` yaklaşımıyla açıkça tanımlanır; başka referans kullanılıyorsa golden fixture ile sabitle.
- Resize yüksek kalite yolu gamma-aware, premultiplied alpha kullanır; legacy `fast` ayrı adlandırılır.
- Float accumulator sınırlandırma ve `u8` quantization rounded/clamped; NaN/Inf parametre reddi.
- Büyük downscale için çok kademeli pipeline gerekliyse kalite regression ile savun.
- Nearest ve 90° dönüşleri exact; bilinear/bicubic/Lanczos toleranslı golden testlerle doğrulanır.

### 9.3 Geometry kabul testleri

- 1x1, 1xN, Nx1, sınır piksel, odd dimensions ve oversized input.
- 45° transparent border, alpha halos, kaynak/hedef overlap.
- Exact crop ve flip için pixel-exact property test.
- Resize 2×→½× round trip için tolerans ve kalite metriği.
- EXIF orientation 1–8 testleri.
- Negatif, sıfır, aşırı, NaN/Inf boyut ve açı parametreleri.

---

## 10. Filters, Enhancement ve Pixel Kernels

### 10.1 Temel filtreler

- Grayscale: sRGB luma ile lineer luminance farklıdır; API'de tanımla.
- Invert, threshold (global; adaptive daha sonra), posterize ve solarize.
- Brightness, contrast, saturation, hue, gamma, temperature/tint (gelişmiş).
- Convolution generic `NxM` kernel, edge/border mode, normalization ve offset.
- Box blur, Gaussian blur (separable), unsharp mask, sharpen.
- Sobel/Prewitt, Laplacian, edge magnitude; Canny ileri modül.
- Median/percentile filter, morphology erode/dilate/open/close.
- Histogram, equalization, CLAHE (ileri), channel split/merge ve LUT.
- Noise/grain, vignette, emboss, pixelate ve sepia (ek uygulama modülleri).

### 10.2 Kernel kontratı

- `radius`, `sigma`, kernel boyutu ve toplam work budget için açık üst sınırlar.
- Kernel boyutu tek/çiftse anchor ve padding politikası belirlenir.
- Separable convolution varsa exact olmayan float farkları tolerans sınırına alınır.
- Premultiplied alpha üzerinde renge etkisi nasıl olacak açıkça belirtilir.
- Kenar kipleri: clamp, mirror, wrap, constant; hazır olmayan kip `UnsupportedFeatureError`.
- Float intermediate kullanımı, taşma, negatif değer ve NaN handling belgeli.
- Kernel cache ve SIMD sonradan; eski sonuçla toleranslı regression şart.

### 10.3 İstatistik/analiz API'si

- Channel histogramları; min/max/mean/std, percentiles ve entropy.
- Renk sayımı ve palet üretimi (quantization) temel/gelişmiş fazlar.
- `ImageHash` gibi algısal hash'ler ayrı yardımcı modülde; kriptografik hash yerine geçmez.
- PSNR, MSE, MAE ve SSIM (temel doğrulama için; uygulama tamamlanınca belge).
- Her metrik için kanal/alpha ve renk uzayı varsayımı açık.

---

## 11. Drawing, Text ve Rasterization

### 11.1 Çizim çekirdeği

- Düzlem canvas, rectangle, rounded rectangle, line, polyline, ellipse, polygon, bezier path (fazlı).
- Stroke/fill, opacity, joins/caps, clipping region, transformations.
- Anti-aliasing (coverage/supersampling); deterministik CPU rasterizer.
- Flood fill seçeneği kaynak bütçeli ve iteratif; stack overflow yaratmaz.
- Mask/clip uygulaması multi-layer ile ortak engine kullanır.

### 11.2 Yazı ve font

- Temel bitmap font (lisansı açık, embedded) ile küçük taşınabilir bir başlangıç sistemi.
- TrueType/OpenType, Unicode shaping, bidi, font fallback yalnızca doğrulanmış native text adapter (örn. FreeType/HarfBuzz tabanlı) veya bağımsız kapsamlı implementasyon fazıyla.
- Nox `str` Unicode code point semantiği, glyph shaping ile aynı şey değildir; Latin/Türkçe düzgün çıkması bile CJK/Arabic desteği iddiası değildir.
- Untrusted font dosyaları da untrusted binary input; format sınırı ve kaynak izolasyonu uygulanır.
- `draw_text`, font selection, baseline, kerning, line-height ve alignment sözleşmeleri.
- Text ölçümü (`measure_text`) ve çizimi aynı shaping engine'i kullanır.

---

## 12. Compositing, Masks ve Blend Modes

- `source_over`, `destination_over`, `source_in`, `source_out`, `xor`, `plus` gibi Porter–Duff kipleri (fazlı).
- Normal, multiply, screen, overlay, darken, lighten, color-dodge, difference, hue/saturation vb. blend modları yalnızca matematiği ve renk uzayı doğrulanınca.
- Binary ve 8-bit/float mask; mask boyut uyuşmazlığı için explicit scaling policy.
- Layer tree z-order, offset, opacity ve transform.
- Translucent edges, alpha=0/255 exact test, subpixel alignment.
- Compositing pipeline büyük görüntülerde tile tabanlı çalışabilecek şekilde saf kernel üzerinden sunulur.

---

## 13. Animasyon ve Çok Kareli Görüntüler

### 13.1 Veri modeli

`Frame(image, duration_ms, offset_x, offset_y, disposal, blend)` + `Animation(canvas, frames, loop_count, metadata)` hedef modelidir.

- Animasyon decoder canvas kompozisyonunu doğru hesaplamalı; yalnızca frame delta buffer'ını bütün kare sanmamalı.
- GIF: global/local color table, transparency index, delay, disposal 0/1/2/3, logical screen background ve loop extension.
- GIF 0 delay için güvenli politika; kullanıcıya süre ve frame count kontrolleri.
- APNG: `dispose_op`, `blend_op`, frame offsets ve sequence sırası.
- WebP animated: frame disposal/blend ve ICC/EXIF meta; native adapter.
- Çok sayfalı TIFF, animasyonun aynısı değildir; `ImageSequence` türü değerlendir.

### 13.2 Animasyon güvenliği

- `frames * width * height * bpp` ile potansiyel toplam iş sınırı.
- Lazy frame decode desteklenirse kapatılan decoder sonrası frame referansı geçersiz olmaz; veya erişim typed error.
- Delta frame/restore-previous için kaynak buffer üst sınırı.
- Sonsuz loop, encode sırasında işlem limitini aşmaz; loop metadatası ile gerçek iteration ayrı.

---

## 14. Image Pipeline: Eager, Lazy, Tiles ve Streaming

### 14.1 Katmanlı execution modeli

- Faz 1: Eager operations — `Image` alır ve yeni `Image` üretir.
- Faz 2: Operasyon DAG'i — `Pipeline` immutable node'lar, explicit parameters, no arbitrary code execution.
- Faz 3: Planner — format dönüşümünü birleştirme, ardışık crop/resize sadeleştirme, tile sınırlarını belirleme.
- Faz 4: Executor — chunk/tile, memory budget, concurrency ve backpressure.
- Faz 5: Kaynak çekme (streaming decode/encode) — codec'in gerçek desteğine göre.

### 14.2 Pipeline node türleri

`Decode`, `ConvertColor`, `Orient`, `Crop`, `Resize`, `Affine`, `Filter`, `Composite`, `Draw`, `Encode`.

- Nodes saf/typed; parametreleri veri olarak tutulur, değerlendirme sırasında `eval` çalıştırılmaz.
- Plan optimizer, piksel davranışını değiştiren yeniden sıralama yapmaz; renk/alpha conversion en kritik örnek.
- Plan cache key: input fingerprint, operator tipi, parametreler, color profile, codec/config version, target capability.
- Disk/memory cache TTL ve eviction politikası kaynak bütçesine uyar.
- Mutasyon içeren operatörlerin DAG'e etkisi açık; görüntü değişmişken cache invalidate gerekir.

### 14.3 Tile ve halo

- Tile boyutu yapılandırılır (örn. 256×256), ancak performansla kalibre edilir.
- Gaussian blur/convolution için halo/border pixels doğru; tile sınırında çizgi oluşmaz.
- Çok aşamalı filtrelerde halo birikimi ve global dependency analizi yapılır.
- İşlenen toplam görüntü/özellik, memory ceiling'i aşmayacak şekilde chunk'lanır.
- Full-image gerektiren transform (global histogram, arbitrary rotate, global sort) otomatik tile-streaming iddiasıyla sunulmaz; ayrı `requires_materialization` sinyali.

### 14.4 Streaming ve disk spill

- `ImageReader`/`ImageWriter` aç/kapat protokolü.
- Decode aşaması ve görüntü işlemi pencereleri input/output work budget ile sınırlandırılır.
- `read_chunk(n)` ile gelen verinin hemen tümünü belleğe toplamak **streaming değildir**.
- Transpose/rotate gibi işlemlerde external memory algoritması gerekir; aksi halde unsupported veya eager fallback açık.
- Spill path, izinli temp root altında; atomik yazma, quota ve cleanup.
- Caller cancellation ile temp dosyalar kaldırılır; partial output silinir.

### 14.5 Paralel işlem

- CPU paralelliği tile/scanline batch düzeyinde, **disjoint output slices** veya bağımsız buffer'larla.
- NOX `spawn`, scheduler ve thread paylaşım kuralları esas alınır; QBE/LLVM farklılıkları ile `--strict-spawn-shared` CI'da denenir.
- Global mutable cache, shared buffer write-race ve false sharing'den kaçın.
- Sıralı sonuç ve deterministik output için çıktı tile koordinatına göre birleştirilir.
- İş iptali, timeout, worker exception ve pool shutdown için test şart.
- Paralelleştirme threshold'u benchmark'a dayanmalı; küçük görüntüleri gereksiz paralel yapma.

---

## 15. SIMD, Native Acceleration ve GPU Geleceği

- Önce doğru scalar NOX reference kernel.
- Ardından measurable hotspot'lar için veri yerleşimi, loop fusion, tile cache ve pre-allocation.
- SIMD intrinsics veya native optimized kernel, **yalnızca** derleyicinin belgelenmiş kabiliyeti ve platform matrisi üzerinden kullanılır.
- `lowlevel` yalnızca kanıtlanmış hotspot, pointer lifetime ve alignment testi ile; API tip güvenliği korunur.
- Native adapter olmadan çalışan reference fallback daima korunur.
- İleri GPU backend (Metal/Vulkan/CUDA) yalnızca backend interface soyutlaması olarak planlanabilir; ilk release için zorunlu değildir.
- Tüm hızlandırılmış yollar golden/differential testlerde reference çıktıyla karşılaştırılır.
- AVX2/NEON veya Apple Silicon özel hız iddiaları bench ve CPU-feature gating olmadan yazılmaz.

---

## 16. Platform Desteği ve Capability Matrix

| Hedef | Core kernels | File I/O | Native codecs | Thread | Statü yaklaşımı |
|---|---|---|---|---|---|
| macOS arm64/x64 | Hedef | Hedef | Opt-in | Hedef | Öncelikli CI |
| Linux x64/arm64 | Hedef | Hedef | Opt-in | Hedef | Öncelikli CI |
| Windows x64 | Hedef | Hedef | Opt-in | Hedef | Öncelikli CI |
| wasm32-wasi | Hedef | WASI izinli | Hayır | Mode'a bağlı | Ayrı fixture |
| wasm32-freestanding | Saf piksel çekirdeği | VFS/host'a bağlı | Hayır | Mode'a bağlı | Ayrı fixture |
| Android/iOS | Hedef | OS izinli | Link doğrulamasına bağlı | Hedefe bağlı | Deneysel |
| NOX freestanding OS | Saf compute alt kümesi | Yok | Yok | Hedefe bağlı | Uzun vadeli |

- Platform destek matrisi **başlangıçta hedef**, CI geçmeden başarı rozeti yok.
- WASM: `extern def` ile sistem libjpeg/linking yok; saf NOX codec ile sınırlı veya WASM için özel bağımsız codec derlemesi ayrı ADR gerektirir.
- WASM thread modu `crossOriginIsolated`, host ve runtime gerektirebilir; default modda gerçek paralellik varsayma.
- Mobil: iOS statik arşiv, Android library packaging ve native ABI koşulları ayrı denetlenir.
- Freestanding: byte buffer ve compute kernels, OS dosyalarına/entropiye yaslanamaz.

---

## 17. Diğer NOX Framework'leri ile Entegrasyon

### 17.1 Nyx

- Güvenli multipart upload → MIME sniff → decode limit → EXIF strip → transform → response.
- Thumbnail/avatars, responsive `srcset`, conditional caching için ince `prism.adapters.nyx`.
- HTTP request body limiti sadece Nyx'e değil ayrıca Prism decode budget'a tabidir.
- Kullanıcı dosya adı doğrudan path olarak kullanılmaz; `nox.path.safe_join` veya server-generated key.
- User upload'dan SVG/PDF rasterizasyonu otomatik etkinleştirilmez.

### 17.2 Aether

- Tipli image processing API, request job model, format negotiation ve medya hata kodları.
- Güvenli async iş devretme; HTTP isteğinde büyük synchronous resize default değildir.

### 17.3 Ember / Chronos

- Batch conversion, thumbnail generation, cleanup ve asset processing job'ları.
- Idempotency key: (input content hash + transform spec + output codec version); job retry tekrar çıktı bozmaz.
- Worker crash sonrası temp cleanup ve atomik output transition.
- Chronos sadece periyodik job tetikleyici; pixel engine'e karıştırılmaz.

### 17.4 Aura

- Native pixel buffer → UI image surface adaptörü ve ownership lifetime.
- UI thread bloklanmaması; async worker/preview thumbnail.
- DPI/pixel density ve color profile taşıma.

### 17.5 Glyph / Atlas / Nova / Quartz / Pulse

- Glyph: doğrulanmış güvenli üretilmiş img asset linkleri; görüntü pipeline'ı Glyph içine kopyalanmaz.
- Atlas: histogram/batch metadata sonuçlarını typed tabular veri olarak opsiyonel dışa verme.
- Nova: uzaktan görüntü indirme için ayrı explicit adapter; timeout, max bytes, SSRF ve redirect policy.
- Quartz: codec fuzz, property, cross-platform differential, resampling regression.
- Pulse: decode duration, memory peak, cache hits ve codec failures; input image veya kişisel EXIF loglanmaz.

**Çekirdek kural:** Bu framework'ler *zorunlu dependency olmayacak*. Adapter modülleri opsiyonel, sürüm pinli ve ayrı test edilen entegrasyonlardır.

---

## 18. CLI ve Developer Experience

Önerilen CLI komutu: `prism` (Nox paketinin `bin` tanımı doğrulandıktan sonra).

Örnek amaçlanan alt komutlar:

```text
prism info input.png
prism convert input.png -o output.webp --quality 82
prism resize input.jpg -o thumb.png --width 640 --height 480 --fit contain
prism crop input.png -o cropped.png --x 10 --y 20 --width 300 --height 200
prism strip-metadata input.jpg -o public.jpg
prism batch ./images --format webp --output-dir ./public --dry-run
prism benchmarks --dataset ./benchmarks/fixtures
prism codecs --list
```

- CLI ve kütüphane aynı core API'yi kullanır.
- Komutlar test edilmiş şekilde uygulanmadan README'de 'mevcut' diye gösterilmez.
- `--dry-run` output path, codec ve estimate bilgilerini verir; dosya yazmaz.
- Üzerine yazma varsayılan olarak kapalı; `--overwrite` açık seçim.
- `--max-pixels`, `--max-memory`, `--timeout` ve codec availability görünür.
- Hata mesajında dosya içeriği veya EXIF kişisel bilgi sızdırılmaz.
- TTY/progress uygulaması Rune opsiyonel adapter; çekirdek bağımlılığı olmaz.

---

## 19. Tipli Hata Modeli

`PrismError` üst sınıfı altında aşağıdaki stabil türler planlanır:

- `InvalidImageError`: geçersiz temel görüntü modeli veya input.
- `CorruptDataError`: bozulan, kesilen veya CRC/checksum uyuşmayan medya.
- `UnsupportedFormatError`: format tanındı, desteklenmiyor.
- `CodecUnavailableError`: format prensipte destekli, bu build'de adapter yok.
- `UnsupportedPixelFormatError`: bit depth/color type/alpha kombinasyonu desteklenmiyor.
- `ColorProfileError`: ICC/profile dönüşümü güvenli biçimde yapılamıyor.
- `ResourceLimitError`: dimension/pixel/frame/memory/time/metadata/work budget aşıldı.
- `DimensionError`: crop/resize/stride ve geometrik parametre geçersiz.
- `EncodeError`, `DecodeError`, `ImageIOError`: ilgili etap hataları.
- `PipelineError`, `CancelledError` (NOX'un mevcut istisna semantiğiyle uyumlu), `NativeAdapterError`.

- Bir hata ya typed olarak dönmeli ya fırlatılmalı, ikisi birden sessizce karışmamalı.
- Exception chaining (`raise ... from ...`) mevcut NOX sürümü doğrulanarak kullanılmalı.
- Native decoder hata mesajları kullanıcıya doğrudan yazılmadan sınıflandırılır.
- Hatalı codec girişi hiçbir koşulda memory corruption veya process abort ile bitmemelidir; native adapter'ın izolasyon sınırı buna göre belirlenir.
- Invalid input için fallback'in yanlış görüntü üretmesine izin verilmez.

---

## 20. Güvenlik Threat Modeli

### 20.1 Düşman modeli

Saldırgan kontrol edebilir: dosya içerikleri, metadata, ICC profile, boyut alanları, frame/chunk sayıları, path ve URL parametreleri, output path/format/quality, image pipeline spec ve eşzamanlı istekler.

### 20.2 Korunacak varlıklar

- Sunucu process belleği, CPU süresi, disk kotası ve thread havuzu.
- Dosya sistemi izinli root ve harici ağ erişimi.
- Kullanıcıların özel metadata ve görüntü içerikleri.
- İmzalanmış paketler, codec native bağımlılıkları ve release artefact'ları.

### 20.3 Zorunlu kontroller

- **Decompression bomb:** pixel/frame/decoded byte ratio ve toplam CPU budget.
- **Integer overflow:** boyut/offset/stride/alloc checked arithmetic.
- **Out-of-bounds:** tüm buffer erişimleri dahil header/CRC ve chunk sınırı.
- **Heap exhaustion:** partial decode limit, tile, batch backpressure ve cache quota.
- **Path traversal:** `nox.path.safe_join`, root allowlist, symlink TOCTOU sınırları, temp dosya izolasyonu.
- **SSRF:** URL okuyucu başlangıçta kapalı; açılırsa yalnızca explicit allowlist, DNS-rebinding kontrolü, özel IP blokları/redirect policy ve network deny.
- **Malicious font/profile:** parser limitleri, sandbox native adapter ve fuzz.
- **ZIP/ICC/EXIF recursion:** azami derinlik ve boyut sınırı.
- **Metadata exfiltration:** EXIF GPS vb. default strip politikasını ürün kullanımına göre belirle; sanitize imkânını zorunlu sun.
- **Native dependency CVE:** SBOM, lock/signature, güvenlik taraması ve pinned sürümler.
- **Unsafe output overwrite:** no-clobber, atomik replacement semantics, permission handling.
- **Cache poisoning:** cache key kaynak içeriğine, codec/transform/color ayarlarına bağlı.
- **Command/SQL injection:** pipeline string'i shell'e verilmez; hiçbir sistem komutu user-supplied arguments ile string concat yapılmaz.
- **Resource cancellation:** müşteri bağlantısı düşse bile iş thread'i yetkisiz sınırsız işleyemez.

### 20.4 Güvenlik olayı politikası

SECURITY.md içinde private report yöntemi, desteklenen sürümler, koordineli açıklama, güvenlik testi/patch ve bağımlılık güncelleme prosedürü tanımlanır.

---

## 21. Test Stratejisi ve Doğruluk Sözleşmesi

### 21.1 Test sınıfları

1. **Unit:** checked arithmetic, PixelFormat, stride, buffer, color math.
2. **Golden:** küçük, lisanslı veya kendimiz ürettiğimiz görüntüler ve beklenen hash/pixel arrays.
3. **Round trip:** lossless codec pixel-exact; lossy codec PSNR/SSIM ve metadata politikasına göre.
4. **Differential:** Pillow, ImageMagick, libvips/Sharp veya bağımsız referansla aynı fixture, açık tolerance.
5. **Metamorphic:** flip twice identity, rotate 90×4 identity, crop compose, alpha=255 source-over identity.
6. **Property-based:** rastgele boyut, piksel formatı, kernel ve memory boundary; minimum reproducer.
7. **Fuzz:** PNG/BMP/GIF decoder, EXIF/ICC, palette/chunk/stride ve native adapter wrappers.
8. **Security:** max dims, buffer overflow, truncated headers, decompression bombs, path traversal, SSRF.
9. **Failure injection:** alloc failure, short read/write, interrupted encode, thread kill, cancellation.
10. **Concurrency:** parallel/single result equality, race, deadlock, worker failure, deterministic output.
11. **Platform:** LLVM/QBE macOS/Linux/Windows; wasm/mobile feature matrix.
12. **Compatibility:** public API stability ve codec platform fallback.
13. **Performance:** reproducible memory and throughput regression.

### 21.2 Fixture disiplini

- `tests/fixtures-manifest.json`: dosya, kaynağı, lisansı, SHA256, format varyantı, dimensions, golden output.
- Telifli/özel fotoğrafları fixture'a kontrolsüz ekleme; public-domain veya projede üretilmiş sentetik img seç.
- Her codec için valid/invalid/truncated/adversarial fixture kümesi.
- Public CI mümkünse dış HTTP indirmesine bağımlı değil.
- Referans çıktılar deterministik üretilir; altın testi güncellemesi ayrı review gerektirir.
- Renk profili ve gamma farkları varsa expected pixel tolerance gerekçelendirilmeli.

### 21.3 İlk aşama zorunlu property'ler

- `decode(encode(img, lossless))` pikseller, boyut, renk/alpha modu korunuyorsa eşit.
- `flip_h(flip_h(img))` = img; `flip_v(flip_v(img))` = img.
- `crop(img, x, y, w, h)` exact bölgedeki tüm pikselleri korur.
- `resize(img, original_size, nearest)` identity (belirlenen sampling semantiğinde).
- `composite(transparent, src)` = src (uyumlu alpha/color şartlarında).
- N farklı tile boyutu ile reference image sonuçları eşit veya açık tolerans.
- Invalid dimension, corrupt PNG ve memory exhaustion doğru typed error döndürür.
- Encode başarısızken eski output dosyası hasarsız kalır.

### 21.4 Test komutları ve CI

```bash
noxc version
noxc check prism.nox
noxc check --strict-spawn-shared prism.nox
noxc test
noxc test tests/unit
noxc test tests/codecs
noxc sbom .
```

- `--strict-spawn-shared` yalnızca güncel noxc sürümünde mevcutsa kullanılır; sürüm kapısı belgelenir.
- `noxc test --backend llvm ...` ve `--backend qbe ...` platform desteklediği ölçüde gerçek test dosyasında denenir.
- CI güvenli scalar core yolunu bütün platformlarda, native codec matrix'ini yalnızca destekli host'larda çalıştırır.
- `noxc test` komutu test dosyası derlemesini ve başarısızlık exit code'unu içerir; yalnızca `check` yeterli değildir.
- macOS ARM64, Linux x64 ve Windows x64 release kapısıdır; ARM64 Linux/Windows ek coverage, uygun runner mevcutsa.
- WASM ve mobil, doğrulanmış hedefler için ayrı pipeline ve açık capability matrix; CI çalışmadan 'destekleniyor' denmez.
- C/FFI codec adapter varsa ASan/UBSan benzeri uygun native test, leak/ownership kontrollü harness zorunlu.

---

## 22. Benchmark ve Performans Kapıları

### 22.1 Benchmark seti

| Senaryo | Veri seti | Ölçüm |
|---|---|---|
| Decode PNG/JPEG/WebP | 1/12/48 MP sentetik | latency, throughput, peak RSS |
| Resize | 1920×1080→320×180 ve 8K→1080p | p50/p95, kalite |
| Blur | 4096×4096 RGBA, sigma 2/8 | latency, boundary correctness |
| Composite | 4/16 layer | latency, memory, alpha correctness |
| Histogram/enhance | 8/16-bit (desteklenirse) | throughput |
| Thumbnail batch | 1000 × 2–6 MP | images/s, RSS, CPU |
| Streaming | RAM'i aşan dosya | peak RSS, temp bytes, stability |
| Animated GIF | 100 frame | total decode work, memory |
| WASM scalar | 512/2048 px | perf + parity |

### 22.2 Kıyas disiplini

- Karşılaştırma: Prism native scalar, Prism fast/native, Pillow, Sharp/libvips ve gerekirse ImageMagick.
- Her rakipte **aynı codec, kalite, renk profili, orientation ve çıktı semantiği**; adil olmayan test yayımlama.
- Kaynak girişini ve decode süresini dahil edip etmediğini belirt; cold/warm-cache ayrı.
- CPU/OS/RAM/toolchain, commits, compiler flags, thread sayısı, codec versions, median p50/p95 ve peak RSS kaydedilir.
- En az 10 geçerli tekrar; warmup ve dışlayıcı kriter belirtilir.
- Memory-limited senaryolarda OOM/failure sonuçlarını da göster.
- Regression: kesin performans eşiği ilk F0 baseline'ından türetilir. Ölçümsüz hayali `2x Sharp` taahhüdü yok.

### 22.3 Başlangıç performans hedefleri (garanti değil)

- Eager RGBA8 resize/blur için gereksiz tam görüntü ara kopyalarını kaldırmak.
- 16+ MP resimde peak RSS'i sınırlamak; kritik işlerde tile yolu eager'a göre anlamlı tasarruf göstermeli.
- 4 worker ile uygun büyük görüntülerde tek worker'a göre ölçülebilir throughput artışı sağlamak.
- Thumbnail servisi için p95 latency ve memory budget prod iş yükünden kalibre edilmeli.
- `Image` format conversion microbench'te allocation sayısı ve copies ölçülmeli.
- Fast path ancak sonuç ve bellek kuralları sağlanırsa varsayılanlaştırılmalı.

---

## 23. API / Format / Platform Uyumluluğu ve Sürümleme

- `0.x`: deneyseldir; breaking changes CHANGELOG'da açık.
- `1.0`: API kararlılık sözleşmesi; semver major kırılma için.
- Yeni format desteği, var olan formatın desteklenen bit-depth değişimi ve yeni işlem açıkça release notes'ta.
- Taslak API'ler `experimental` modülünde; stable facade'ye erken ekleme.
- `DecodeOptions`/`EncodeOptions` default değişikliği gözlemlenebilir davranışı etkiliyorsa major/minor politika değerlendirmesi.
- Codec native paketleme matrisi, platform lisansları ve destek durumu release artefact'larında açıklanır.
- Core ve optional native modules ayrı sürüm/feature metadata ile uyumsuz kombinasyonda fail-fast.
- `nox.json` ve `nox.lock` reproducible; tag'lenmiş release üzerinden publish; source label gerçeği yansıtmalı (`nox` / `mixed`).
- `AGENTS.md` plan dokümanı olarak sürüme bağlanır; tamamlanmayan fazlar '✅' yapılmaz.

---

## 24. Dokümantasyon ve Örnekler

Aşağıdaki içerikler test/ci ile güncel tutulmalı:

- Installation, quickstart ve noxc version compatibility.
- API reference (gerçek imzalar, dönüş türleri, error).
- Supported formats: decode/encode/bit depth/alpha/animation/ICC/multi-page.
- Pixel and color semantics, alpha/linear-vs-sRGB örnekleri.
- File and HTTP input security; resource limit profile önerileri.
- Native adapters build/test/distribution ve lisans belgeleri.
- Benchmarks methodology + reproducible scripts + raw results.
- Nyx upload thumbnail / Aura preview / Ember batch examples.
- Platform matrix (hosted, mobile, wasm, freestanding).
- Migration guide, deprecations, changelog ve security guidance.

**Örnek zorunluluk:** README'deki her `.nox` kodu kullanılan `noxc` sürümünde derlenir/çalışır. Sahte API örnekleri `proposal` etiketi taşır ve `examples/` altında kullanıcıya çalışan örnek diye sunulmaz.

---

# 25. İkinci Başlangıçtan Kararlı Sürüme Ayrıntılı Geliştirme Roadmap'i

> Aşağıdaki F0–F16 fazlarının sıra ve feature kapıları bağlayıcıdır. İleriki faz prototiplenebilir; ama önceki fazların kabul kriterleri tamamlanmadan stabil ürün iddiasında bulunulmaz. Her faz sonunda gözlemlenebilir bir demo, test raporu, kullanım örneği ve dokümantasyon güncellemesi gerekir.

## F0 — Discovery, NOX Audit ve Repo Temeli

**Hedef:** Gerçek araç zinciri ve minimum yapı doğrulandı.

Görevler:

- Var olan Prism deposunu/çalışma alanını ve lisans durumunu incele; mevcut kodu koru.
- NOX, Nyx, Nox stdlib yetenek matrisi çıkar; `noxc version`, LLVM/QBE, Windows/macOS/Linux sınırları.
- `nox.json`, `README.md`, `LICENSE`, `SECURITY.md`, `CHANGELOG.md`, CI skeleton, docs skeleton oluştur.
- `Buffer`, BinaryReader/Writer, file I/O, `nox.bytes`, `list[u8]`, `ptr_of`, thread denemeleri.
- Codec native-free/core stratejisini ADR-001 olarak kaydet.
- 1x1 ve 32x32 RGBA prototype buffer + checked offset testleri.

Kabul kriterleri:

- [ ] `noxc check` ve `noxc test` örnekleri geçiyor.
- [ ] Desteklenen/eksik hedef platformlar raporlandı.
- [ ] Belirsiz native codec gereksinimleri `docs/capabilities.md` içinde açık.
- [ ] Başlangıç perf/memory baselines ve fixture manifest hazırlanmış.

## F1 — Safe Pixel Core ve Image Model

**Hedef:** Piksel modeli güvenli, sınır kontrollü ve test edilebilir.

Görevler:

- `Pixel`, `PixelFormat`, `ColorSpace`, `AlphaMode`, `Image`, `MutableImage` ve `ImageView` minimum tanımları.
- RGBA8, RGB8, Gray8 ve checked allocation/stride/index.
- get/set/copy/clone/crop_view, ownership ve mutability contract.
- Error türleri, ResourceLimits; negative test ve leak/resource test.

Kabul kriterleri:

- [ ] Geçerli piksel erişimi exact; tüm invalid indeksler typed hata.
- [ ] Overflow ve oversized allocation tahsisten önce reddediliyor.
- [ ] View parent yaşam süresi, mutable aliasing ve copy davranışı testli.
- [ ] 1000 random küçük image constructor/test vakası geçiyor.

## F2 — Portable Image I/O ve Minimal Codec Suite

**Hedef:** Saf NOX image open/save API'si ilk gerçek formatlarda çalışıyor.

Görevler:

- Header sniffing, PPM/PGM P5/P6, BMP 24/32-bit uncompressed.
- Stream/file/memory adapter, `ImageInfo` probe.
- No-clobber/atomic output policy; symlink/path tests.
- Corrupt/truncated/oversized input fixture.

Kabul kriterleri:

- [ ] Encode/decode lossless round-trip pixel-exact.
- [ ] Format sniff uzantıdan bağımsız.
- [ ] 10k synthetic malformed headers fuzz test process crash olmadan geçiyor.
- [ ] Windows/Linux/macOS en az bir CI job'da I/O testi geçiyor.

## F3 — Geometry and Resampling

**Hedef:** Yaygın resize/crop/rotate/flip/thumbnail işlemleri.

Görevler:

- Crop/pad/flip/rotate 90/180/270 ve arbitrary rotate.
- Nearest, bilinear, bicubic, Lanczos3/area.
- Contain/cover/fit ve orientation normalization.
- Alpha/gamma safe processing, geometry tests.

Kabul kriterleri:

- [ ] Exact geometry properties ve ≥100 golden fixture.
- [ ] Referans resampling görüntüleri açık toleransla karşılaştırılıyor.
- [ ] 8-bit alpha halos/gamma regression test geçiyor.
- [ ] Büyük görüntü resize'da memory budget devrede.

## F4 — PNG Engine (8-bit)

**Hedef:** PNG baseline decode/encode güvenilir.

Görevler:

- Compression capability proof, PNG parser/CRC/filter logic.
- Gray/RGB/RGBA, destekli palette/tRNS kombinasyonları.
- Baseline encoder, chunk order, metadata policy.
- Zlib/DEFLATE desteği eksikse native-free imkan araştırma ADR ve onaylı fallback.

Kabul kriterleri:

- [ ] PNG resmi/bağımsız fixture'larla exact pixel decode.
- [ ] CRC/chunk overflow/decompression-bomb testleri geçiyor.
- [ ] Output harici okuyucular tarafından açılıyor.
- [ ] Unsupported 16-bit/Adam7 varyantları doğru typed error (henüz desteklenmiyorsa).

## F5 — Color, Alpha ve Metadata Hygiene

**Hedef:** Doğru renk ve güvenli metadata yönetimi.

Görevler:

- sRGB↔LinearSRGB, straight↔premultiplied alpha.
- EXIF orientation 1–8 ve strip/private metadata.
- ICC carry-through, color conversion capability gating.
- Renk golden ve blend correctness test.

Kabul kriterleri:

- [ ] Alpha 0/255, translucency ve linear blending fixture'ları geçiyor.
- [ ] EXIF orientation round-trip double-rotate yapmıyor.
- [ ] GPS metadata strip fiilen doğrulanmış.
- [ ] ICC dönüştürme olmayan build bunu iddia etmiyor.

## F6 — Filters and Enhancement

**Hedef:** Pillow seviyesine yaklaşan yaygın piksel işlemleri.

Görevler:

- Grayscale/invert/brightness/contrast/saturation/gamma.
- Generic convolution, Gaussian, sharpen/unsharp, Sobel/Laplacian.
- Histogram ve basic stats; morphology/median.
- Kernel limits, border modes, numeric correctness.

Kabul kriterleri:

- [ ] Scalar reference vs optional optimized code tutarlı.
- [ ] Sigma/kernel/NaN invalid inputs hata veriyor.
- [ ] Tile sınırı/transparent edge artefaktı testleri.
- [ ] Performance baseline ve peak RSS kaydedilmiş.

## F7 — Drawing, Mask and Compositing

**Hedef:** Programatik görsel üretimi.

Görevler:

- Rectangle, lines, ellipses, polygons, basic antialias.
- Layer/mask/compositing ve Porter-Duff source-over.
- Blend modes: normal/multiply/screen başlangıç.
- Bitmap font temel, lisanslı/testli.

Kabul kriterleri:

- [ ] Blend alpha edge golden geçiyor.
- [ ] Shape geometry ve clipping random testleri.
- [ ] Text output deterministik ve font lisansı belgeli.
- [ ] Render başarısızlığı kaynak görseli bozmuyor.

## F8 — Native Codec Adapters (JPEG, WebP)

**Hedef:** Gerçek web image pipeline'ı desteklemek.

Görevler:

- Native codec adapter ABI ve build şablonu.
- libjpeg-turbo, libwebp seçeneklerini lisans, platform, CVE ve dağıtım bakımından değerlendir.
- JPEG baseline/progressive ve WebP lossless/lossy desteği kapsamını netleştir.
- Quality/subsampling/metadata/ICC options; codec-unavailable fallback.
- Native failure/memory leak test harness ve sandbox stratejisi.

Kabul kriterleri:

- [ ] Her etkin adapter kendi codec fixture/corruption testini geçiyor.
- [ ] Native codec olmadan core derleniyor ve portable formatları işliyor.
- [ ] Package/source label ve lisanslar doğru.
- [ ] En az macOS/Linux/Windows destekli dağıtım kararı belgeli.

## F9 — Animation and Advanced Formats

**Hedef:** GIF ve ileri formatların kontrollü desteği.

Görevler:

- Frame/Animation modeli, GIF palette/LZW/disposal/loop.
- Animated WebP veya APNG adapter pilotu, öncelik matrisi.
- PNG 16-bit/Adam7, TIFF multi-page seçenekleri için capability planı.
- Frame memory/work limits, frame iterator.

Kabul kriterleri:

- [ ] GIF dispose 2/3 ve transparent frame golden.
- [ ] 100 frame memory/resource limit testi.
- [ ] Yanlış frame duration/offset/bad LZW reddediliyor.
- [ ] Multi-frame decode için pixel-equivalence golden.

## F10 — Pipeline DAG, Planner ve Tile Engine

**Hedef:** Uygulama dostu lazy image operations.

Görevler:

- Immutable Pipeline AST/graph, compile-to-plan.
- Safe rewrite optimizations, color/alpha ordering constraints.
- Tile executor, halo, cache key, materialization triggers.
- Error trace/source node id ve memory scheduling.

Kabul kriterleri:

- [ ] Eager vs lazy aynı görüntü (tolerans dahil) üretir.
- [ ] Rastgele 1000 geçerli pipeline expression karşılaştırması.
- [ ] Tile seams/halos için özel golden set.
- [ ] Memory upper bound ve cache eviction fiilen testli.

## F11 — Streaming, Large Image ve Batch Processing

**Hedef:** RAM'i aşan iş akışları.

Görevler:

- `ImageReader/Writer`, chunk/tile streaming, backpressure.
- Temp spill, cleanup, no-clobber atomik commit.
- Batch API, deterministic output, concurrency.
- Encode streaming desteği codec'e göre dürüst capability.

Kabul kriterleri:

- [ ] Tasarlanan streaming senaryosu tüm dosyayı RAM'e yüklemiyor.
- [ ] Temp disk quota, cancellation ve crash cleanup testi.
- [ ] 1000 image batch throughput bench ölçülmüş.
- [ ] Bir iş hata verdiğinde diğerleri için policy açık ve testli.

## F12 — Advanced Color/Type and Professional Features

**Hedef:** Daha yüksek doğruluk ve profesyonel çıktı.

Görevler:

- 16-bit/float pixel types, channel-plane abstractions.
- ICC profile conversion (doğrulanmış adaptör), Display P3, CMYK input.
- Tone mapping/dithering ve color quantization.
- Canny/CLAHE ve yeni blend/shape/text özellikleri.

Kabul kriterleri:

- [ ] Float/16-bit numeric reference test.
- [ ] Profil dönüştürme golden (renk delta metriği).
- [ ] Unsupported color space fail-fast.
- [ ] Perf/memory maliyeti ve format matrix güncel.

## F13 — Ecosystem Integrations ve CLI

**Hedef:** Prism, NOX uygulamalarında kolayca tüketilebilir.

Görevler:

- `prism` CLI, `--dry-run`, explicit overwrite/options.
- Nyx upload/thumbnail, Aether API, Aura preview, Ember batch.
- Nova download adapter (açık SSRF policy), Glyph/Atlas/Pulse opsiyonel entegrasyon.
- Paket publish/doğru import path/README examples.

Kabul kriterleri:

- [ ] `prism info`, `convert`, `resize`, `batch` gerçek fixture ile çalışıyor.
- [ ] Nyx API 5MB/100MP güvenlik sınırı testli.
- [ ] Optional adapters olmadan core kullanılabiliyor.
- [ ] Örneklerin tümü gerçek `noxc` ile derleniyor.

## F14 — Portability (WASM / Mobile / Freestanding Subset)

**Hedef:** Platform başına gerçek capability matrisi.

Görevler:

- wasm32-wasi / freestanding saf çekirdek fixture.
- Android/iOS compile/link ve runner testleri (mevcut toolchain ile).
- Core compile without filesystem/network/native.
- Renderer/FFI platform-specific imkan incelemesi.

Kabul kriterleri:

- [ ] Destek ilan edilen her hedefte gerçek smoke/fixture geçiyor.
- [ ] Native codec olmayan hedeflerde typed `CodecUnavailableError`.
- [ ] WASM memory budget ve input limitleri testli.
- [ ] `docs/compatibility.md` matrix sonucu yansıtıyor.

## F15 — Security, Fuzz, Performance ve Release Candidate

**Hedef:** Production readiness.

Görevler:

- Decoder/header/EXIF/ICC fuzz + corpus minimization.
- Native CVE / SBOM / provenance / license audit.
- Cross-platform CI, leak/ASan uygun araçlarla, stress.
- Benchmark Pillow/Sharp/libvips karşılaştırması, reproducible rapor.
- API review, docs complete, security report.

Kabul kriterleri:

- [ ] High/critical açık vulnerability yok; bulunanlar izlenebilir şekilde düzeltilmiş.
- [ ] 24 saatlik batch stress/negative workload planı veya uygulanmış sonuç raporu.
- [ ] Üç masaüstü platformda core+codec regression yeşil.
- [ ] Güncel ve raw sonuçları içeren benchmark raporu.

## F16 — Prism 1.0 Stable Release

**Hedef:** Kararlı public contract ve belgeli destek matrisi.

Kabul kriterleri:

- [ ] `prism.nox` API, garanti kapsamı ve semver stabilite belgeli.
- [ ] Desteklenen codec/kodlama/bit-depth/alpha platform matrisi dürüst.
- [ ] README quickstart ve 10+ gerçek dünya örneği tested.
- [ ] CI release artifacts, SBOM/signatures, registry metadata hazır.
- [ ] Migration/deprecation/security/support politikaları yazılı.
- [ ] Takip planı: optimized SIMD, GPU, AVIF/HEIF, TIFF, advanced type/graphics ayrı backlog.

---

## 26. Fazların Öncelik Kuralları

**MVP sınırı:** F0–F4 + minimum F5; RGBA8/RGB8/Gray8, PPM/BMP/PNG temel, crop/resize/rotate, secure file I/O. İlk milestone için JPEG native bağımlılığına girme zorunluluğu yok; web'de gerçek kullanılabilirlik için F8 ayrıca önemlidir.

**Web/Backend kullanım sürümü:** F0–F8 + F13 Nyx entegrasyonu ve geçerli güvenlik kapıları.

**Professional/High-performance:** F9–F12, F14–F15.

**Stable 1.0:** API'nin kapsamı, doğrulanmış özellikler ve CI/test kalitesi esas; her ileri/experimental formatı tamamlamak zorunlu değildir. F16'daki açık hedeflerin karşılanması gerekir; ileri codec'ler experimental kalabilir.

Her faz tamamlandığında `docs/roadmap.md` tablosunda: `feature`, `status`, `noxc_version`, `target_platforms`, `tests`, `benchmarks`, `known_limits`, `commit`, `date` alanları güncellenir.

---

## 27. Agent Çalışma Disiplini ve PR Şablonu

Her PR/görevde aşağıdaki bölümler zorunlu:

```text
Objective:
- Hangi feature/faz?

Current-state audit:
- Dosyalarda önceden neler vardı?
- Kaynak NOX sürümü nedir?

Design:
- Public API değişikliği var mı?
- Memory, alpha, color, codec ve security etkisi nedir?
- Hedef platform/fallback nedir?

Implementation:
- Hangi modüller ve neden?
- Hangi alternatifler reddedildi?

Verification:
- noxc check/test çıktıları
- Codec golden/differential/fuzz/limit sonuçları
- LLVM/QBE/platform sonuçları
- Benchmark yöntem ve sonuçlar (varsa)

Compatibility:
- Breaking değişiklik?
- API/codec feature matrix değişimi?
- Native ABI / lisans etkisi?

Docs:
- README/API/roadmap/changelog/security güncellemesi

Known limitations:
- Kalan kusurlar ve açık görevler
```

### 27.1 Yasak agent davranışları

- Derlenmeyen `.nox` örneklerini 'kullanıma hazır' diye yayımlamak.
- Güvenlik sınırlarını testleri geçsin diye sessizce kaldırmak.
- Büyük görseli RAM'de açıp buna streaming demek.
- Gereksiz native/Node/Python backend eklemek.
- `nox` derleyicisine geniş çaplı kaynak kodu değişikliği yaparak Prism işini bitmiş saymak.
- Var olmayan `nox.image` veya codec fonksiyonunu gerçekmiş gibi kullanmak.
- Hardcoded macOS/Linux path veya codec path'ini tek kurulum yöntemi yapmak.
- Bir platformda test edilmemiş özelliği tüm platformlarda destekli ilan etmek.
- Referans kütüphaneden lisans ihlal edecek kod kopyalamak.
- FFI/native ownership, buffer/stride ve release hatalarını görmezden gelmek.
- Github release/package publish işlemini kullanıcıya bildirmeden veya istenen release planına uymadan yapmak.

---

## 28. Definition of Done — Her Özellik İçin

- [ ] Girdi/çıktı/yan etki/bellek/renk/alpha semantiği yazılı.
- [ ] Public API statik tipli, test edilmiş, uygun modülde.
- [ ] Input ve output resource limitleri uygulanıyor.
- [ ] Checked arithmetic ve buffer boundaries testli.
- [ ] En az bir normal, bir negatif, bir edge ve uygun golden/properties test var.
- [ ] Exception/resource cleanup testli.
- [ ] NOX `check`, `test`, hedef backend/platform testleri raporlu.
- [ ] İçerik codec ise corrupt/fuzz ve format matrisi güncel.
- [ ] Yeni native dependency varsa lisans, ABI ve SBOM geçerli.
- [ ] Benchmark claim varsa reproducible raw veri var.
- [ ] README/docs/changelog/roadmap güncel.
- [ ] Kaynak kökte `nox.json` ve `nox.lock` davranışı uyumlu.
- [ ] Güvenlik veya geriye uyumluluk ihlali yok.

---

## 29. İlk Otonom Görev: Güvenli Minimal Çekirdek

**Agent'ın hemen başlatacağı iş F0 ve F1'in ilk dikey dilimidir.** Doğrudan JPEG/WebP/libvips entegrasyonuyla başlamayacak.

1. Repoyu keşfet; bu dosyayı koru.
2. NOX sürümünü ve Buffer/bytes/binary/fs semantiklerini gerçek compilation probe ile doğrula.
3. `core/errors.nox`, `core/limits.nox`, `core/pixel.nox`, `core/image.nox` taslaklarını yaz.
4. RGBA8 `Image(2,2)` oluştur, dört pikseli set/read, clone ve crop testini çalıştır.
5. Checked arithmetic ile `width/height/stride/byte_length` limitlerini uygula.
6. En az 20 negatif test: 0/negatif boyut, max int64, alt/üst buffer boundary, invalid stride, oversize, alias mutation.
7. Basit `examples/create_checkerboard.nox` uygulamasını PPM P6 output'a kadar getir.
8. macOS/Linux/Windows için başlangıç CI kur; varsa bozuk platformu açıkça raporla.
9. Değişikliklerden sonra `docs/capabilities.md`, `docs/architecture.md`, `docs/roadmap.md` güncelle.
10. Kullanıcıya doğrulananları, kalanları ve bir sonraki faz olan F2 için net adımları içeren rapor sun.

**Başarı tanımı:** NOX ile derlenen, test edilen, güvenli bir piksel çekirdeği + gerçek PPM çıktı + basit görüntü fixture'ı. Sonraki işleri bu temel üzerine inşa et.

---

## 30. Son Mimari Pusula

> **Prism, NOX için bir görüntü gösterme uygulaması değil; güvenli medya girdisini doğrulanabilir native piksel işlemlerine dönüştüren, performansı ölçülebilir, taşınabilir bir görüntü işleme altyapısıdır.** Çekirdek saf NOX kalır; codec'ler yalnızca gerekli ve izole noktalarda native teknoloji kullanır. Özellik kapsamını büyütürken güvenlik, renk doğruluğu ve kullanıcıların verilerini koruma ilkelerinden vazgeçilmez.

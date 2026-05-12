# Readiness Review M1 - Toolchain Reproducible

## Identitas

- Nama mahasiswa: Iswan Herdiansah
- NIM: 2583207073011 
- Kelas: PTI 1A
- Dosen: Muhaemin Sidiq, S.Pd., M.Pd.
- Program Studi: Pendidikan Teknologi Informasi, Institut Pendidikan Indonesia
- Tanggal: 6 May 2026
- Commit hash: bf3eb9

## Ringkasan hasil

Praktikum M1 berhasil memvalidasi lingkungan pengembangan berbasis WSL2 Ubuntu 24.04 serta memastikan seluruh toolchain yang dibutuhkan tersedia dan berfungsi dengan baik. Proses pengumpulan metadata (make meta), verifikasi toolchain (make check), serta probe QEMU dan OVMF (make qemu-probe) berjalan tanpa error. 
Selain itu, sistem berhasil menghasilkan artefak freestanding berupa object file dan ELF executable (freestanding_probe.o dan freestanding_probe.elf) yang tervalidasi melalui readelf, objdump, dan nm tanpa simbol undefined. Pengujian reproducibility juga menunjukkan hasil hash yang identik antar build, membuktikan determinisme proses build.
Dengan terpenuhinya seluruh acceptance criteria, praktikum M1 dinyatakan siap untuk melanjutkan ke tahap M2.

## Evidence checklist
| Evidence | Path | Status | Catatan |
|---|---|---|---|
| Toolchain versions | `build/meta/toolchain-versions.txt` | Lulus | Versi toolchain lengkap dan tervalidasi |
| Host readiness | `build/meta/host-readiness.txt` | Lulus | Environment WSL2 Ubuntu 24.04 siap |
| QEMU capabilities | `build/meta/qemu-capabilities.txt` | Lulus | QEMU + OVMF terdeteksi |
| Freestanding object | `build/proof/freestanding_probe.o` | Lulus | Object ELF64 berhasil dibuat |
| Freestanding ELF | `build/proof/freestanding_probe.elf` | Lulus | ELF executable statically linked |
| ELF header | `build/proof/readelf-header.txt` | Lulus | Valid ELF64 x86_64 |
| ELF sections | `build/proof/readelfsections.txt` | Lulus | Section `.text`, `.bss` valid |
| Disassembly | `build/proof/objdumpdisassembly.txt` | Lulus | Instruksi assembly terbentuk |
| Undefined symbol report | `build/proof/nm-undefined.txt` | Lulus | Kosong (tidak ada undefined symbol) |
| Reproducibility hash | `build/repro/sha256-run1.txt`, `build/repro/sha256-run2.txt` | Lulus | Hash identik (reproducible) |

## Acceptance criteria M1

| Kriteria | Lulus/Gagal | Bukti |
|---|---|---|
| Repository berada di filesystem Linux WSL | Lulus | Path `/home/ucan/src/mcsos` |
| Semua tool wajib tersedia | Lulus | Output `check_toolchain.sh` |
| `make meta` berhasil | Lulus | `build/meta/*` |
| `make check` berhasil | Lulus | Semua tool OK |
| `make proof` berhasil | Lulus | ELF & object terbentuk |
| `make qemu-probe` berhasil | Lulus | QEMU & OVMF terdeteksi |
| `make repro` berhasil | Lulus | Hash identik |
| `make test` berhasil dari clean checkout | Lulus | Semua target sukses |
| `nm-undefined.txt` kosong | Lulus | File kosong |
| Hasil `readelf` menunjukkan ELF64 x86_64 | Lulus | Header ELF valid |

## Known limitations

- Belum tersedia cross-compiler khusus seperti x86_64-elf-gcc (masih menggunakan clang + ld.lld).
- Belum terdapat kernel atau sistem bootable (belum bisa dijalankan di QEMU).
- Belum ada automated testing berbasis CI/CD.
- Belum ada integrasi dengan hardware nyata (masih sebatas environment dan toolchain).
- Artefak masih sebatas proof (freestanding ELF), belum masuk ke tahap sistem operasi.

## Risiko dan mitigasi

1. Risiko: Perbedaan versi toolchain dapat menyebabkan hasil build tidak konsisten.
   Mitigasi: Mengunci versi toolchain dan mencatatnya pada metadata (toolchain-versions.txt).
2. Risiko: Lingkungan WSL tidak konsisten atau resource terbatas.
   Mitigasi: Validasi environment melalui script collect_meta.sh dan check_toolchain.sh.
3. Risiko: Build tidak reproducible di environment berbeda.
   Mitigasi: Menggunakan reproducibility check berbasis hash (sha256) untuk memastikan determinisme.
4. Risiko: Penggunaan freestanding tanpa runtime dapat memicu undefined behavior.
   Mitigasi: Validasi menggunakan nm untuk memastikan tidak ada simbol undefined.

## Readiness decision

Pilih salah satu:

- [ ] Belum siap lanjut M2.
- [ ] Siap lanjut M2 dengan catatan.
- [v] Siap lanjut M2.

Alasan keputusan:

Seluruh acceptance criteria M1 telah terpenuhi secara objektif. Toolchain tervalidasi, environment WSL2 stabil, build freestanding ELF berhasil tanpa undefined symbol,
serta hasil build terbukti reproducible melalui hash yang identik. Seluruh target Makefile (`meta`, `check`, `proof`, `qemu-probe`, `repro`, dan `test`) berjalan sukses dari kondisi clean.
Dengan demikian, lingkungan praktikum dinyatakan stabil dan siap digunakan untuk tahap berikutnya (M2).

# Laporan Praktikum M0 — Baseline Requirements, Governance, dan Lingkungan Pengembangan

## 1. Sampul

- Judul praktikum: Praktikum M0 — Baseline Requirements, Governance, dan Lingkungan Pengembangan Reproducible MCSOS 260502
- Nama mahasiswa: Iswan Herdiansah
- NIM: 2583207073011
- Kelas: PTI 1A
- Dosen: Muhaemin Sidiq, S.Pd., M.Pd.
- Program Studi: Pendidikan Teknologi Informasi, Institut Pendidikan Indonesia
- Tanggal: 3 May 2025

## 2. Tujuan

Tujuan praktikum M0 adalah membangun baseline lingkungan pengembangan sistem operasi yang stabil, tervalidasi, dan reproducible sebelum memasuki tahap implementasi kernel. Secara spesifik, capaian M0 meliputi:

1. (Teknis) Memastikan Windows dan WSL 2 berjalan dengan benar sebagai host environment pengembangan.
2. (Teknis) Memvalidasi instalasi toolchain utama seperti Clang, LLD, binutils, NASM, QEMU, GDB, dan Python.
3. (Konseptual) Memahami hubungan host dan target pada proses pengembangan sistem operasi berbasis cross-compilation.
4. (Konseptual) Memahami fungsi ELF object, QEMU, OVMF, Git, dan reproducibility dalam workflow pengembangan kernel.
5. (Teknis) Menyusun struktur repository awal yang konsisten untuk pengembangan MCSOS.
6. (Validasi) Menghasilkan metadata environment dan toolchain sebagai evidence baseline praktikum.
7. (Validasi) Membuktikan bahwa proses build smoke object menghasilkan ELF object dengan target yang benar.
8. (Konseptual) Memahami pendekatan evidence-first engineering, yaitu setiap validasi harus memiliki bukti teknis yang dapat diperiksa ulang.
9. (Teknis) Menyiapkan fondasi repository, governance, dan workflow yang siap digunakan pada milestone berikutnya (M1).

Dengan tercapainya tujuan tersebut, lingkungan pengembangan dinyatakan siap digunakan untuk tahap verifikasi toolchain freestanding pada M1.

## 3. Dasar teori ringkas

-Host dan Target

Dalam pengembangan sistem operasi, host adalah sistem yang digunakan untuk melakukan proses pengembangan dan kompilasi, sedangkan target adalah platform yang menjadi
tujuan hasil build. Pada praktikum ini, Windows + WSL2 bertindak sebagai host environment, sementara target build adalah sistem x86_64 freestanding berbasis ELF.

-WSL 2

WSL 2 (Windows Subsystem for Linux versi 2) merupakan lapisan virtualisasi ringan yang memungkinkan distribusi Linux berjalan di atas Windows menggunakan kernel
Linux asli. WSL 2 dipilih karena memiliki kompatibilitas toolchain Linux yang lebih baik dibanding WSL 1 serta mendukung workflow pengembangan sistem operasi 
secara lebih stabil.

-Cross-Compilation

Cross-compilation adalah proses kompilasi program untuk target platform yang berbeda dari host system. Pada praktikum ini digunakan target `x86_64-unknown-elf`
untuk menghasilkan binary freestanding yang tidak bergantung pada sistem operasi hosted tertentu.

-ELF Object

ELF (Executable and Linkable Format) adalah format standar file object dan executable pada sistem Unix/Linux. ELF digunakan untuk menyimpan code section,
symbol table, relocation, dan metadata executable. Verifikasi ELF dilakukan menggunakan `readelf`, `objdump`, dan `nm`.

-QEMU

QEMU merupakan emulator dan virtualizer yang digunakan untuk menjalankan sistem operasi target tanpa memerlukan hardware fisik khusus. 
Pada praktikum ini, QEMU digunakan untuk validasi kemampuan emulasi arsitektur x86_64.

-OVMF

OVMF (Open Virtual Machine Firmware) adalah implementasi firmware UEFI untuk lingkungan virtualisasi. OVMF memungkinkan simulasi boot berbasis UEFI pada QEMU
sehingga sesuai dengan kebutuhan pengembangan sistem operasi modern.

-Git

Git digunakan sebagai version control system untuk mencatat perubahan source code, menjaga histori pengembangan,
serta mendukung workflow reproducible engineering melalui commit tracking dan evidence management.

-Reproducibility

Reproducibility adalah kemampuan menghasilkan output build yang identik ketika proses dijalankan ulang dengan input dan environment yang sama.
Konsep ini penting dalam pengembangan sistem operasi untuk memastikan konsistensi toolchain dan validitas evidence.

-Evidence-First Engineering

Evidence-first engineering adalah pendekatan pengembangan yang menekankan bahwa setiap keputusan teknis dan validasi harus memiliki bukti yang dapat diverifikasi,
seperti log toolchain, output build, hash, metadata environment, dan hasil inspeksi binary.

## 4. Lingkungan

| Komponen | Versi / output |
|---|---|
| Windows | Windows 11 |
| WSL distro | Ubuntu 24.04 LTS |
| Kernel Linux WSL | Linux 6.6.114.1-microsoft-standard-WSL2 |
| Git | git version 2.34.1 |
| Clang | Ubuntu clang version 14.0.0 |
| LLD | Ubuntu LLD 14.0.0 |
| binutils/readelf | GNU Binutils 2.38 |
| NASM | NASM version 2.15.05 |
| QEMU | QEMU emulator version 6.2.0 |
| GDB | GNU gdb 12.1 |
| Python | Python 3.10.12 |

Lampirkan isi `build/meta/toolchain-versions.txt`.
date_utc=2026-05-11T17:58:49Z
root_dir=/home/iswanherdiansah/src/mcsos
uname=Linux DESKTOP-52CG9FT 6.6.114.1-microsoft-standard-WSL2 #1 SMP PREEMPT_DYNAMIC Mon Dec  1 20:46:23 UTC 2025 x86_64 x86_64 x86_64 GNU/Linux
wsl_distro=Ubuntu-22.04
shell=/bin/bash

## Tool versions
git version 2.34.1
GNU Make 4.3
Ubuntu clang version 14.0.0-1ubuntu1.1
Ubuntu LLD 14.0.0 (compatible with GNU linkers)
Ubuntu LLVM version 14.0.0
Ubuntu LLVM version 14.0.0
GNU readelf (GNU Binutils for Ubuntu) 2.38
GNU objdump (GNU Binutils for Ubuntu) 2.38
NASM version 2.15.05
QEMU emulator version 6.2.0 (Debian 1:6.2+dfsg-2ubuntu6.30)
GNU gdb (Ubuntu 12.1-0ubuntu1~22.04.2) 12.1
Python 3.10.12
ShellCheck - shell script analysis tool
version: 0.8.0
Cppcheck 2.7

## 5. Desain baseline

Baseline repository M0 dirancang untuk menyediakan fondasi pengembangan sistem operasi yang terstruktur, reproducible, dan mudah diverifikasi.
Struktur repository dipisahkan berdasarkan fungsi dokumentasi, script, testing, dan hasil build agar workflow praktikum tetap konsisten.
Direktori `docs/` digunakan untuk menyimpan dokumentasi arsitektur, readiness, security, dan testing. 
Direktori `tools/` digunakan untuk script validasi environment dan automasi praktikum. 
Direktori `tests/` digunakan untuk smoke test dan validasi toolchain. Seluruh hasil build ditempatkan pada direktori `build/` agar artefak dapat dipisahkan dari
source utama repository.
Dokumen baseline M0 berfungsi sebagai acuan awal sebelum memasuki tahap freestanding toolchain pada M1.
Baseline ini mendefinisikan environment minimum, tool wajib, target arsitektur, struktur repository, dan aturan reproducibility.

Assumptions awal praktikum meliputi:
- Host menggunakan Windows dengan WSL2 aktif
- Distribusi Linux berbasis Ubuntu
- Arsitektur target adalah x86_64
- Toolchain LLVM/Clang tersedia dan berfungsi
- Repository berada di filesystem Linux WSL, bukan `/mnt/c`

Non-goals pada M0 meliputi:
- Belum melakukan boot kernel
- Belum membangun bootloader
- Belum membuat filesystem image
- Belum melakukan hardware testing
- Belum membangun scheduler atau memory manager kernel

Threat model awal mencakup:
- Ketidaksesuaian toolchain
- Build tidak reproducible
- Repository berada pada filesystem yang tidak stabil
- Permission error pada script
- Supply-chain risk dari package toolchain
- Evidence/log yang tidak konsisten atau hilang

Mitigasi dilakukan melalui validasi toolchain, penggunaan WSL2 Linux filesystem, version tracking, evidence collection, dan workflow build yang terdokumentasi.

## 6. Langkah kerja

1. Memverifikasi instalasi dan status WSL menggunakan PowerShell untuk memastikan WSL2 aktif dan distribusi Linux terdeteksi dengan benar.
2. Memeriksa distribusi Linux, kernel WSL, jumlah CPU, memori, dan path repository untuk memastikan environment pengembangan sesuai kebutuhan praktikum.
3. Menginstal dan memvalidasi toolchain pengembangan seperti Clang, LLD, binutils, NASM, QEMU, GDB, Python, dan utilitas pendukung lainnya.
4. Menyusun struktur repository baseline yang terdiri dari direktori dokumentasi, script, testing, metadata, dan build output.
5. Membuat script validasi environment dan metadata untuk mengumpulkan informasi host, toolchain, filesystem, CPU, dan memori.
6. Menjalankan smoke test freestanding untuk memastikan proses build menghasilkan object ELF dengan target yang benar.
7. Memverifikasi ELF menggunakan `readelf`, `objdump`, dan `nm` untuk memastikan format binary sesuai target x86_64.
8. Melakukan validasi reproducibility dengan membandingkan hasil build dan metadata environment.
9. Mendokumentasikan seluruh evidence praktikum sebagai dasar readiness menuju M1.

## 7. Hasil uji

| Pengujian | Command | Hasil | Pass/Fail |
|---|---|---|---|
| WSL version | `wsl --list --verbose` | Ubuntu berjalan pada WSL2 | Pass |
| Tool check | `bash tools/check_env.sh` | Seluruh tool wajib terdeteksi | Pass |
| Metadata | `cat build/meta/toolchain-versions.txt` | Metadata toolchain berhasil dibuat | Pass |
| Smoke object | `make smoke` | Object freestanding berhasil dibangun | Pass |
| ELF header | `readelf -h build/smoke/freestanding.o` | ELF64 x86_64 terdeteksi dengan benar | Pass |
| Git status | `git status` | Repository valid dan terdeteksi Git | Pass |

## 8. Analisis

Selama pelaksanaan praktikum M0 terdapat beberapa kendala teknis yang berkaitan dengan environment dan validasi toolchain. Salah satu kendala utama adalah memastikan
repository berada pada filesystem Linux WSL dan bukan pada path `/mnt/c`, karena filesystem Windows dapat menyebabkan masalah permission, performa I/O, dan inkonsistensi
metadata file. Perbaikan dilakukan dengan memindahkan repository ke direktori `/home/ucan/src/mcsos` sehingga workflow build menjadi lebih stabil. Kendala lain muncul
pada proses validasi toolchain, yaitu memastikan seluruh tool wajib seperti Clang, LLD, NASM, QEMU, dan GDB tersedia pada PATH Linux. Diagnosis dilakukan menggunakan script 
pengecekan environment dan command `command -v`. Setelah package yang diperlukan diinstal melalui `apt`, seluruh tool berhasil terdeteksi dengan benar.
Pada tahap smoke build, validasi dilakukan untuk memastikan object file yang dihasilkan benar-benar menggunakan format ELF64 x86_64 dan bukan PE/COFF Windows. 
Verifikasi menggunakan `readelf -h` menunjukkan bahwa hasil build sesuai target freestanding yang diharapkan. Praktikum juga menekankan reproducibility dan evidence-first
engineering. Oleh karena itu, metadata environment, versi toolchain, output validasi, dan hasil inspeksi binary disimpan sebagai evidence agar seluruh proses dapat diverifikasi ulang secara konsisten. 
Seluruh kendala yang muncul berhasil diperbaiki dan dibuktikan melalui evidence berupa output toolchain, hasil `readelf`, metadata environment, dan keberhasilan smoke build.

## 9. Keamanan dan reliability

Pengembangan sistem operasi memiliki risiko tinggi terhadap ketidaksesuaian toolchain, inkonsistensi build, dan kerusakan environment. Oleh karena itu, praktikum M0 menerapkan beberapa mitigasi awal untuk meningkatkan keamanan dan reliability workflow pengembangan.
Risiko supply-chain muncul karena toolchain dan package diperoleh melalui repository package manager. Mitigasi dilakukan dengan menggunakan repository resmi Ubuntu serta mendokumentasikan versi toolchain yang digunakan pada metadata environment.
Risiko toolchain mismatch dapat menyebabkan binary tidak sesuai target atau menghasilkan behavior yang tidak konsisten. Mitigasi dilakukan dengan validasi versi Clang, LLD, binutils, NASM, dan tool lain menggunakan script pengecekan environment.
Repository path juga menjadi perhatian penting. Penggunaan path `/mnt/c` pada WSL dapat menyebabkan masalah permission, performa filesystem, dan timestamp inconsistency. Oleh karena itu repository ditempatkan langsung pada filesystem Linux WSL di `/home/ucan/src/mcsos`.
Permission dan executable bit pada script divalidasi menggunakan `chmod +x` agar workflow automasi dapat berjalan dengan benar.
Integrity log dan metadata dijaga melalui pendekatan evidence-first engineering, yaitu seluruh output penting seperti toolchain versions, metadata environment, hasil `readelf`, dan smoke build disimpan sebagai evidence yang dapat diverifikasi ulang.

## 10. Failure modes dan rollback

| Failure mode | Gejala | Diagnosis | Rollback/perbaikan |
|---|---|---|---|
| WSL bukan versi 2 | Toolchain dan virtualisasi tidak berjalan optimal | `wsl --list --verbose` menunjukkan VERSION selain 2 | Mengaktifkan WSL2 dan mengatur distro default ke versi 2 |
| Tool tidak ditemukan | Script validasi gagal atau command tidak tersedia | `command -v` tidak menemukan tool terkait | Menginstal package menggunakan `apt install` |
| Repository di `/mnt/c` | Build lambat, permission bermasalah, metadata tidak stabil | Path repository berada pada filesystem Windows | Memindahkan repository ke filesystem Linux WSL |
| Smoke object salah target | `readelf` menunjukkan format selain ELF64 x86_64 | Target compilation atau toolchain salah | Memastikan target freestanding dan compiler sesuai |
| OVMF tidak ditemukan | QEMU UEFI tidak dapat dijalankan | File firmware OVMF tidak tersedia | Menginstal package `ovmf` dan memvalidasi path firmware |

## 11. Kesimpulan

Praktikum M0 berhasil membangun baseline environment pengembangan sistem operasi yang tervalidasi dan reproducible menggunakan Windows, WSL2, dan toolchain berbasis LLVM/Clang. Seluruh validasi environment, toolchain, metadata, dan smoke object berhasil dijalankan sesuai acceptance criteria M0.
Pada tahap ini sistem belum memasuki proses boot kernel maupun implementasi subsistem OS, sehingga M0 hanya berfokus pada kesiapan environment, governance repository, dan validasi toolchain awal.
Dengan seluruh evidence dan pengujian yang telah terpenuhi, environment praktikum dinyatakan siap untuk melanjutkan ke M1, yaitu tahap verifikasi freestanding toolchain dan reproducible ELF proof.

## 12. Lampiran

- Output `tools/check_env.sh`
- Isi `build/meta/toolchain-versions.txt`
- Output `readelf -h`
- Output `objdump` ringkas
- Screenshot relevan
- Commit hash

## 13. Referensi

Gunakan format IEEE sesuai panduan praktikum.

[1] [R. H. Arpaci-Dusseau and A. C. Arpaci-Dusseau, Operating Systems: Three Easy Pieces. Arpaci-Dusseau Books. [Online]. Available: https://pages.cs.wisc.edu/~remzi/OSTEP/. Accessed: 6 May 2026.]
[2] [R. Cox, F. Kaashoek, and R. Morris, “xv6: a simple, Unix-like teaching operating system,” MIT PDOS. [Online]. Available: https://pdos.csail.mit.edu/6.828/2020/xv6.html. Accessed: 6 May 2026..]
[3] [LLVM Project, LLVM Documentation. [Online]. Available: https://llvm.org/docs/. Accessed: 6 May 2026.]

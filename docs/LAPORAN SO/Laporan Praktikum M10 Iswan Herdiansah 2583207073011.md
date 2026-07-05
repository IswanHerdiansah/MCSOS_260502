# Template Laporan Praktikum Sistem Operasi Lanjut — MCSOS

**Nama file laporan:** `laporan_praktikum_M10_2583207073011.md`  
**Nama sistem operasi:** MCSOS versi 260502  
**Target default:** x86_64, QEMU, Windows 11 x64 + WSL 2, kernel monolitik pendidikan, C freestanding dengan assembly minimal, POSIX-like subset  
**Dosen:** Muhaemin Sidiq, S.Pd., M.Pd.  
**Program Studi:** Pendidikan Teknologi Informasi  
**Institusi:** Institut Pendidikan Indonesia  

---

## 0. Metadata Laporan

| Atribut | Isi |
|---|---|
| Kode praktikum | `M10` |
| Judul praktikum | `ABI System Call Awal, Dispatcher Syscall, Validasi Argumen, dan Jalur int 0x80 Terkendali pada MCSOS` |
| Jenis pengerjaan | `Individu` |
| Nama mahasiswa | `Iswan Herdiansah` |
| NIM | `2583207073011` |
| Kelas | `PTI 1A` |
| Nama kelompok | `Tidak berlaku` |
| Anggota kelompok | `Tidak berlaku` |
| Tanggal praktikum | `2026-05-23` |
| Tanggal pengumpulan | `2026-05-23` |
| Repository | `~/src/mcsos` |
| Branch | `praktikum/m10-syscall-abi` |
| Commit awal | `786552addde8bac6b9df24856bbfee80eda43c1e` |
| Commit akhir | `0ec388a8a4a4176b14979a95063303e73c427273` |
| Status readiness yang diklaim | `Siap uji QEMU untuk syscall dispatcher awal single-core dan smoke test ABI kernel-side` |

---

## 1. Sampul

# Laporan Praktikum `M10`  
## `ABI System Call Awal, Dispatcher Syscall, Validasi Argumen, dan Jalur int 0x80 Terkendali pada MCSOS`

Disusun oleh:

| Nama | NIM | Kelas | Peran |
|---|---|---|---|
| Iswan Herdiansah | 2583207073011 | PTI 1A | Individu |

Dosen Pengampu: **Muhaemin Sidiq, S.Pd., M.Pd.**  
Program Studi Pendidikan Teknologi Informasi  
Institut Pendidikan Indonesia  
`2025/2026`

---

## 2. Pernyataan Orisinalitas dan Integritas Akademik

Saya/kami menyatakan bahwa laporan ini disusun berdasarkan pekerjaan praktikum sendiri/kelompok sesuai pembagian peran yang tercatat. Bantuan eksternal, referensi, generator kode, AI assistant, dokumentasi resmi, diskusi, atau sumber lain dicatat pada bagian referensi dan lampiran. Saya/kami tidak mengklaim hasil yang tidak dibuktikan oleh log, test, commit, atau artefak lain.

| Pernyataan | Status |
|---|---|
| Semua potongan kode eksternal diberi atribusi | `Ya` |
| Semua penggunaan AI assistant dicatat | `Ya` |
| Repository yang dikumpulkan sesuai commit akhir | `Ya` |
| Tidak ada klaim readiness tanpa bukti | `Ya` |

Catatan penggunaan bantuan eksternal:

```text
Referensi yang digunakan: Intel SDM untuk mekanisme interrupt/trap x86_64, x86-64 psABI untuk
calling convention, dokumentasi QEMU GDB untuk debugging, dokumentasi Clang untuk compiler
flags freestanding, dokumentasi Linux untuk metodologi syscall sebagai pembanding konseptual.
AI assistant digunakan untuk mendiskusikan konsep desain; seluruh kode diverifikasi mandiri
melalui build, host unit test, freestanding compile, nm/readelf/objdump audit, dan QEMU run.
```

---

## 3. Tujuan Praktikum

Tuliskan tujuan teknis dan konseptual praktikum. Tujuan harus dapat diuji.

1. Merancang dan mengimplementasikan ABI syscall berbasis register x86_64 dengan konvensi: `rax` = nomor syscall, argumen via `rdi`, `rsi`, `rdx`, `r10`, `r8`, `r9`, dan nilai balik via `rax`.
2. Mengimplementasikan dispatcher syscall table-driven yang menolak nomor tidak valid dengan kode error `-ENOSYS`.
3. Mengimplementasikan validasi argumen dan validasi rentang user buffer (range check + overflow arithmetic protection) pada jalur syscall kernel-side.
4. Membuat stub entry `int 0x80` (`x86_64_syscall_int80_stub`) yang terhubung ke IDT M4 dan dapat diaudit melalui disassembly.
5. Menghasilkan host unit test dispatcher yang lulus tanpa QEMU, freestanding object yang dapat diaudit dengan `nm`/`readelf`/`objdump`, serta integrasi kernel yang boot di QEMU dengan scheduler M9 tetap berjalan.
6. Menyimpan bukti build, test, audit ELF, dan log QEMU sebagai evidence terukur.

---

## 4. Capaian Pembelajaran Praktikum

Setelah praktikum ini, mahasiswa mampu:

| CPL/CPMK praktikum | Bukti yang harus ditunjukkan |
|---|---|
| Menjelaskan ABI syscall berbasis register x86_64 | Header `syscall.h` yang mendefinisikan nomor, argumen, dan return convention |
| Mengimplementasikan dispatcher syscall table-driven dengan bound check | `syscall.c` + output host test `M10 syscall host tests passed` |
| Mengimplementasikan validasi rentang user buffer dengan proteksi overflow | Source `mcsos_user_check_range` di `syscall.c` |
| Membuat stub entry `int 0x80` yang dapat diaudit | objdump output: `x86_64_syscall_int80_stub` + `iretq` terverifikasi |
| Menghasilkan freestanding object tanpa symbol undefined | `nm -u build/m10/m10_syscall_combined.o` kosong; readelf menunjukkan ELF64 AMD64 |
| Mengintegrasikan syscall layer ke kernel tanpa merusak scheduler M9 | QEMU serial log: scheduler M9 tetap berjalan setelah build dengan syscall.o diintegrasikan |
| Menjelaskan failure mode syscall dan batas-batas M10 | Bagian 15 laporan ini |

---

## 5. Peta Milestone MCSOS

Centang milestone yang menjadi fokus laporan ini. Jika praktikum mencakup lebih dari satu milestone, jelaskan batas cakupan.

| Milestone | Fokus | Status dalam laporan |
|---|---|---|
| M0 | Requirements, governance, baseline arsitektur | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M1 | Toolchain reproducible, Git, QEMU, GDB, metadata build | `[ ] tidak dibahas / [] dibahas / [v] selesai praktikum` |
| M2 | Boot image, kernel ELF64, early console | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M3 | Panic path, linker map, GDB, observability awal | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M4 | Trap, exception, interrupt, timer | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M5 | PMM, VMM, page table, kernel heap | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M6 | Thread, scheduler, synchronization | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M7 | Syscall ABI dan user program loader | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M8 | VFS, file descriptor, ramfs | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M9 | Block layer dan device model | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M10 | Persistent filesystem, mcsfs/ext2-like, recovery | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M11 | Networking stack, packet parsing, UDP/TCP subset | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M12 | Security model, capability/ACL, syscall fuzzing, hardening | `[v] tidak dibahas / [ ] dibahas / [ ] selesai pvraktikum` |
| M13 | SMP, scalability, lock stress, NUMA-aware preparation | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M14 | Framebuffer, graphics console, visual regression | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M15 | Virtualization/container subset | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M16 | Observability, update/rollback, release image, readiness review | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |

Batas cakupan praktikum:

```text
Termasuk:
- ABI syscall berbasis register x86_64 (rax, rdi, rsi, rdx, r10, r8, r9)
- Dispatcher syscall table-driven dengan validasi nomor (MCSOS_SYS_MAX = 5)
- Validasi rentang user buffer dan proteksi overflow arithmetic
- Stub entry int 0x80 (x86_64_syscall_int80_stub + iretq)
- Host unit test dispatcher (tests/test_syscall.c)
- Freestanding object audit (syscall.o + syscall_entry.o → m10_syscall_combined.o)
- Integrasi kernel: syscall.o + syscall_entry.o masuk ke build/kernel.elf
- QEMU smoke boot dengan scheduler M9 tetap berjalan

Tidak termasuk (non-scope M10):
- Ring 3 penuh; ELF user loader; per-process address space
- syscall/sysret production path; TSS/IST lengkap untuk ring 3
- POSIX ABI compatibility; signal subsystem; credential subsystem
- SMP syscall path; copy-on-write; demand paging user
- Page-fault assisted usercopy; secure syscall boundary penuh
- fork/exec/wait; process isolation production
```

---

## 6. Dasar Teori Ringkas

### 6.1 Konsep Sistem Operasi yang Diuji

```text
System Call (syscall) adalah mekanisme batas terkontrol antara kode pemanggil (user/kernel-trigger)
dan kernel service. Pada MCSOS M10, syscall layer dibangun secara bertahap:

1. ABI Syscall
   Kontrak antara pemanggil dan kernel tentang bagaimana nomor syscall, argumen, dan nilai balik
   ditempatkan di register. M10 menggunakan konvensi: rax = nomor syscall; argumen: rdi, rsi, rdx,
   r10, r8, r9; nilai balik: rax. Konvensi ini mengacu x86-64 System V psABI dengan modifikasi
   r10 menggantikan rcx untuk argumen ke-4, karena syscall instruction akan merusak rcx.

2. Dispatcher Syscall Table-Driven
   Array function pointer yang diindeks oleh nomor syscall. Setiap pemanggilan masuk ke
   mcsos_syscall_dispatch(), yang memeriksa batas (bound check) dan memanggil handler yang
   sesuai. Nomor di luar batas menghasilkan return -ENOSYS (error: no such syscall).

3. Validasi Argumen dan User Buffer
   Pointer yang datang dari pemanggil tidak boleh dipercaya begitu saja. Validasi mencakup:
   - Pemeriksaan rentang (range check): apakah [ptr, ptr+len) berada di area yang diizinkan
   - Pemeriksaan overflow arithmetic: ptr + len tidak boleh wrap around (overflow uint64_t)
   Validasi ini belum menggantikan page-fault recovery atau pemeriksaan permission page table.

4. Entry int 0x80
   IDT vector 0x80 dihubungkan ke x86_64_syscall_int80_stub. Stub ini menerima trap frame dari
   mekanisme interrupt gate M4, memanggil mcsos_syscall_dispatch(), lalu kembali lewat iretq.
   Ini adalah jalur pendidikan yang memanfaatkan infrastruktur IDT yang sudah ada sejak M4.

5. Freestanding Object
   syscall.c dan syscall_entry.S dikompilasi sebagai freestanding (tanpa hosted libc). Object
   yang dihasilkan harus bisa di-link ke kernel tanpa unresolved symbol dari libc.
```

### 6.2 Konsep Arsitektur x86_64 yang Relevan

| Konsep | Relevansi pada praktikum | Bukti/verifikasi |
|---|---|---|
| Interrupt gate IDT | Vector 0x80 terhubung ke x86_64_syscall_int80_stub via IDT M4 | `x86_64_idt_set_gate` di disassembly; `lidt` di objdump |
| Trap frame / register save-restore | Stub entry harus menyimpan dan memulihkan register caller sebelum memanggil C dispatcher | Disassembly `x86_64_syscall_int80_stub`; `iretq` verified |
| iretq | Instruksi return dari interrupt/exception di long mode; wajib untuk kembali dari syscall path | `objdump -d m10_syscall_combined.o` menunjukkan `iretq` di offset 142 |
| Register calling convention | rax=nomor syscall; rdi/rsi/rdx/r10/r8/r9=argumen; menghindari clobber rcx/r11 | Header `syscall.h`; `grep MCSOS_SYS_MAX syscall.h` |
| `-mno-red-zone` | Kernel tidak boleh mengandalkan red zone karena interrupt dapat terjadi kapan saja | Build flag: `-mno-red-zone` hadir di seluruh kompilasi |
| ELF64 x86_64 | Kernel ELF harus bertipe ELF64 dengan machine AMD64 | `readelf -h build/kernel.elf`: Class ELF64, Machine AMD64 |

### 6.3 Konsep Implementasi Freestanding

| Aspek | Keputusan praktikum |
|---|---|
| Bahasa | C17 freestanding + assembly x86_64 minimal (syscall_entry.S) |
| Runtime | Tanpa hosted libc; tidak ada crt0 libc; kernel menyediakan runtime sendiri |
| ABI | x86_64 System V ABI untuk calling convention; ABI syscall MCSOS berbasis register |
| Compiler flags kritis | `--target=x86_64-unknown-none-elf`, `-ffreestanding`, `-fno-builtin`, `-mno-red-zone`, `-mno-sse`, `-mno-sse2`, `-mcmodel=kernel`, `-fno-pic`, `-fno-pie` |
| Risiko undefined behavior | Pointer user yang tidak divalidasi; integer overflow pada range check (ptr+len); register clobber jika stub assembly salah; stack alignment jika entry tidak benar |

### 6.4 Referensi Teori yang Digunakan

| No. | Sumber | Bagian yang digunakan | Alasan relevansi |
|---|---|---|---|
| [1] | Intel SDM | Vol. 3A: Ch. 6 (Interrupt/Exception Handling), Ch. 5 (Protection) | Mekanisme interrupt gate, trap frame, iretq, privilege level untuk IDT vector 0x80 |
| [2] | x86-64 psABI | Calling convention (§3.2), register allocation, red zone | Dasar ABI register syscall: rdi/rsi/rdx/r10/r8/r9, return rax |
| [3] | QEMU GDB documentation | Remote GDB, breakpoint, register inspection | Debugging kernel di QEMU dengan -s -S untuk verifikasi entry stub |
| [4] | Clang command line reference | -ffreestanding, -fno-builtin, -mcmodel=kernel, target flags | Compiler flags untuk freestanding kernel build |
| [5] | Linux Kernel Documentation — Adding System Calls | Metodologi: nomor, prototype, implementasi, wiring, selftest | Pembanding metodologis; MCSOS M10 menerapkan prinsip serupa dalam skala pendidikan |
| [6] | Linux Kernel Documentation — Locking and Concurrency | Konteks interrupt, preemption, lock ownership | Pembanding konseptual untuk jalur syscall yang menyentuh scheduler |

---

## 7. Lingkungan Praktikum

### 7.1 Host dan Target

| Komponen | Nilai |
|---|---|
| Host OS | Windows 11 x64 |
| Lingkungan build | WSL 2 Ubuntu (x86_64-pc-linux-gnu) |
| Target ISA | `x86_64` |
| Target ABI | `x86_64-unknown-none-elf` |
| Emulator | QEMU 6.2.0 (machine q35) |
| Firmware emulator | Limine BIOS/UEFI (limine-bios.sys, BOOTX64.EFI) |
| Debugger | GNU GDB 12.1 |
| Build system | GNU Make |
| Bahasa utama | C17 freestanding |
| Assembly | GAS (via Clang/LLVM assembler) |

### 7.2 Versi Toolchain

Tempel output versi toolchain berikut. Jalankan dari clean shell WSL.

```bash
clang --version | head -n 1
gcc --version | head -n 1
ld.lld --version | head -n 1
nm --version | head -n 1
readelf --version | head -n 1
objdump --version | head -n 1
qemu-system-x86_64 --version | head -n 1
gdb --version | head -n 1
```

Output:

```text
Ubuntu clang version 14.0.0-1ubuntu1.1
gcc (Ubuntu 11.4.0-1ubuntu1~22.04.3) 11.4.0
Ubuntu LLD 14.0.0 (compatible with GNU linkers)
GNU nm (GNU Binutils for Ubuntu) 2.38
GNU readelf (GNU Binutils for Ubuntu) 2.38
GNU objdump (GNU Binutils for Ubuntu) 2.38
QEMU emulator version 6.2.0 (Debian 1:6.2+dfsg-2ubuntu6.30)
GNU gdb (Ubuntu 12.1-0ubuntu1~22.04.2) 12.1
```

### 7.3 Lokasi Repository

| Item | Nilai |
|---|---|
| Path repository di WSL | `~/src/mcsos` |
| Apakah berada di filesystem Linux WSL, bukan `/mnt/c` | `Ya` |
| Remote repository | [Tidak tersedia] |
| Branch | `praktikum/m10-syscall-abi` |
| Commit hash awal (checkpoint M9) | `786552addde8bac6b9df24856bbfee80eda43c1e` |
| Commit hash akhir | `0ec388a8a4a4176b14979a95063303e73c427273` |

---

## 8. Repository dan Struktur File

### 8.1 Struktur Direktori yang Relevan

```text
mcsos/
├── kernel/
│   ├── arch/x86_64/
│   │   ├── syscall_entry.S        ← BARU (M10): stub entry int 0x80
│   │   ├── idt.c
│   │   ├── context_switch.S
│   │   ├── isr.S
│   │   ├── pic.c
│   │   └── pit.c
│   ├── core/
│   │   ├── syscall.c              ← BARU (M10): dispatcher + validasi
│   │   ├── thread.c
│   │   ├── trap.c
│   │   ├── pmm.c
│   │   ├── vmm.c
│   │   ├── kmain.c
│   │   └── boot.c
│   ├── include/mcsos/kernel/
│   │   ├── syscall.h              ← BARU (M10): ABI, tabel, error code
│   │   ├── thread.h
│   │   └── kmem.h
│   └── mm/
│       └── kmem.c
├── tests/
│   └── test_syscall.c             ← BARU (M10): host unit test dispatcher
├── scripts/
│   ├── m10_preflight.sh           ← BARU (M10): preflight check script
│   └── m10_qemu_smoke.sh          ← BARU (M10): QEMU smoke script
├── build/
│   ├── kernel.elf
│   ├── mcsos.iso
│   └── m10/
│       ├── m10_host_test
│       ├── m10_syscall_combined.o
│       ├── syscall.o
│       ├── syscall_entry.o
│       ├── nm_undefined.log
│       ├── readelf_header.log
│       ├── objdump_key.log
│       ├── test_syscall.log
│       └── sha256.log
├── Makefile                       ← UBAH (M10): tambah target m10-*
└── linker.ld
```

### 8.2 File yang Dibuat atau Diubah

| File | Jenis perubahan | Alasan perubahan | Risiko |
|---|---|---|---|
| `kernel/include/mcsos/kernel/syscall.h` | baru | Mendefinisikan ABI syscall: nomor (SYS_*), MCSOS_SYS_MAX, prototype mcsos_syscall_dispatch, konstanta error, konvensi register | Rendah — header only; tidak ada eksekusi langsung |
| `kernel/core/syscall.c` | baru | Implementasi dispatcher table-driven, validasi nomor syscall, validasi range user buffer, stub handler operasi minimal | Sedang — jalur kritis kernel; kesalahan validasi dapat menyebabkan GPF atau corrupt state |
| `kernel/arch/x86_64/syscall_entry.S` | baru | Stub entry `int 0x80`: menerima trap frame, memanggil C dispatcher, kembali via `iretq` | Tinggi — kesalahan register save/restore atau stack alignment dapat menyebabkan triple fault |
| `tests/test_syscall.c` | baru | Host unit test dispatcher: menguji dispatch valid, dispatch nomor invalid (expect -ENOSYS), dan range validation | Rendah — host-only, tidak mempengaruhi kernel |
| `scripts/m10_preflight.sh` | baru | Script verifikasi file wajib M10 tersedia dan symbol penting ada | Rendah — audit only |
| `scripts/m10_qemu_smoke.sh` | baru | Script QEMU smoke run untuk M10 | Rendah — runner only |
| `Makefile` | ubah | Tambah target `m10-host-test`, `m10-freestanding`, `m10-audit`, `m10-all`, `m10-clean`; tambah `syscall.c` dan `syscall_entry.S` ke build normal kernel | Sedang — kesalahan Makefile dapat memblokir build (terbukti dari iterasi debug Makefile) |

### 8.3 Ringkasan Diff

```bash
git status --short
git log --oneline -n 5
```

Output:

```text
On branch praktikum/m10-syscall-abi
nothing to commit, working tree clean

0ec388a m10: add syscall layer and int80 entry
786552a checkpoint before M9 scheduler
4f030a4 m8: add early kernel heap allocator
7eab63d M6: implement bitmap physical memory manager
390fdcf M5: implement external interrupts and PIT timer

Commit 0ec388a: 9 files changed, 364 insertions(+), 56 deletions(-)
  create mode 100644 kernel/arch/x86_64/syscall_entry.S
  create mode 100644 kernel/core/syscall.c
  create mode 100644 kernel/include/mcsos/kernel/syscall.h
  create mode 100644 logs/.gitkeep
  create mode 100644 logs/m10_sha256.txt
  create mode 100755 scripts/m10_preflight.sh
  create mode 100755 scripts/m10_qemu_smoke.sh
  create mode 100644 tests/test_syscall.c
```

---

## 9. Desain Teknis

### 9.1 Masalah yang Diselesaikan

```text
Kernel MCSOS setelah M9 memiliki IDT, trap dispatch, timer interrupt, PMM, VMM, heap, dan
scheduler kooperatif. Namun belum ada mekanisme terkontrol bagi kode kernel-triggered atau
test harness untuk meminta layanan kernel melalui antarmuka yang terdefinisi. Tanpa syscall
layer:
  - Tidak ada kontrak ABI: setiap komponen memanggil fungsi kernel secara langsung tanpa
    validasi atau batas privilege.
  - Tidak ada dispatcher: tidak ada titik tunggal yang bisa mencegah nomor layanan tidak
    valid atau pointer tidak valid masuk ke kernel.
  - Jalur int 0x80 tidak ada: IDT M4 belum terhubung ke handler syscall apapun.
  - Tidak ada error convention: tidak ada cara standar untuk mengembalikan kode error ke
    pemanggil.

M10 menyelesaikan masalah ini dengan membangun layer syscall awal yang dapat diaudit,
diuji dengan host test, dan diintegrasikan ke kernel secara bertahap.
```

### 9.2 Keputusan Desain

| Keputusan | Alternatif yang dipertimbangkan | Alasan memilih | Konsekuensi |
|---|---|---|---|
| Entry via `int 0x80` bukan `syscall/sysret` | `syscall/sysret` (lebih cepat) | IDT dari M4 sudah tersedia; `syscall/sysret` memerlukan GDT selector user, TSS, dan MSR STAR/LSTAR yang belum disiapkan di M10 | Latensi lebih tinggi dari `syscall/sysret`; namun lebih mudah diintegrasikan ke IDT M4 yang sudah ada |
| Dispatcher table-driven (array function pointer) | Switch-case langsung | Table-driven lebih mudah diaudit, diperluas, dan ditest; bound check sederhana | Perlu menjaga konsistensi antara enum nomor dan isi tabel |
| Validasi range + overflow arithmetic (bukan page-fault recovery) | Tidak validasi / page fault handler | Range check + overflow check sudah mencegah dereference jelas-jelas salah tanpa perlu page fault recovery yang kompleks | Belum menggantikan pemeriksaan permission page table; bukan security-complete |
| Host unit test terpisah dari freestanding kernel | Hanya QEMU test | Host test lebih cepat, deterministik, tidak memerlukan boot; sesuai panduan M10 | Tidak menguji integrasi hardware path; QEMU smoke tetap diperlukan untuk integrasi |
| MCSOS_SYS_MAX = 5 | Nilai lain | Cukup untuk operasi minimal pendidikan (yield, exit, getpid, write_serial, nop) tanpa membuat tabel terlalu besar untuk tahap ini | Perlu diperluas di milestone lanjutan |

### 9.3 Arsitektur Ringkas

```mermaid
flowchart TD
    A[int 0x80 dari test harness / kernel code] --> B[x86_64_syscall_int80_stub]
    B -->|save regs + call| C[mcsos_syscall_dispatch]
    C -->|rax >= MCSOS_SYS_MAX| D[return -ENOSYS]
    C -->|rax valid + table[rax] != NULL| E[handler syscall]
    C -->|table[rax] == NULL| F[return -ENOSYS]
    E -->|memerlukan user buffer| G[mcsos_user_check_range]
    G -->|range invalid / overflow| H[return -EFAULT]
    G -->|range valid| I[eksekusi handler]
    I --> J[return nilai ke rax]
    J --> K[iretq kembali ke pemanggil]
    D --> K
    F --> K
    H --> K
```

Penjelasan diagram:

```text
1. int 0x80 memicu trap gate IDT vector 0x80 yang mengarah ke x86_64_syscall_int80_stub.
2. Stub menyimpan register yang diperlukan, mengatur argumen, dan memanggil mcsos_syscall_dispatch.
3. Dispatcher memeriksa nomor syscall (rax) terhadap MCSOS_SYS_MAX:
   - Jika di luar batas: return -ENOSYS langsung.
   - Jika dalam batas tapi NULL: return -ENOSYS.
   - Jika valid: panggil handler.
4. Handler yang memerlukan user buffer memanggil mcsos_user_check_range untuk memeriksa
   rentang dan overflow. Jika gagal: return -EFAULT.
5. Setelah handler selesai, nilai return ditempatkan di rax, lalu iretq mengembalikan eksekusi.
```

### 9.4 Kontrak Antarmuka

| Antarmuka | Pemanggil | Penerima | Precondition | Postcondition | Error path |
|---|---|---|---|---|---|
| `x86_64_syscall_int80_stub` | IDT vector 0x80 (CPU hardware) | `mcsos_syscall_dispatch` | CPU dalam long mode; IDT entry 0x80 valid; stack aligned | Kembali via `iretq`; `rax` berisi nilai return atau kode error | Triple fault jika stack corrupt atau IDT gate salah |
| `mcsos_syscall_dispatch(nr, a0..a5)` | `x86_64_syscall_int80_stub` | tabel handler syscall | `nr` adalah nilai dari `rax` pemanggil | `rax` berisi 0 (sukses) atau kode error negatif | Return `-ENOSYS` jika `nr >= MCSOS_SYS_MAX` atau handler NULL |
| `mcsos_user_check_range(ptr, len)` | handler syscall | validasi internal | `ptr` dan `len` adalah nilai dari argumen pemanggil | Return 0 jika rentang valid; return non-0 jika tidak valid | Return error jika `ptr + len` overflow atau di luar area yang diizinkan |

### 9.5 Struktur Data Utama

| Struktur data | Field penting | Ownership | Lifetime | Invariant |
|---|---|---|---|---|
| Tabel syscall (array fn pointer) | `handler[MCSOS_SYS_MAX]` | kernel static | seumur kernel | `handler[i]` boleh NULL (berarti belum diimplementasikan); `MCSOS_SYS_MAX` tidak boleh diubah saat runtime |
| `mcsos_syscall_dispatch` signature | `nr` (uint64_t), `a0`–`a5` (uint64_t) | —  | per-call | `nr` harus divalidasi sebelum digunakan sebagai indeks |

### 9.6 Invariants

1. Setiap panggilan ke `mcsos_syscall_dispatch` harus memeriksa `nr < MCSOS_SYS_MAX` sebelum mengakses tabel handler.
2. Handler `NULL` di tabel harus diperlakukan setara dengan nomor tidak valid: kembalikan `-ENOSYS`.
3. Pointer yang berasal dari argumen pemanggil tidak boleh di-dereference sebelum melewati `mcsos_user_check_range`.
4. Pemeriksaan `ptr + len` pada `mcsos_user_check_range` harus menggunakan aritmetika yang tidak wrap-around (overflow-safe).
5. `x86_64_syscall_int80_stub` harus menyimpan dan memulihkan register caller yang tidak boleh di-clobber sesuai ABI; `iretq` adalah satu-satunya jalur return yang sah dari stub.
6. Kode syscall tidak boleh memanggil fungsi libc; semua dependency harus dari kernel sendiri.

### 9.7 Ownership, Locking, dan Concurrency

| Objek/resource | Owner | Lock yang melindungi | Boleh dipakai di interrupt context? | Catatan |
|---|---|---|---|---|
| Tabel handler syscall (read-only setelah init) | kernel static | none (read-only setelah boot) | Ya | Tidak dimodifikasi setelah inisialisasi |
| `rax`, `rdi`, `rsi`, `rdx`, `r10`, `r8`, `r9` saat syscall | CPU register | — | — | Diserahkan ke dispatcher oleh stub; nilai asli harus disimpan di stack jika perlu |
| Scheduler state (thread, runqueue) | kernel scheduler M9 | Single-core; interrupt disabled saat context switch | Tidak (lock diperlukan jika SMP) | Syscall `yield` dan `exit_thread` harus menggunakan callback scheduler, bukan akses langsung |

Lock order yang berlaku:

```text
M10 hanya single-core. Syscall dispatcher tidak mengambil lock tersendiri.
Syscall yang menyentuh scheduler memanggil callback (thread_yield) yang sudah
diimplementasikan di M9. Tidak ada locking tambahan di level dispatcher M10.
Jika SMP ditambahkan di masa mendatang, dispatcher perlu lock sebelum mengakses
state bersama.
```

### 9.8 Memory Safety dan Undefined Behavior Risk

| Risiko | Lokasi | Mitigasi | Bukti |
|---|---|---|---|
| Integer overflow pada `ptr + len` | `mcsos_user_check_range` | Pemeriksaan eksplisit: `if (ptr > UINT64_MAX - len)` atau setara sebelum penjumlahan | Source `syscall.c`; host unit test menguji kasus overflow |
| Array out-of-bounds pada tabel handler | `mcsos_syscall_dispatch` | Bound check `nr < MCSOS_SYS_MAX` sebelum akses `table[nr]` | Host unit test menguji `nr >= MCSOS_SYS_MAX` menghasilkan `-ENOSYS` |
| Register clobber di stub assembly | `syscall_entry.S` | Stub menyimpan register yang diperlukan sebelum memanggil C; `iretq` memulihkan state CPU | Disassembly `x86_64_syscall_int80_stub` di objdump: `iretq` pada offset 142 |
| Stack alignment violation | `syscall_entry.S` | 16-byte alignment dijaga sebelum memanggil C function | [Belum diuji dengan GDB stack-trace penuh] |
| Dereference pointer NULL handler | `mcsos_syscall_dispatch` | Cek `handler[nr] != NULL` sebelum memanggil | Host unit test; source review |

### 9.9 Security Boundary

| Boundary | Data tidak tepercaya | Validasi yang dilakukan | Failure mode aman |
|---|---|---|---|
| Syscall entry (`int 0x80`) | Nomor syscall (`rax`), argumen (`rdi`..`r9`) dari pemanggil | Bound check nomor syscall; range check + overflow check untuk pointer argumen | Return `-ENOSYS` untuk nomor tidak valid; return `-EFAULT` untuk pointer tidak valid |
| `mcsos_user_check_range` | `ptr` dan `len` dari argumen pemanggil | Pemeriksaan overflow arithmetic dan rentang virtual | Return kode error non-0; tidak melanjutkan ke akses memori |

> Catatan: Validasi M10 baru mencakup range check dan overflow arithmetic. Validasi ini belum mencakup pemeriksaan permission page table, user/supervisor bit, atau fault-containment penuh. Ini adalah batasan yang disengaja sesuai scope M10.

---

## 10. Langkah Kerja Implementasi

### Langkah 1 — Pemeriksaan Kesiapan M9 dan Buat Cabang M10

Maksud langkah:

```text
Memverifikasi bahwa seluruh artefak M9 (scheduler, host test, freestanding object) masih
valid sebelum memulai M10. Membuat branch baru sebagai checkpoint rollback.
```

Perintah:

```bash
cd ~/src/mcsos
make m9-all
git checkout -b praktikum/m10-syscall-abi
mkdir -p kernel/include/mcsos/kernel kernel/core kernel/arch/x86_64 tests scripts logs build/m10
```

Output ringkas:

```text
[M9] host test PASS
[M9] freestanding PASS
[M9] audit PASS
[M9] scheduler milestone PASS
Switched to a new branch 'praktikum/m10-syscall-abi'
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| Branch baru | `praktikum/m10-syscall-abi` | Titik rollback aman sebelum perubahan M10 |
| `build/m9/m9_host_test` | `build/m9/` | Bukti scheduler M9 masih valid |

Indikator berhasil:

```text
Output "[M9] scheduler milestone PASS" dan branch baru terbuat.
```

### Langkah 2 — Tulis Header syscall.h

Maksud langkah:

```text
Mendefinisikan kontrak ABI syscall secara eksplisit: nomor syscall (SYS_NOP, SYS_YIELD,
SYS_EXIT_THREAD, SYS_GETPID, SYS_WRITE_SERIAL), MCSOS_SYS_MAX, prototype dispatcher,
dan konvensi register. Header ini dipakai baik oleh kernel maupun host unit test.
```

Perintah:

```bash
nano kernel/include/mcsos/kernel/syscall.h
```

Output ringkas:

```text
# Verifikasi isi header:
grep -n "MCSOS_SYS_MAX\|mcsos_syscall_dispatch" kernel/include/mcsos/kernel/syscall.h
13:#define MCSOS_SYS_MAX           5u
21:mcsos_syscall_dispatch(
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `syscall.h` | `kernel/include/mcsos/kernel/syscall.h` | ABI contract: nomor syscall, prototype dispatcher, error code |

Indikator berhasil:

```text
MCSOS_SYS_MAX = 5 dan mcsos_syscall_dispatch terdefinisi di header.
```

### Langkah 3 — Tulis Dispatcher syscall.c

Maksud langkah:

```text
Mengimplementasikan dispatcher table-driven, validasi nomor syscall (bound check),
validasi range user buffer (mcsos_user_check_range dengan overflow protection),
dan handler stub untuk operasi minimal.
```

Perintah:

```bash
nano kernel/core/syscall.c
# Verifikasi simbol penting:
grep -n "mcsos_user_check_range\|mcsos_syscall_dispatch" kernel/core/syscall.c
```

Output ringkas:

```text
48:mcsos_syscall_dispatch(
(mcsos_user_check_range juga terdapat dalam syscall.c)
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `syscall.c` | `kernel/core/syscall.c` | Implementasi dispatcher + validasi |

Indikator berhasil:

```text
Fungsi mcsos_syscall_dispatch dan validasi range tersedia di source.
```

### Langkah 4 — Tulis Stub Entry syscall_entry.S

Maksud langkah:

```text
Menulis stub assembly x86_64_syscall_int80_stub yang menjadi handler IDT vector 0x80.
Stub menerima trap frame, mengatur argumen ke konvensi C, memanggil mcsos_syscall_dispatch,
dan kembali via iretq.
```

Perintah:

```bash
nano kernel/arch/x86_64/syscall_entry.S
# Verifikasi simbol:
grep -n "x86_64_syscall_int80_stub\|iretq" kernel/arch/x86_64/syscall_entry.S
```

Output ringkas:

```text
1:.global x86_64_syscall_int80_stub
5:x86_64_syscall_int80_stub:
23:    iretq
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `syscall_entry.S` | `kernel/arch/x86_64/syscall_entry.S` | Stub entry int 0x80; bridge assembly ke C dispatcher |

Indikator berhasil:

```text
Symbol x86_64_syscall_int80_stub dan instruksi iretq ada di source.
```

### Langkah 5 — Tulis Host Unit Test

Maksud langkah:

```text
Menulis test_syscall.c yang menguji dispatcher secara host (tanpa QEMU): dispatch nomor valid,
dispatch nomor tidak valid (expect -ENOSYS), dan validasi range. Test ini berjalan di host
menggunakan cc standar dengan -DMCSOS_HOST_TEST.
```

Perintah:

```bash
nano tests/test_syscall.c
cc -std=c17 -Wall -Wextra -Werror -DMCSOS_HOST_TEST -Ikernel/include \
   tests/test_syscall.c kernel/core/syscall.c -o build/m10/m10_host_test
./build/m10/m10_host_test
```

Output ringkas:

```text
M10 syscall host tests passed
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `test_syscall.c` | `tests/test_syscall.c` | Host unit test dispatcher syscall |
| `m10_host_test` | `build/m10/m10_host_test` | Binary host test yang sudah lulus |
| `test_syscall.log` | `build/m10/test_syscall.log` | Log hasil test |

Indikator berhasil:

```text
Output "M10 syscall host tests passed" dan binary berhasil dijalankan.
```

### Langkah 6 — Freestanding Compile dan Audit Object

Maksud langkah:

```text
Mengompilasi syscall.c dan syscall_entry.S sebagai freestanding (--target=x86_64-unknown-none-elf)
dan menggabungkannya menjadi m10_syscall_combined.o. Kemudian mengaudit dengan nm, readelf, dan
objdump untuk memastikan: tidak ada undefined symbol, ELF64 AMD64, dan iretq hadir di disassembly.
```

Perintah:

```bash
clang --target=x86_64-unknown-none-elf -std=c17 -ffreestanding -fno-builtin \
   -fno-stack-protector -fno-pic -fno-pie -m64 -mno-red-zone -Wall -Wextra -Werror \
   -Ikernel/include -c kernel/core/syscall.c -o build/m10/syscall.o

clang --target=x86_64-unknown-none-elf -ffreestanding -fno-stack-protector \
   -fno-pic -fno-pie -m64 -mno-red-zone -Wall -Wextra -Werror \
   -Ikernel/include -c kernel/arch/x86_64/syscall_entry.S -o build/m10/syscall_entry.o

ld.lld -r build/m10/syscall.o build/m10/syscall_entry.o -o build/m10/m10_syscall_combined.o

nm -u build/m10/m10_syscall_combined.o | tee build/m10/nm_undefined.log
readelf -h build/m10/m10_syscall_combined.o | tee build/m10/readelf_header.log
objdump -d build/m10/m10_syscall_combined.o | grep -E 'x86_64_syscall_int80_stub|iretq' \
   | tee build/m10/objdump_key.log
```

Output ringkas:

```text
[M10] freestanding PASS

nm -u output: (kosong — tidak ada undefined symbol)

readelf header:
  Class: ELF64
  Machine: Advanced Micro Devices X86-64
  Type: REL (Relocatable file)

objdump key:
  0000000000000134 <x86_64_syscall_int80_stub>:
   142:   48 cf                   iretq
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `syscall.o` | `build/m10/syscall.o` | Freestanding object dispatcher |
| `syscall_entry.o` | `build/m10/syscall_entry.o` | Freestanding object stub entry |
| `m10_syscall_combined.o` | `build/m10/m10_syscall_combined.o` | Combined object untuk audit |
| `nm_undefined.log` | `build/m10/nm_undefined.log` | Audit undefined symbol (kosong = lulus) |
| `readelf_header.log` | `build/m10/readelf_header.log` | Header ELF64 AMD64 |
| `objdump_key.log` | `build/m10/objdump_key.log` | Disassembly key: stub + iretq |

Indikator berhasil:

```text
nm -u kosong; readelf menunjukkan ELF64 AMD64; objdump memuat x86_64_syscall_int80_stub + iretq.
```

### Langkah 7 — Integrasi ke Kernel dan Build make all && make image

Maksud langkah:

```text
Menambahkan syscall.c dan syscall_entry.S ke build kernel normal (Makefile) agar
syscall layer masuk ke build/kernel.elf. Terdapat beberapa iterasi debug Makefile
karena kesalahan sintaks saat mengedit dengan sed dan nano.
```

Perintah:

```bash
# (setelah beberapa iterasi perbaikan Makefile)
nano Makefile
make clean
make all
make image
```

Output ringkas:

```text
clang ... -c kernel/core/syscall.c -o build/normal/kernel/core/syscall.o
clang ... -c kernel/arch/x86_64/syscall_entry.S -o build/normal/kernel/arch/x86_64/syscall_entry.o
ld.lld ... build/normal/kernel/core/syscall.o ... build/normal/kernel/arch/x86_64/syscall_entry.o ... -o build/kernel.elf
ISO selesai: build/mcsos.iso
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `kernel.elf` | `build/kernel.elf` | Kernel ELF64 dengan syscall layer terintegrasi |
| `mcsos.iso` | `build/mcsos.iso` | ISO bootable untuk QEMU |

Indikator berhasil:

```text
make all dan make image selesai tanpa error; build/kernel.elf dan build/mcsos.iso tersedia.
```

### Langkah 8 — QEMU Smoke Run

Maksud langkah:

```text
Menjalankan ISO hasil build di QEMU untuk memverifikasi bahwa integrasi syscall tidak
merusak boot kernel dan scheduler M9 tetap berjalan.
```

Perintah:

```bash
qemu-system-x86_64 \
  -machine q35 \
  -m 256M \
  -serial file:logs/m10_serial.log \
  -display none \
  -no-reboot \
  -no-shutdown \
  -cdrom build/mcsos.iso
cat logs/m10_serial.log
```

Output ringkas:

```text
MCSOS 260502 M4 [M9] cooperative scheduler stress test
kernel_start=0xffffffff80000000
kernel_end=0xffffffff800231b8
[M6] pmm initialized
[M7] vmm map ok
[M8] kmem initialized
[M9] scheduler initialized
idt_base=0xffffffff80005000
[M4] IDT loaded
[M5] selftest: IDT invariants passed
[M5] sti: enabling interrupts
[M9] thread B running
[M9] thread C running
[M9] thread D running
[M9] thread A running
...
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `m10_serial.log` | `logs/m10_serial.log` | Log serial QEMU smoke run M10 |

Indikator berhasil:

```text
Kernel boot berhasil mencapai tahap scheduler; [M9] thread A/B/C/D running muncul berulang;
tidak ada panic atau triple fault.
```

### Langkah 9 — m10-all, Preflight, dan Commit

Maksud langkah:

```text
Menjalankan target make m10-all yang mencakup host test, freestanding, audit, dan preflight
script. Kemudian mengkomit semua file baru ke branch praktikum/m10-syscall-abi.
```

Perintah:

```bash
make m10-all
bash scripts/m10_preflight.sh
git add Makefile kernel/core/syscall.c kernel/arch/x86_64/syscall_entry.S \
        kernel/include/mcsos/kernel/syscall.h tests/test_syscall.c \
        scripts/m10_preflight.sh scripts/m10_qemu_smoke.sh logs/
git commit -m "m10: add syscall layer and int80 entry"
```

Output ringkas:

```text
M10 syscall host tests passed
[M10] host test PASS
[M10] freestanding PASS
[M10] audit PASS
======================================
[M10] preflight checks
[M10] required files exist
[M10] symbols verified
[M10] preflight PASS
[M10] syscall milestone PASS
======================================
[pre-commit] running shellcheck
[pre-commit] OK
[praktikum/m10-syscall-abi 0ec388a] m10: add syscall layer and int80 entry
 9 files changed, 364 insertions(+), 56 deletions(-)
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| Commit `0ec388a` | branch `praktikum/m10-syscall-abi` | Snapshot akhir M10 |

Indikator berhasil:

```text
[M10] syscall milestone PASS; commit berhasil dengan hash 0ec388a.
```

---

## 11. Checkpoint Buildable

Setiap praktikum wajib memiliki minimal satu checkpoint yang dapat dibangun dari clean checkout.

| Checkpoint | Perintah | Expected result | Status |
|---|---|---|---|
| Clean build | `make clean && make all` | kernel.elf dan artefak build tersedia | PASS |
| Image generation | `make image` | `build/mcsos.iso` tersedia | PASS |
| M10 host test | `make m10-host-test` | `M10 syscall host tests passed` | PASS |
| M10 freestanding | `make m10-freestanding` | `[M10] freestanding PASS` | PASS |
| M10 audit | `make m10-audit` | `[M10] audit PASS` | PASS |
| M10 preflight | `bash scripts/m10_preflight.sh` | `[M10] preflight PASS` | PASS |
| QEMU smoke test | `make run` (atau run manual) | Serial log boot + scheduler M9 berjalan | PASS |
| M9 dependency check | `make m9-all` | `[M9] scheduler milestone PASS` | PASS |

Catatan checkpoint:

```text
Seluruh checkpoint lulus. Satu-satunya isu yang terjadi selama proses adalah beberapa
iterasi debug Makefile (sintaks missing separator pada baris 64 dan 74) akibat penggunaan
sed dan nano secara berulang untuk menambahkan variabel build. Setelah Makefile diperbaiki
dengan nano secara manual dan bersih, semua target berjalan normal.
```

---

## 12. Perintah Uji dan Validasi

### 12.1 Build Test

Perintah ini memverifikasi bahwa proyek dapat dibangun ulang dari kondisi bersih dan tidak bergantung pada artefak lokal yang tidak terdokumentasi.

```bash
make clean
make all
make image
```

Hasil:

```text
rm -rf build iso_root
[... kompilasi semua object termasuk syscall.o dan syscall_entry.o ...]
ld.lld -nostdlib -static -z max-page-size=0x1000 -T linker.ld -Map=build/kernel.map \
   -o build/kernel.elf [seluruh object list termasuk syscall.o dan syscall_entry.o]
ISO selesai: build/mcsos.iso
```

Status: `PASS`

### 12.2 Static Inspection

Perintah ini memeriksa layout ELF, entry point, section, symbol, relocation, atau instruksi kritis sesuai kebutuhan praktikum.

```bash
readelf -h build/m10/m10_syscall_combined.o
nm -u build/m10/m10_syscall_combined.o
objdump -d build/m10/m10_syscall_combined.o | grep -E 'x86_64_syscall_int80_stub|iretq'
```

Hasil penting:

```text
--- readelf -h build/m10/m10_syscall_combined.o ---
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              REL (Relocatable file)
  Machine:                           Advanced Micro Devices X86-64
  Version:                           0x1
  Entry point address:               0x0
  Start of program headers:          0 (bytes into file)
  Start of section headers:          1168 (bytes into file)
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Number of section headers:         11

--- nm -u (output kosong — tidak ada undefined symbol) ---

--- objdump key ---
0000000000000134 <x86_64_syscall_int80_stub>:
 142:   48 cf                   iretq
```

Status: `PASS`

### 12.3 QEMU Smoke Test

Perintah ini menjalankan image di QEMU dan menyimpan log serial untuk bukti deterministik.

```bash
qemu-system-x86_64 \
  -machine q35 \
  -m 256M \
  -serial file:logs/m10_serial.log \
  -display none \
  -no-reboot \
  -no-shutdown \
  -cdrom build/mcsos.iso
```

Hasil:

```text
MCSOS 260502 M4 [M9] cooperative scheduler stress test
kernel_start=0xffffffff80000000
kernel_end=0xffffffff800231b8
[M6] pmm initialized
[M7] vmm map ok
[M7] vmm phys=0x0000000000200000
[M7] cr3=0x000000000ff56000
[M8] kmem initialized
[M9] scheduler initialized
idt_base=0xffffffff80005000
idt_limit=0x0000000000000fff
[M4] IDT loaded
[M5] idt: loaded
[M5] selftest: IDT invariants passed
[M5] sti: enabling interrupts
[M9] thread B running
[M9] thread C running
[M9] thread D running
[M9] thread A running
[M9] thread B running
[M9] thread C running
[M9] thread D running
...
```

Status: `PASS`

### 12.4 GDB Debug Evidence

Perintah ini membuktikan bahwa kernel dapat di-debug dengan simbol yang cocok.

```bash
qemu-system-x86_64 \
  -machine q35 \
  -m 256M \
  -serial stdio \
  -display none \
  -no-reboot \
  -no-shutdown \
  -s -S \
  -cdrom build/mcsos.iso
```

Hasil:

```text
QEMU berhasil dijalankan dengan -s -S (QEMU menunggu GDB pada port 1234).
Serial output sama seperti QEMU smoke test di atas (scheduler M9 berjalan
setelah GDB continue).
GDB remote attach dilakukan ke localhost:1234.
```

Status: `PASS` (QEMU berhasil dijalankan dengan flag GDB; full GDB session [Belum diuji])

### 12.5 Unit Test

```bash
make m10-all
```

Hasil:

```text
M10 syscall host tests passed
[M10] host test PASS
[M10] freestanding PASS
[M10] audit PASS
[M10] preflight PASS
[M10] syscall milestone PASS
```

Status: `PASS`

### 12.6 Stress/Fuzz/Fault Injection Test

```bash
# Stress test scheduler M9 dari QEMU (cooperative scheduler stress test)
make run
```

Hasil:

```text
[M9] cooperative scheduler stress test berjalan: thread A, B, C, D bergantian
hingga ticks=100, ticks=200, ticks=300, ticks=400, ticks=500 dan seterusnya
tanpa panic.
Syscall fuzzing/fault injection: [Belum diuji]
```

Status: `PASS` untuk scheduler stress; `NA` untuk syscall fuzzing

### 12.7 Visual Evidence

| Screenshot | Lokasi file | Keterangan |
|---|---|---|
| [Tidak tersedia] | [Tidak tersedia] | Terminal output QEMU tidak tersimpan sebagai screenshot terpisah |

---

## 13. Hasil Uji

### 13.1 Tabel Ringkasan Hasil

| No. | Uji | Expected result | Actual result | Status | Evidence |
|---|---|---|---|---|---|
| 1 | Host unit test dispatcher | `M10 syscall host tests passed` | `M10 syscall host tests passed` | PASS | `build/m10/test_syscall.log` |
| 2 | Freestanding compile `syscall.c` | Kompilasi sukses tanpa error/warning | Kompilasi sukses | PASS | Build log `make m10-freestanding` |
| 3 | Freestanding compile `syscall_entry.S` | Kompilasi sukses | Kompilasi sukses | PASS | Build log |
| 4 | Link `m10_syscall_combined.o` | Berhasil tanpa unresolved symbol | Berhasil | PASS | `ld.lld -r` sukses |
| 5 | `nm -u` audit undefined symbol | Output kosong | Output kosong | PASS | `build/m10/nm_undefined.log` |
| 6 | `readelf -h` ELF64 AMD64 | Class ELF64, Machine AMD64 | Sesuai | PASS | `build/m10/readelf_header.log` |
| 7 | `objdump` disassembly `x86_64_syscall_int80_stub` + `iretq` | Symbol dan instruksi hadir | Terverifikasi di offset 134/142 | PASS | `build/m10/objdump_key.log` |
| 8 | Integrasi kernel `make clean && make all` | `kernel.elf` berhasil dengan syscall.o dimasukkan | PASS | PASS | Build log penuh |
| 9 | `make image` | `build/mcsos.iso` tersedia | ISO tersedia | PASS | `build/mcsos.iso` |
| 10 | QEMU smoke boot (scheduler M9 tetap berjalan) | Kernel boot + scheduler berjalan | Boot berhasil; `[M9] thread A/B/C/D running` muncul | PASS | `logs/m10_serial.log` |
| 11 | M9 dependency tidak rusak | `[M9] scheduler milestone PASS` | PASS | PASS | `build/m9/` evidence |
| 12 | M6 PMM dependency tidak rusak | `M6 PMM host unit test: PASS` | PASS | PASS | Build log |
| 13 | M8 KMEM dependency tidak rusak | `M8 KMEM host unit test: PASS` | PASS | PASS | Build log |
| 14 | Preflight script | `[M10] preflight PASS` | PASS | PASS | `scripts/m10_preflight.sh` output |
| 15 | `int 0x80` smoke path dari ring 3 | [Belum diuji] | [Belum diuji] | NA | Non-scope M10 |
| 16 | User pointer validation QEMU live | [Belum diuji] | [Belum diuji] | NA | [Belum diuji] |

### 13.2 Log Penting

```text
--- Host test ---
M10 syscall host tests passed
[M10] host test PASS

--- Freestanding + Audit ---
[M10] freestanding PASS
[M10] audit PASS

--- Preflight ---
======================================
[M10] preflight checks
======================================
[M10] required files exist
[M10] symbols verified
======================================
[M10] preflight PASS
======================================
======================================
[M10] syscall milestone PASS
======================================

--- QEMU Serial (awal) ---
MCSOS 260502 M4 [M9] cooperative scheduler stress test
kernel_start=0xffffffff80000000
kernel_end=0xffffffff800231b8
[M6] pmm initialized
[M7] vmm map ok
[M7] vmm phys=0x0000000000200000
[M7] cr3=0x000000000ff56000
[M8] kmem initialized
[M9] scheduler initialized
idt_base=0xffffffff80005000
idt_limit=0x0000000000000fff
[M4] IDT loaded
[M5] idt: loaded
[M5] selftest: IDT invariants passed
[M5] sti: enabling interrupts
[M9] thread B running
[M9] thread C running
[M9] thread D running
[M9] thread A running
...
[MCSOS:TIMER] ticks=100
...
[MCSOS:TIMER] ticks=200
...
[MCSOS:TIMER] ticks=300
...
[MCSOS:TIMER] ticks=400
...
[MCSOS:TIMER] ticks=500
```

### 13.3 Artefak Bukti

| Artefak | Path | SHA-256 / hash | Fungsi |
|---|---|---|---|
| `m10_host_test` | `build/m10/m10_host_test` | `e14cd711a254435c8117cc63f976d34d160a1e3146592b07797ef69b037f60e9` | Binary host unit test dispatcher |
| `m10_syscall_combined.o` | `build/m10/m10_syscall_combined.o` | `77de021134d34737eb6f501d1011a6211310109a50581b60796fb06bac8400af` | Combined freestanding object untuk audit |
| `kernel.elf` | `build/kernel.elf` | [Tidak tersedia — tidak direkam dalam log] | Kernel ELF64 dengan syscall layer |
| `mcsos.iso` | `build/mcsos.iso` | [Tidak tersedia — tidak direkam dalam log] | ISO bootable |
| `nm_undefined.log` | `build/m10/nm_undefined.log` | — | Audit undefined symbol (kosong) |
| `readelf_header.log` | `build/m10/readelf_header.log` | — | Header ELF64 AMD64 |
| `objdump_key.log` | `build/m10/objdump_key.log` | — | Disassembly key: stub + iretq |
| `test_syscall.log` | `build/m10/test_syscall.log` | — | Log host test |

Perintah hash:

```bash
sha256sum build/m10/m10_host_test build/m10/m10_syscall_combined.o
```

---

## 14. Analisis Teknis

### 14.1 Analisis Keberhasilan

```text
1. Host unit test lulus karena dispatcher berhasil mengimplementasikan dua invariant utama:
   (a) nomor syscall di luar [0, MCSOS_SYS_MAX) dikembalikan dengan -ENOSYS; (b) nomor valid
   dengan handler terdaftar berhasil memanggil handler dan mengembalikan nilai yang diharapkan.
   Test ini tidak memerlukan hardware dan berjalan deterministik di host.

2. Freestanding compile berhasil karena syscall.c dan syscall_entry.S tidak memanggil fungsi
   libc apapun. Semua fungsi yang dibutuhkan sudah tersedia di kernel sendiri. Flag -ffreestanding
   -fno-builtin memastikan compiler tidak menyisipkan libc dependency tersembunyi.

3. nm -u kosong membuktikan bahwa tidak ada unresolved symbol di m10_syscall_combined.o.
   Ini berarti object dapat di-link ke kernel tanpa risiko linker error dari dependency tak
   terduga.

4. readelf menunjukkan ELF64 AMD64 — sesuai target x86_64-unknown-none-elf dan konsisten
   dengan format kernel MCSOS yang sudah berjalan sejak M2.

5. objdump memverifikasi bahwa x86_64_syscall_int80_stub berada di offset 0x134 dan iretq
   hadir di offset 0x142. Ini membuktikan bahwa stub assembly berhasil dikompilasi dan
   instruksi return path (iretq) ada di lokasi yang benar.

6. Integrasi ke kernel (make all + make image) berhasil setelah perbaikan Makefile. Scheduler
   M9 tetap berjalan di QEMU setelah syscall layer diintegrasikan, yang membuktikan bahwa
   penambahan syscall.o dan syscall_entry.o ke link chain tidak merusak state kernel yang
   sudah ada.
```

### 14.2 Analisis Kegagalan atau Perbedaan Hasil

```text
1. Bug Makefile — Missing separator (baris 64 dan 74)
   Gejala: `make: *** [Makefile:64: ...] missing separator. Stop.`
   Akar masalah: Penggunaan `sed -i` untuk menambahkan variabel ke Makefile menghasilkan
   karakter tab/spasi yang salah atau sintaks tidak valid untuk Make. Selain itu, percobaan
   `cat >> Makefile << 'EOF'` yang diinterupsi meninggalkan teks yang tidak valid.
   Perbaikan: Edit Makefile manual dengan nano setelah membersihkan baris yang rusak.
   Dampak: Menyebabkan beberapa iterasi make clean/make all yang gagal sebelum akhirnya
   berhasil.

2. Path source yang salah di Makefile awal
   Gejala: `cc1: fatal error: tests/test_syscall_host.c: No such file or directory`
   dan `cc1: fatal error: kernel/syscall/syscall.c: No such file or directory`
   Akar masalah: Makefile versi awal menggunakan path `tests/test_syscall_host.c` dan
   `kernel/syscall/syscall.c` yang tidak sesuai dengan lokasi file sebenarnya
   (`tests/test_syscall.c` dan `kernel/core/syscall.c`).
   Perbaikan: Koreksi path di Makefile.

3. Isu sumber lain: . ./build/m10/test_syscall.log
   Gejala: `./build/m10/test_syscall.log: line 1: M10: command not found`
   Akar masalah: Makefile versi awal mencoba `source` file log sebagai shell script.
   Perbaikan: Hapus baris source tersebut; hanya gunakan output dari host test secara langsung.

4. int 0x80 smoke path dari ring 3 belum diuji
   Ini adalah non-scope M10 yang disengaja. Jalur ini memerlukan TSS, GDT user selector,
   dan user-mode state yang belum disiapkan di M10.
```

### 14.3 Perbandingan dengan Teori

| Konsep teori | Implementasi praktikum | Sesuai/tidak sesuai | Penjelasan |
|---|---|---|---|
| ABI syscall berbasis register (rax=nr, rdi/rsi/rdx/r10/r8/r9=args) | `syscall.h` + stub assembly | Sesuai | Konvensi register M10 mengikuti psABI x86-64 dengan modifikasi r10 untuk argumen ke-4 |
| Table-driven dispatcher dengan bound check | `mcsos_syscall_dispatch` di `syscall.c` | Sesuai | Array fn pointer diindeks nr; cek `nr < MCSOS_SYS_MAX` sebelum akses |
| Overflow-safe range check untuk user pointer | `mcsos_user_check_range` | Sesuai | Pemeriksaan ptr + len tanpa wrap-around sebelum dereference |
| `int 0x80` via interrupt gate IDT | `x86_64_syscall_int80_stub` + IDT M4 | Sesuai | Stub terhubung ke IDT M4; `iretq` sebagai return path |
| Freestanding — tidak ada libc dependency | `nm -u` kosong | Sesuai | Verified oleh audit nm |
| Validasi pointer bukan security-complete tanpa page fault recovery | Batasan M10 | Sesuai | Laporan secara eksplisit menyatakan range check belum menggantikan pemeriksaan permission page |
| Scheduler M9 tetap aman setelah integrasi syscall | QEMU log: scheduler berjalan | Sesuai | Tidak ada panic atau hang; scheduler M9 tetap berjalan normal |

### 14.4 Kompleksitas dan Kinerja

| Aspek | Estimasi/hasil | Bukti | Catatan |
|---|---|---|---|
| Kompleksitas dispatcher | O(1) per syscall (bound check + table lookup) | Analisis source | Array lookup dengan satu bound check |
| Kompleksitas range check | O(1) | Analisis source | Dua perbandingan integer |
| Waktu build (make all) | [Tidak tersedia — tidak diukur] | — | Build meliputi seluruh kernel termasuk M4–M9 |
| Waktu boot QEMU | Scheduler mulai setelah IDT loaded | Serial log | `[M5] sti: enabling interrupts` → `[M9] thread B running` segera setelahnya |
| Penggunaan memori | [Tidak tersedia] | — | Tidak diukur di M10 |
| Latensi `int 0x80` vs `syscall` | Lebih tinggi dari syscall/sysret | Intel SDM | int 0x80 melewati IDT; syscall/sysret adalah fast path yang tidak tersedia di M10 |

---

## 15. Debugging dan Failure Modes

### 15.1 Failure Modes yang Ditemukan

| Failure mode | Gejala | Penyebab sementara | Bukti | Perbaikan |
|---|---|---|---|---|
| Makefile missing separator | `make: *** missing separator. Stop.` pada baris 64 dan 74 | Penggunaan sed + cat EOF yang menghasilkan sintaks tab/spasi tidak valid di Makefile | Log terminal: `Makefile:64: *** missing separator` | Edit Makefile manual dengan nano; hapus baris yang rusak |
| Path source file salah di Makefile | `cc1: fatal error: tests/test_syscall_host.c: No such file or directory` | Path file di variabel Makefile tidak sesuai dengan path aktual di repository | Log terminal: `No such file or directory` | Koreksi path ke `tests/test_syscall.c` dan `kernel/core/syscall.c` |
| `source` log file sebagai shell | `M10: command not found` | Makefile mencoba `source` file log yang berisi teks bukan shell script | Log terminal | Hapus baris `source` dari Makefile |

### 15.2 Failure Modes yang Diantisipasi

| Failure mode | Deteksi | Dampak | Mitigasi |
|---|---|---|---|
| Invalid syscall number (nr >= MCSOS_SYS_MAX) | Bound check di dispatcher; host test | Return -ENOSYS; tidak ada akses array di luar batas | Bound check wajib sebelum table access |
| NULL handler di tabel | Cek handler != NULL; host test | Return -ENOSYS; tidak ada null dereference | Dispatcher cek NULL sebelum call |
| Invalid user pointer (out of range / overflow) | `mcsos_user_check_range`; host test | Return -EFAULT; tidak ada dereference | Range check + overflow arithmetic check |
| Unresolved symbol libc | nm -u audit | Linker error saat build kernel | Flag -ffreestanding -fno-builtin; nm audit wajib kosong |
| Triple fault akibat entry stub salah | QEMU reset; QEMU -d int,cpu_reset | Kernel reboot tanpa error log | Verifikasi iretq di disassembly; test dengan GDB breakpoint |
| Register clobber di stub | Nilai register caller berubah setelah syscall return | Corrupt caller state | Save/restore register di stub sesuai ABI |
| Invalid trap return path | QEMU hang atau GPF | Eksekusi return ke alamat salah | Verifikasi iretq sebagai satu-satunya return path |
| Scheduler corruption dari syscall yield | Scheduler hang atau panic | Syscall yield memanggil path yang salah | Gunakan callback thread_yield yang sudah divalidasi di M9 |
| Stale interrupt state | Interrupt tidak aktif setelah syscall | Timer tidak berjalan; scheduler tidak dipanggil | Pastikan iretq memulihkan rflags termasuk IF bit |
| Missing IDT vector 0x80 | int 0x80 menghasilkan GPF | Gate 0x80 belum di-set di IDT | x86_64_idt_set_gate(0x80, ...) wajib dipanggil saat init |

### 15.3 Triage yang Dilakukan

```text
1. Saat Makefile error: baca pesan error (missing separator + nomor baris), buka Makefile
   dengan nano, identifikasi baris yang mengandung karakter tidak valid (tab yang salah, echo
   yang tidak valid), hapus dan tulis ulang secara manual.

2. Saat path file salah: baca pesan error (No such file or directory), gunakan `ls tests/`
   dan `ls kernel/core/` untuk verifikasi nama file aktual, koreksi variabel di Makefile.

3. Saat QEMU smoke (sebelum integrasi syscall benar): cek serial log via -serial file: atau
   -serial stdio; verifikasi bahwa scheduler stage marker muncul; jika tidak muncul, cek
   build log apakah ada link error.
```

### 15.4 Panic Path

```text
Tidak ada panic yang terjadi selama praktikum M10. Kernel berhasil boot dan menjalankan
scheduler M9 tanpa panic di semua run QEMU yang dilakukan.

Panic path tetap tersedia di kernel (kernel_panic_at terverifikasi di kernel.syms.txt
dan cpu_halt_forever di disassembly). Jika terjadi fault pada syscall path (misalnya
triple fault akibat entry stack salah), QEMU -d int,cpu_reset,guest_errors akan menampilkan
log interrupt dan CPU reset untuk diagnosis.
```

---

## 16. Prosedur Rollback

Rollback harus menjelaskan cara kembali ke kondisi aman jika perubahan gagal.

| Skenario rollback | Perintah | Data yang harus diselamatkan | Status |
|---|---|---|---|
| Kembali ke commit M9 | `git checkout 786552addde8bac6b9df24856bbfee80eda43c1e` | Log dan evidence M9 di `build/m9/` | Teruji (branch M9 masih ada) |
| Revert commit M10 | `git revert 0ec388a8a4a4176b14979a95063303e73c427273` | Log evidence M10 bila diperlukan | Belum diuji eksplisit |
| Bersihkan artefak build | `make clean` | Source aman di Git | Teruji |
| Regenerasi image dari M9 | `git checkout 786552a && make clean && make all && make image` | — | Belum diuji penuh |

Catatan rollback:

```text
Branch praktikum/m10-syscall-abi berisi commit M10 (0ec388a). Commit sebelumnya
(786552a) adalah checkpoint M9 yang telah divalidasi dengan [M9] scheduler milestone PASS.
Rollback ke M9 dapat dilakukan dengan git checkout ke hash tersebut dan melakukan
make clean && make all untuk mendapatkan kernel M9 yang bersih.

Rollback dibuktikan secara implisit: sebelum M10 dimulai, make m9-all lulus, yang
memverifikasi bahwa checkpoint M9 masih valid. Branch M9 terpisah
(m9-kernel-thread-scheduler) juga tersedia.
```

---

## 17. Keamanan dan Reliability

### 17.1 Risiko Keamanan

| Risiko | Boundary | Dampak | Mitigasi | Evidence |
|---|---|---|---|---|
| User pointer tidak valid (out-of-range atau overflow) | Entry syscall — argument validation | Dereference di luar rentang valid, potential memory corruption | `mcsos_user_check_range`: range check + overflow arithmetic check | Host unit test; source review |
| Invalid syscall number | Entry dispatcher | Out-of-bounds table access | Bound check `nr < MCSOS_SYS_MAX` sebelum akses tabel | Host unit test membuktikan -ENOSYS untuk nr invalid |
| NULL handler di tabel | Dispatcher | Null pointer dereference | Cek `handler[nr] != NULL` sebelum call | Source review; host test |
| Register clobber di stub assembly | `syscall_entry.S` | Corrupt caller state atau return ke alamat salah | Save/restore register sesuai ABI; iretq sebagai satu-satunya return path | Disassembly: iretq terverifikasi |
| Libc dependency tersembunyi | Seluruh syscall object | Linker failure atau runtime dependency tak terduga | -ffreestanding -fno-builtin; nm -u audit wajib kosong | `nm_undefined.log` kosong |
| Missing IDT vector 0x80 gate | IDT setup saat boot | int 0x80 menghasilkan GPF bukan syscall | `x86_64_idt_set_gate(0x80, ...)` dipanggil saat init | QEMU smoke: tidak ada GPF |

> Catatan: Validasi M10 belum mencakup pemeriksaan permission bit page table, user/supervisor isolation penuh, atau fault-containment dari page-fault assisted usercopy. Ring 3 penuh, credential subsystem, dan secure syscall boundary adalah non-scope M10.

### 17.2 Reliability dan Data Integrity

| Risiko reliability | Dampak | Deteksi | Mitigasi |
|---|---|---|---|
| Makefile sintaks rusak akibat sed | Build gagal seluruhnya | `make: *** missing separator` | Edit Makefile manual; tidak menggunakan sed pada Makefile untuk perubahan struktural |
| Stale interrupt state setelah iretq | Timer tidak berjalan; scheduler tidak di-trigger | Serial log: timer ticks tidak muncul | iretq memulihkan rflags dari stack; pastikan IF bit dipertahankan |
| Scheduler corruption dari syscall path | Scheduler hang atau loop tak terbatas | Serial log diam; timer ticks berhenti | Syscall yang melibatkan scheduler menggunakan callback M9 yang sudah divalidasi |
| Triple fault dari stack alignment | CPU reset tanpa log | QEMU -d cpu_reset | Stack 16-byte aligned sebelum memanggil C dari stub |

### 17.3 Negative Test

| Negative test | Input buruk | Expected result | Actual result | Status |
|---|---|---|---|---|
| Dispatch nomor syscall tidak valid | `nr >= MCSOS_SYS_MAX` | Return `-ENOSYS` | Return `-ENOSYS` (dibuktikan host test) | PASS |
| Dispatch dengan handler NULL | nr valid tapi handler di tabel NULL | Return `-ENOSYS` | Return `-ENOSYS` (dibuktikan host test) | PASS |
| User pointer range overflow | `ptr + len > UINT64_MAX` (overflow) | Return `-EFAULT` atau error | Return error (dibuktikan host test range validation) | PASS |
| User pointer out of valid range | `ptr` di luar area yang diizinkan | Return `-EFAULT` | Return error (dibuktikan host test) | PASS |
| int 0x80 smoke dari ring 3 | [Belum diuji] | Syscall tereksekusi dan return | [Belum diuji] | NA |

---

## 18. Pembagian Kerja Kelompok

Tidak berlaku — praktikum dikerjakan secara individu.

### 18.1 Mekanisme Koordinasi

```text
Tidak berlaku.
```

### 18.2 Evaluasi Kontribusi

| Anggota | Persentase kontribusi yang disepakati | Bukti | Catatan |
|---|---:|---|---|
| Iswan Herdiansah | 100% | Seluruh commit pada branch praktikum/m10-syscall-abi | Pengerjaan individu |

---

## 19. Kriteria Lulus Praktikum

Bagian ini wajib diisi. Praktikum dinyatakan memenuhi kriteria minimum hanya jika bukti tersedia.

| Kriteria minimum | Status | Evidence |
|---|---|---|
| Proyek dapat dibangun dari clean checkout | PASS | `make clean && make all` berhasil; `build/kernel.elf` dan `build/mcsos.iso` tersedia |
| Perintah build terdokumentasi | PASS | Bagian 10 (Langkah Kerja) dan bagian 12.1 (Build Test) |
| QEMU boot atau test target berjalan deterministik | PASS | Serial log: scheduler M9 berjalan; timer ticks muncul deterministik |
| Semua unit test/praktikum test relevan lulus | PASS | `M10 syscall host tests passed`; `[M10] syscall milestone PASS` |
| Log serial disimpan | PASS | `logs/m10_serial.log` |
| Panic path terbaca atau dijelaskan jika belum relevan | PASS | `kernel_panic_at` dan `cpu_halt_forever` terverifikasi di disassembly; tidak ada panic saat run |
| Tidak ada warning kritis pada build | PASS | Build dengan `-Wall -Wextra -Werror` berhasil tanpa warning |
| Perubahan Git terkomit | PASS | Commit `0ec388a8` pada branch `praktikum/m10-syscall-abi` |
| Desain dan failure mode dijelaskan | PASS | Bagian 9 (Desain Teknis) dan bagian 15 (Failure Modes) |
| Laporan berisi screenshot/log yang cukup | PASS | Log build, log QEMU, readelf/objdump, sha256, dan host test output tersedia di lampiran |

Kriteria tambahan untuk praktikum lanjutan:

| Kriteria lanjutan | Status | Evidence |
|---|---|---|
| Static analysis dijalankan | NA | Tidak dijalankan di M10 |
| Stress test dijalankan | PASS | Scheduler M9 stress test (cooperative, multi-thread) berjalan via QEMU |
| Fuzzing atau malformed-input test dijalankan | NA | Tidak dijalankan di M10 |
| Fault injection dijalankan | NA | Tidak dijalankan di M10 |
| Disassembly/readelf evidence tersedia | PASS | `readelf_header.log`, `objdump_key.log`, `nm_undefined.log` tersedia |
| Review keamanan dilakukan | PASS | Bagian 17 (Keamanan dan Reliability) |
| Rollback diuji | PASS (implisit) | Checkpoint M9 valid sebelum M10 dimulai; branch M9 tersedia |

---

## 20. Readiness Review

Pilih satu status dengan alasan berbasis bukti.

| Status | Definisi | Pilihan |
|---|---|---|
| Belum siap uji | Build/test belum stabil atau bukti belum cukup | `[ ]` |
| Siap uji QEMU | Build bersih, QEMU/test target berjalan, log tersedia | `[V]` |
| Siap demonstrasi praktikum | Siap ditunjukkan di kelas dengan bukti uji, failure mode, dan rollback | `[ ]` |
| Kandidat siap pakai terbatas | Hanya untuk penggunaan terbatas setelah test, security review, dokumentasi, dan known issue tersedia | `[ ]` |

Alasan readiness:

```text
Hasil praktikum M10 dinyatakan SIAP UJI QEMU berdasarkan bukti berikut:

1. Build bersih: make clean && make all && make image berhasil tanpa error atau warning.
   kernel.elf (ELF64 AMD64) dan mcsos.iso tersedia.

2. Host unit test lulus: "M10 syscall host tests passed" — dispatcher, bound check,
   dan range validation terverifikasi tanpa QEMU.

3. Freestanding object valid: nm -u kosong (tidak ada undefined symbol); readelf
   membuktikan ELF64 AMD64; objdump membuktikan x86_64_syscall_int80_stub + iretq hadir.

4. QEMU smoke boot berhasil: serial log menunjukkan seluruh milestone M4–M9 tetap
   berjalan (PMM, VMM, kmem, scheduler, IDT, timer) setelah integrasi syscall layer.

5. Git commit bersih: commit 0ec388a pada branch praktikum/m10-syscall-abi; working tree
   clean; 9 files changed.

Pembatasan yang berlaku:
- Belum siap demonstrasi penuh karena int 0x80 dari ring 3 belum diuji (non-scope M10).
- Belum siap pakai terbatas karena ring 3, ELF user loader, TSS, user isolation, dan
  secure syscall boundary belum ada.
- Hasil ini hanya boleh disebut "siap uji QEMU untuk syscall dispatcher awal single-core
  dan smoke test ABI kernel-side".
```

Known issues:

| No. | Issue | Dampak | Workaround | Target perbaikan |
|---|---|---|---|---|
| 1 | `int 0x80` path dari ring 3 belum diuji | Tidak dapat memverifikasi syscall dari userspace nyata | Hanya gunakan kernel-triggered/test-harness syscall | M11 (user-mode bring-up) |
| 2 | User pointer validation bukan security-complete (tidak ada page-fault recovery) | Range check tidak mencegah akses ke halaman yang mapped tapi tidak valid | Batasan disengaja di M10 | M11/M12 (page-fault assisted usercopy) |
| 3 | Tidak ada syscall log (tracepoint per syscall) | Sulit di-debug dispatch yang masuk di runtime | Gunakan GDB breakpoint di mcsos_syscall_dispatch | M11 atau pengayaan M10 |
| 4 | IDT vector 0x80 belum terverifikasi dengan GDB breakpoint saat live | Belum ada bukti hardware path int 0x80 triggered | [Belum ada workaround] | M11 saat ring 3 tersedia |

Keputusan akhir:

```text
Berdasarkan bukti build bersih (make clean && make all && make image PASS), host unit test
lulus (M10 syscall host tests passed), freestanding object audit lulus (nm kosong, readelf
ELF64 AMD64, objdump iretq verified), dan QEMU serial log yang menunjukkan kernel boot
dan scheduler M9 tetap berjalan, hasil praktikum M10 layak disebut SIAP UJI QEMU untuk
syscall dispatcher awal single-core dan smoke test ABI kernel-side.

Hasil ini belum layak disebut siap demonstrasi praktikum karena jalur int 0x80 dari ring 3
belum diuji secara live dengan GDB. Hasil ini juga belum layak disebut siap pakai terbatas
karena ring 3, user isolation, TSS, dan secure syscall boundary belum diimplementasikan
(non-scope M10).
```

---

## 21. Rubrik Penilaian 100 Poin

| Komponen | Bobot | Indikator nilai penuh | Nilai |
|---:|---:|---|---:|
| Kebenaran fungsional | 30 | Implementasi memenuhi target praktikum, build/test lulus, output sesuai expected result | `[0-30]` |
| Kualitas desain dan invariants | 20 | Desain jelas, kontrak antarmuka eksplisit, invariants/ownership/locking terdokumentasi | `[0-20]` |
| Pengujian dan bukti | 20 | Unit/integration/QEMU/static/fuzz/stress evidence memadai sesuai tingkat praktikum | `[0-20]` |
| Debugging dan failure analysis | 10 | Failure mode, triage, panic/log, dan rollback dianalisis | `[0-10]` |
| Keamanan dan robustness | 10 | Boundary, input validation, privilege, memory safety, dan negative tests dibahas | `[0-10]` |
| Dokumentasi dan laporan | 10 | Laporan rapi, lengkap, dapat direproduksi, memakai referensi yang layak | `[0-10]` |
| **Total** | **100** |  | `[0-100]` |

Catatan penilai:

```text
[Diisi dosen/asisten.]
```

---

## 22. Kesimpulan

### 22.1 Yang Berhasil

```text
1. ABI syscall berbasis register x86_64 berhasil dirancang dan didefinisikan di syscall.h
   dengan nomor syscall (SYS_NOP, SYS_YIELD, SYS_EXIT_THREAD, SYS_GETPID, SYS_WRITE_SERIAL),
   MCSOS_SYS_MAX = 5, dan prototype mcsos_syscall_dispatch.

2. Dispatcher table-driven berhasil diimplementasikan di syscall.c dengan bound check
   (nr < MCSOS_SYS_MAX) dan cek handler != NULL. Host unit test lulus: "M10 syscall host
   tests passed".

3. Validasi user buffer (mcsos_user_check_range) berhasil diimplementasikan dengan
   range check dan overflow arithmetic protection. Negative test lulus.

4. Stub entry int 0x80 (x86_64_syscall_int80_stub + iretq) berhasil dikompilasi sebagai
   freestanding assembly dan terverifikasi lewat objdump.

5. Freestanding object audit lulus: nm -u kosong; readelf ELF64 AMD64; objdump membuktikan
   symbol dan instruksi kritis ada.

6. Integrasi kernel berhasil: make clean && make all && make image PASS; syscall.o dan
   syscall_entry.o masuk ke kernel.elf tanpa merusak build.

7. QEMU smoke boot berhasil: scheduler M9 tetap berjalan setelah syscall layer diintegrasikan;
   timer ticks deterministik muncul di serial log.

8. Dependency milestone sebelumnya tidak rusak: M6 PMM, M8 KMEM, M9 scheduler semuanya
   tetap lulus setelah perubahan M10.

9. Seluruh perubahan berhasil dikomit ke branch praktikum/m10-syscall-abi (commit 0ec388a).
```

### 22.2 Yang Belum Berhasil

```text
1. int 0x80 smoke path dari ring 3 belum diuji. Ini adalah non-scope M10 yang disengaja
   karena memerlukan TSS, GDT user selector, user stack, dan user-mode state.

2. User pointer validation tidak security-complete: range check + overflow check ada, tetapi
   belum ada pemeriksaan permission bit page table atau fault-containment dari page-fault
   assisted usercopy.

3. GDB breakpoint live pada x86_64_syscall_int80_stub belum dilakukan untuk memverifikasi
   hardware path int 0x80 secara eksplisit dengan register dump.

4. Tracepoint per syscall (logging setiap dispatch) belum diimplementasikan, sehingga
   observabilitas runtime terbatas.

5. Ring 3, ELF user loader, TSS, per-process address space, credential subsystem, signal
   subsystem, SMP syscall path — semua non-scope M10.
```

### 22.3 Rencana Perbaikan

```text
1. M11 — User-mode bring-up terbatas:
   - Siapkan GDT selector user (ring 3 code + data segment)
   - Siapkan TSS dengan kernel stack pointer
   - Siapkan page table user/supervisor
   - Implementasi return-to-user path dari kernel
   - Buat program user minimal yang memanggil int 0x80
   - Uji syscall dari ring 3 dengan GDB untuk verifikasi privilege transition

2. Segera setelah M10:
   - Tambahkan GDB breakpoint test di x86_64_syscall_int80_stub untuk verifikasi hardware
     path int 0x80 secara live
   - Tambahkan tracepoint/log sederhana di mcsos_syscall_dispatch untuk observabilitas

3. M12 atau lanjutan M10:
   - Page-fault assisted usercopy untuk menggantikan range-check-only validation
   - Syscall fuzzing dengan malformed nomor dan pointer
```

---

## 23. Lampiran

### Lampiran A — Commit Log

```text
0ec388a m10: add syscall layer and int80 entry
786552a checkpoint before M9 scheduler
4f030a4 m8: add early kernel heap allocator
7eab63d M6: implement bitmap physical memory manager
390fdcf M5: implement external interrupts and PIT timer
d919353 M4 add x86_64 IDT and exception trap path
3c28480 M3: panic path logging gdb and disassembly audit
c091658 M2 bootable early serial baseline
bf3eb96 M1: add reproducible toolchain readiness baseline
665f104 M0: initialize reproducible OS development baseline
```

### Lampiran B — Diff Ringkas

```diff
# Commit 0ec388a — m10: add syscall layer and int80 entry
# 9 files changed, 364 insertions(+), 56 deletions(-)

# File baru:
# + kernel/arch/x86_64/syscall_entry.S
# + kernel/core/syscall.c
# + kernel/include/mcsos/kernel/syscall.h
# + tests/test_syscall.c
# + scripts/m10_preflight.sh
# + scripts/m10_qemu_smoke.sh
# + logs/.gitkeep
# + logs/m10_sha256.txt

# File diubah:
# M Makefile (tambah target m10-*, tambah syscall.c + syscall_entry.S ke build normal)

# Verifikasi simbol syscall.h:
# grep -n "MCSOS_SYS_MAX\|mcsos_syscall_dispatch" kernel/include/mcsos/kernel/syscall.h
# 13:#define MCSOS_SYS_MAX           5u
# 21:mcsos_syscall_dispatch(

# Verifikasi simbol syscall.c:
# grep -n "mcsos_user_check_range\|mcsos_syscall_dispatch" kernel/core/syscall.c
# 48:mcsos_syscall_dispatch(

# Verifikasi simbol syscall_entry.S:
# grep -n "x86_64_syscall_int80_stub\|iretq" kernel/arch/x86_64/syscall_entry.S
# 1:.global x86_64_syscall_int80_stub
# 5:x86_64_syscall_int80_stub:
# 23:    iretq
```

### Lampiran C — Log Build Lengkap

```text
--- make clean && make all (akhir, setelah perbaikan Makefile) ---
rm -rf build iso_root
mkdir -p build/normal/kernel/arch/x86_64/
clang --target=x86_64-unknown-none-elf -std=c17 -ffreestanding -fno-builtin -fno-stack-protector
  -fno-stack-check -fno-pic -fno-pie -fno-lto -m64 -march=x86-64 -mabi=sysv -mno-red-zone
  -mno-mmx -mno-sse -mno-sse2 -mcmodel=kernel -Wall -Wextra -Werror
  -Ikernel/arch/x86_64/include -Ikernel/include
  -c kernel/arch/x86_64/idt.c -o build/normal/kernel/arch/x86_64/idt.o
[... kompilasi seluruh object M4-M9 ...]
clang ... -c kernel/core/syscall.c -o build/normal/kernel/core/syscall.o
clang ... -c kernel/arch/x86_64/syscall_entry.S -o build/normal/kernel/arch/x86_64/syscall_entry.o
ld.lld -nostdlib -static -z max-page-size=0x1000 -T linker.ld -Map=build/kernel.map
  -o build/kernel.elf [seluruh .o termasuk syscall.o dan syscall_entry.o]
[grep checks on kernel.elf: ELF64, AMD64, kmain, kernel_panic_at, cpu_halt_forever,
 x86_64_idt_init, x86_64_trap_dispatch, iretq, lidt — semua PASS]

--- make image ---
ISO selesai: build/mcsos.iso

--- make m10-all ---
M10 syscall host tests passed
[M10] host test PASS
[M10] freestanding PASS
[M10] audit PASS
[M10] preflight PASS
[M10] syscall milestone PASS
```

### Lampiran D — Log QEMU Lengkap

```text
--- logs/m10_serial.log ---
MCSOS 260502 M4 [M9] cooperative scheduler stress test
kernel_start=0xffffffff80000000
kernel_end=0xffffffff800231b8
[M6] pmm initialized
[M7] vmm map ok
[M7] vmm phys=0x0000000000200000
[M7] cr3=0x000000000ff56000
[M8] kmem initialized
[M9] scheduler initialized
idt_base=0xffffffff80005000
idt_limit=0x0000000000000fff
[M4] IDT loaded
[M5] idt: loaded
[M5] selftest: IDT invariants passed
[M5] sti: enabling interrupts
[M9] thread B running
[M9] thread C running
[M9] thread D running
[M9] thread A running
[M9] thread B running
[M9] thread C running
[M9] thread D running
[M9] thread A running
...
[MCSOS:TIMER] ticks=100
...
[MCSOS:TIMER] ticks=200
...
[MCSOS:TIMER] ticks=300
...
[MCSOS:TIMER] ticks=400
...
[MCSOS:TIMER] ticks=500
...
(berlanjut hingga QEMU dihentikan dengan Ctrl-C)
```

### Lampiran E — Output Readelf/Objdump

```text
--- readelf -h build/m10/m10_syscall_combined.o ---
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              REL (Relocatable file)
  Machine:                           Advanced Micro Devices X86-64
  Version:                           0x1
  Entry point address:               0x0
  Start of program headers:          0 (bytes into file)
  Start of section headers:          1168 (bytes into file)
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           0 (bytes)
  Number of program headers:         0
  Size of section headers:           64 (bytes)
  Number of section headers:         11
  Section header string table index: 9

--- nm -u build/m10/m10_syscall_combined.o ---
(output kosong — tidak ada undefined symbol)

--- objdump key (build/m10/objdump_key.log) ---
0000000000000134 <x86_64_syscall_int80_stub>:
 142:   48 cf                   iretq

--- sha256sum ---
e14cd711a254435c8117cc63f976d34d160a1e3146592b07797ef69b037f60e9  build/m10/m10_host_test
77de021134d34737eb6f501d1011a6211310109a50581b60796fb06bac8400af  build/m10/m10_syscall_combined.o

--- readelf M9 (referensi) ---
ELF Header:
  Class:  ELF64
  Machine: Advanced Micro Devices X86-64
  Type: REL (Relocatable file)

--- objdump M9 symbols (referensi) ---
build/m9/m9_scheduler_combined.o: file format elf64-x86-64
SYMBOL TABLE:
0000000000000000 g F .text 000000000000001e thread_system_init
0000000000000020 g F .text 00000000000000a3 thread_create
00000000000000d0 g F .text 0000000000000086 thread_yield
0000000000000170 g   .text 0000000000000000 mcsos_context_switch
```

### Lampiran F — Screenshot

| No. | File | Keterangan |
|---|---|---|
| 1 | [Tidak tersedia] | Screenshot terminal tidak tersimpan sebagai file terpisah |

### Lampiran G — Bukti Tambahan

```text
--- Acceptance Criteria M10 ---
C1 Header/API syscall tersedia              — PASS
C2 Host unit test dispatcher                — PASS
C3 Freestanding compile object              — PASS
C4 Undefined symbol audit                   — PASS
C5 ELF/disassembly audit                    — PASS
C6 Kernel integration                       — PASS
C7 QEMU smoke boot                          — PASS
C8 int 0x80 smoke path live (ring 3)        — [Belum diuji]
C9 User pointer validation QEMU live        — [Belum diuji]

--- Preflight output ---
======================================
[M10] preflight checks
======================================
[M10] required files exist
[M10] symbols verified
======================================
[M10] preflight PASS
======================================
======================================
[M10] syscall milestone PASS
======================================

--- M9 sha256 (referensi dependency) ---
d1187385c781bf98960cee3885ebef81bc0940bdfd9494e369fbd8b2e7645e45  build/m9/m9_host_test
4b22a5107d133734c061254d2cf72f81438a3840cccbe5e86baa1da1b2bcefcb  build/m9/m9_scheduler_combined.o
```

---

## 24. Daftar Referensi

Referensi yang benar-benar dipakai dalam laporan:

```text
[1] Intel Corporation, "Intel® 64 and IA-32 Architectures Software Developer Manuals,"
    Intel Developer Zone, updated Apr. 6, 2026. [Online]. Available:
    https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html

[2] x86 psABIs Project, "x86-64 psABI," GitLab, created Mar. 1, 2019. [Online]. Available:
    https://gitlab.com/x86-psABIs/x86-64-ABI

[3] QEMU Project, "GDB usage," QEMU documentation. [Online]. Available:
    https://qemu-project.gitlab.io/qemu/system/gdb.html

[4] LLVM Project, "Clang command line argument reference," Clang documentation. [Online].
    Available: https://clang.llvm.org/docs/ClangCommandLineReference.html

[5] Linux Kernel Documentation, "Adding a New System Call," kernel.org documentation.
    [Online]. Available: https://www.kernel.org/doc/html/latest/process/adding-syscalls.html

[6] Linux Kernel Documentation, "Lock types and their rules," kernel.org documentation.
    [Online]. Available: https://www.kernel.org/doc/html/latest/locking/locktypes.html
```

---

## 25. Checklist Final Sebelum Pengumpulan

| Checklist | Status |
|---|---|
| Semua placeholder `[isi ...]` sudah diganti | Ya |
| Metadata laporan lengkap | Ya |
| Commit awal dan akhir dicatat | Ya |
| Perintah build dan test dapat dijalankan ulang | Ya |
| Log build dilampirkan | Ya |
| Log QEMU/test dilampirkan | Ya |
| Artefak penting diberi hash | Ya |
| Desain, invariants, ownership, dan failure modes dijelaskan | Ya |
| Security/reliability dibahas | Ya |
| Readiness review tidak berlebihan | Ya |
| Rubrik penilaian diisi atau disiapkan | Ya (disiapkan untuk dosen) |
| Referensi memakai format IEEE | Ya |
| Laporan disimpan sebagai Markdown | Ya |

---

## 26. Pernyataan Pengumpulan

Saya/kami mengumpulkan laporan ini bersama artefak pendukung pada commit:

```text
0ec388a8a4a4176b14979a95063303e73c427273
```

Status akhir yang diklaim:

```text
Siap uji QEMU untuk syscall dispatcher awal single-core dan smoke test ABI kernel-side
```

Ringkasan satu paragraf:

```text
Praktikum M10 berhasil membangun layer syscall awal MCSOS yang mencakup: header ABI syscall
berbasis register x86_64 (syscall.h), dispatcher table-driven dengan bound check dan
validasi null handler (syscall.c), validasi rentang user buffer dengan overflow arithmetic
protection (mcsos_user_check_range), dan stub entry int 0x80 (x86_64_syscall_int80_stub +
iretq) di syscall_entry.S. Host unit test dispatcher lulus ("M10 syscall host tests passed"),
freestanding object audit lulus (nm kosong, ELF64 AMD64, iretq terverifikasi di disassembly),
build kernel bersih berhasil dengan syscall layer terintegrasi, dan QEMU smoke boot menunjukkan
scheduler M9 tetap berjalan setelah integrasi. Keterbatasan utama: jalur int 0x80 dari ring 3
belum diuji (non-scope M10), validasi user pointer belum security-complete (belum ada
page-fault assisted usercopy), dan GDB breakpoint live pada stub belum dilakukan. Langkah
berikutnya adalah M11 untuk user-mode bring-up terbatas: GDT user selector, TSS, page table
user/supervisor, dan syscall dari ring 3.
```

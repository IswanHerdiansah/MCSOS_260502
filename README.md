<div align="center">

# 🖥️ MCSOS_260502

### Mini Custom Simple Operating System

Repository pengembangan sistem operasi berbasis **x86_64** yang dibangun secara bertahap mulai **Milestone M0 hingga M16** menggunakan **C**, **Assembly**, **LLVM/Clang**, **Limine Bootloader**, dan **QEMU**.

![Platform](https://img.shields.io/badge/Platform-x86__64-blue)
![Compiler](https://img.shields.io/badge/Compiler-Clang-success)
![Bootloader](https://img.shields.io/badge/Bootloader-Limine-orange)
![Language](https://img.shields.io/badge/Language-C%20%7C%20Assembly-red)
![Status](https://img.shields.io/badge/M0--M16-Completed-brightgreen)
![CI](https://img.shields.io/badge/GitHub_Actions-Passing-success)

---

> "Learning Operating System by Building One."

</div>

---

# 📖 About

MCSOS (Mini Custom Simple Operating System) merupakan proyek pembelajaran sistem operasi berbasis **x86_64** yang dikembangkan secara bertahap.

Repository ini mendokumentasikan seluruh proses implementasi mulai dari:

- Bootloader
- Kernel
- Memory Management
- Scheduler
- System Call
- ELF Loader
- Synchronization
- Virtual File System
- Block Device
- Filesystem
- Journaling Recovery

Seluruh milestone berhasil dibangun menggunakan LLVM Toolchain dan diuji menggunakan GitHub Actions.

---

# 👨‍💻 Author

| | |
|------|------|
| Nama | **Iswan Herdiansah** |
| Repository | **MCSOS_260502** |
| Architecture | x86_64 |
| Language | C17 + Assembly |
| Compiler | LLVM Clang |
| Linker | LLD |
| Emulator | QEMU |
| Bootloader | Limine |

---

# 🚀 Project Progress

| Milestone | Deskripsi | Status |
|------------|-----------|--------|
| M0 | Development Environment Setup | ✅ |
| M1 | Toolchain Validation | ✅ |
| M2 | Bootable Kernel | ✅ |
| M3 | Panic Handler & Debugging | ✅ |
| M4 | Interrupt Descriptor Table | ✅ |
| M5 | Timer Interrupt | ✅ |
| M6 | Physical Memory Manager | ✅ |
| M7 | Virtual Memory Manager | ✅ |
| M8 | Kernel Heap Allocator | ✅ |
| M9 | Kernel Scheduler | ✅ |
| M10 | System Call ABI | ✅ |
| M11 | ELF User Loader | ✅ |
| M12 | Synchronization | ✅ |
| M13 | Virtual File System | ✅ |
| M14 | Block Device Layer | ✅ |
| M15 | MCSFS1 Filesystem | ✅ |
| M16 | Journaling Recovery | ✅ |

---

# 📈 Milestone Timeline

```text
M0  Environment
 │
 ▼
M1  Toolchain
 │
 ▼
M2  Bootable Kernel
 │
 ▼
M3  Panic & Debug
 │
 ▼
M4  Interrupt
 │
 ▼
M5  Timer IRQ
 │
 ▼
M6  Physical Memory
 │
 ▼
M7  Virtual Memory
 │
 ▼
M8  Kernel Heap
 │
 ▼
M9  Scheduler
 │
 ▼
M10 System Call
 │
 ▼
M11 ELF Loader
 │
 ▼
M12 Synchronization
 │
 ▼
M13 VFS
 │
 ▼
M14 Block Device
 │
 ▼
M15 MCSFS1
 │
 ▼
M16 Journaling Recovery ✅
```

---

# 🏗️ Kernel Architecture

```text
               +---------------------+
               |   Limine Bootloader |
               +----------+----------+
                          |
                          ▼
                  +---------------+
                  |    Kernel     |
                  +-------+-------+
                          |
      +-------------------+-------------------+
      |                   |                   |
      ▼                   ▼                   ▼
 Memory Manager      Scheduler          System Call
      |                   |                   |
      +-------------------+-------------------+
                          |
                          ▼
                    ELF Loader
                          |
                          ▼
                 Virtual File System
                          |
                          ▼
                    Block Device
                          |
                          ▼
                    MCSFS1 Filesystem
                          |
                          ▼
                 Journaling Recovery
```

---

# 📂 Repository Structure

```text
📦 MCSOS_260502
 ┣ 📂 .github
 ┃ ┗ 📂 workflows
 ┣ 📂 build
 ┣ 📂 configs
 ┣ 📂 docs
 ┣ 📂 evidence
 ┣ 📂 kernel
 ┣ 📂 scripts
 ┣ 📂 tests
 ┣ 📜 Makefile
 ┗ 📜 README.md
```

---

# 🛠️ Build

```bash
make build
```

---

# 🧪 Run M16 Test

```bash
make m16-all
```

Expected output

```text
M16 host test PASS
[M16] host PASS
[M16] freestanding PASS
[M16] audit PASS
M16_PREFLIGHT_DONE
[M16] journaling milestone PASS
```

---

# ⚙️ Continuous Integration

Repository ini menggunakan **GitHub Actions**.

Workflow:

```
M16 Toolchain Check
```

Command yang dijalankan:

```bash
make m16-all
```

---

# ✨ Features

- ✅ Bootable Kernel
- ✅ Interrupt Handling
- ✅ Physical Memory Manager
- ✅ Virtual Memory Manager
- ✅ Kernel Heap
- ✅ Scheduler
- ✅ System Call ABI
- ✅ ELF Loader
- ✅ Synchronization
- ✅ Virtual File System
- ✅ Block Device
- ✅ MCSFS1 Filesystem
- ✅ Journaling Recovery
- ✅ GitHub Actions CI

---

# 📊 Project Statistics

| Item | Status |
|------|--------|
| Milestone Completed | **M0 – M16** |
| Architecture | x86_64 |
| Language | C + Assembly |
| Build System | GNU Make |
| Bootloader | Limine |
| Compiler | LLVM Clang |
| Emulator | QEMU |
| GitHub Actions | ✅ Passing |

---

<div align="center">

## ⭐ MCSOS Development Complete (M0–M16)

**Developed by Iswan Herdiansah**

</div>

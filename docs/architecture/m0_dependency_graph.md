# M0 Dependency Graph

```mermaid
graph TD

A[Windows Host]
B[WSL2 Ubuntu]
C[Toolchain]
D[Smoke Test]
E[Metadata]
F[Evidence]

A --> B
B --> C
C --> D
C --> E
D --> F
E --> F
```

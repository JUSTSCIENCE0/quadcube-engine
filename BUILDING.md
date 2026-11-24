# Building

## Windows
### Prerequisites
- Visual Studio 2022 or later with C++ development tools installed.
- CMake 3.22 or later.
- [KTX-Software v4.4.2](https://github.com/KhronosGroup/KTX-Software/releases#release-v4.4.2) for texture loading.
- [AMD Compressonator V.4.6.0 SDK](https://github.com/GPUOpen-Tools/compressonator/releases#release-V4.6.0) installed for x64. The SDK must contain `include\compressonator.h`, `lib\bin\x64\Compressonator_MD.lib`, and `lib\bin\x64\Compressonator_MDd.lib`.

The SDK is searched for automatically. Environment variables are optional; set either `COMPRESSONATOR_ROOT` or `AMD_COMPRESSONATOR_ROOT` to the SDK installation directory only if CMake does not find it automatically. For example, in PowerShell:

```powershell
$env:COMPRESSONATOR_ROOT = "C:\Compressonator_4.6.0"
```

The finder reports the discovered header and library paths during CMake configuration.

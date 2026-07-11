# Adding GitHub Libraries via CPM

This guide explains how to add external C/C++ libraries from GitHub using **CPM.cmake** (CMake Package Manager) in this project — and how that approach differs from the **ESP-IDF Component Manager**.

---

## Two Distinct Dependency Systems

This project supports two separate ways to pull in external code. They are **independent of each other** and serve different purposes.

| | CPM.cmake | ESP-IDF Component Manager |
|---|---|---|
| **Config file** | Root `CMakeLists.txt` | `main/idf_component.yml` |
| **Registry / Source** | Any GitHub repo, URL, or local path | [components.espressif.com](https://components.espressif.com) |
| **Intended for** | Generic C/C++ libraries (header-only, CMake-based) | ESP-IDF components (hardware drivers, protocols, etc.) |
| **Download location** | `build/_deps/` | `managed_components/` |
| **Linked via** | `target_link_libraries()` in `main/CMakeLists.txt` | Declared in `REQUIRES` inside `idf_component_register()` |

> Do **not** mix them up. A library available as an ESP-IDF component should be added through `idf_component.yml`, not CPM — and vice versa.

---

## How CPM is Bootstrapped

CPM itself is self-downloading. The root `CMakeLists.txt` contains a bootstrap block that fetches the CPM script on first configure if it is not already present:

```cmake
set(CPM_DOWNLOAD_VERSION 0.40.2)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
if(NOT EXISTS "${CPM_DOWNLOAD_LOCATION}")
    message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
    file(DOWNLOAD
        "https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake"
        "${CPM_DOWNLOAD_LOCATION}"
    )
endif()
include("${CPM_DOWNLOAD_LOCATION}")
```

The downloaded script is cached inside the build directory, so internet access is only required once per clean build.

---

## Step 1 — Declare the Package in the Root `CMakeLists.txt`

After the `include("${CPM_DOWNLOAD_LOCATION}")` line, call `CPMAddPackage` for each library you want:

```cmake
CPMAddPackage(
    NAME etl
    GITHUB_REPOSITORY ETLCPP/etl
    GIT_TAG 20.48.0
)
```

**Parameters:**

| Parameter | Description |
|---|---|
| `NAME` | Identifier used to refer to the package later (e.g. in `target_link_libraries`) |
| `GITHUB_REPOSITORY` | `<owner>/<repo>` on GitHub |
| `GIT_TAG` | A tag, branch name, or full commit SHA to pin the version |

CPM will clone the repository into `build/_deps/<name>-src/` and make its CMake targets available.

### Pinning to a specific commit

For reproducible builds, prefer a commit SHA over a branch name:

```cmake
CPMAddPackage(
    NAME etl
    GITHUB_REPOSITORY ETLCPP/etl
    GIT_TAG a1b2c3d4e5f6...  # full 40-char SHA
)
```

---

## Step 2 — Link the Library to Your Component

In `main/CMakeLists.txt`, after `idf_component_register(...)`, add:

```cmake
target_link_libraries(${COMPONENT_LIB} PRIVATE etl::etl)
```

- `${COMPONENT_LIB}` is the CMake target ESP-IDF creates for the `main` component.
- `etl::etl` is the CMake target exported by the ETL library. The exact target name depends on the library — check its own `CMakeLists.txt` or README.
- Use `PRIVATE` if the library is an implementation detail, `PUBLIC` if its headers must be visible to components that depend on `main`.

Your `main/CMakeLists.txt` should look like this when finished:

```cmake
idf_component_register(
    SRCS "main.cpp" ...
    INCLUDE_DIRS "."
    REQUIRES esp_driver_gpio ...
)

target_link_libraries(${COMPONENT_LIB} PRIVATE etl::etl)
```

---

## Complete Example — Adding a New Library

Suppose you want to add [nlohmann/json](https://github.com/nlohmann/json).

**Root `CMakeLists.txt`:**
```cmake
CPMAddPackage(
    NAME nlohmann_json
    GITHUB_REPOSITORY nlohmann/json
    GIT_TAG v3.11.3
)
```

**`main/CMakeLists.txt`:**
```cmake
target_link_libraries(${COMPONENT_LIB} PRIVATE nlohmann_json::nlohmann_json)
```

**Usage in your source code:**
```cpp
#include <nlohmann/json.hpp>
```

---

## Contrast: ESP-IDF Component Manager

The ESP-IDF Component Manager is a **separate system** for libraries specifically packaged for ESP-IDF. It is configured in `main/idf_component.yml`:

```yaml
dependencies:
  idf:
    version: '>=4.1.0'
  espressif/mqtt: ^1.0.0
```

Components declared here are downloaded into the `managed_components/` directory at the project root and are registered as proper ESP-IDF components. They integrate with `idf_component_register()`'s `REQUIRES` field:

```cmake
idf_component_register(
    ...
    REQUIRES esp_wifi __idf_espressif__mqtt
)
```

Use this system for Espressif-provided or community ESP-IDF components (MQTT, LCD drivers, sensors, etc.). Use CPM for general-purpose C/C++ libraries that are not packaged as ESP-IDF components.

---

## Troubleshooting

**`CPMAddPackage` target not found at link time**
- Confirm the `CPMAddPackage` call appears *before* `project()` is referenced by any subdir, or at minimum before the component is configured. In ESP-IDF, place all `CPMAddPackage` calls in the root `CMakeLists.txt` after `include(CPM...)`.

**Library CMake targets differ from the `NAME` argument**
- The `NAME` you pass to CPM is not necessarily the CMake target name. Check the library's own `CMakeLists.txt` for `add_library(...)` or `install(TARGETS ...)` to find the correct target.

**CPM re-downloads on every clean build**
- The downloaded source is in `build/_deps/` which is wiped on a full clean. Set `CPM_SOURCE_CACHE` to a path outside the build directory to persist downloaded sources across cleans:
  ```cmake
  set(CPM_SOURCE_CACHE "${CMAKE_SOURCE_DIR}/.cpm_cache")
  ```

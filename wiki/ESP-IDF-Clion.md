Follow the official walkthrough: https://developer.espressif.com/blog/clion/#debugging-a-project

## 1. Configure the toolchain (do this first — everything else depends on it)

- Go to **Settings > Build, Execution, Deployment > Toolchains** → click **Manage toolchains**
- Click `+` → select **System** (Windows: use **MinGW** as the base, per JetBrains' guide)
- In the new toolchain, click **Add environment > From file**, then browse to and select `export.bat` (found in your ESP-IDF install directory, e.g. `C:\Users\<you>\esp\esp-idf\export.bat`)
  - This is the important part: CLion runs this script to pull in `IDF_PATH`, compiler paths, Python env, etc. — everything the build needs. Without it, CMake configuration will fail.
- Click **Apply**

## 2. Configure the CMake profile

Go to **Settings > Build, Execution, Deployment > CMake** and edit the Debug profile:

- **Toolchain**: select the one you just created
- **CMake options**: `-DIDF_TARGET=esp32s3` (swap for your actual chip)
- **Build directory**: `build`
- **Environment**: add `ESPPORT=COM3` (or whatever your board's COM port is) — this scopes the flash port to CMake/build/run without touching your system environment
- Click **OK** and let the project reload (watch for `[Finished]` in the CMake log)

## 3. Build & flash

- Select the `flash` run configuration from the toolbar dropdown
- Click **Build** — CLion handles the `idf.py build flash` equivalent under the hood

## 4. Debugging (optional, more involved)

Full walkthrough: https://developer.espressif.com/blog/clion/#debugging-a-project

Uses CLion's **Debug Servers** feature:

- Enable Debug Servers under **Settings > Advanced Settings > Debugger**
- Add a new debug server (**Generic** template) pointing to:
  - `openocd` executable (under `.espressif/tools/openocd-esp32/...`) with args like `-f board/esp32s3-builtin.cfg`
  - the matching `xtensa-esp-elf-gdb` binary for your chip, with connection args `tcp::3333`
- Disable **Persistent session** on the Debugger tab for stability
- Test the connection with **Test Run** before relying on it

## 5. Install the ESP-IDF plugin

https://plugins.jetbrains.com/plugin/23886-esp-idf — adds native build/flash/monitor actions so you're not hand-rolling shell-script run configurations for `idf.py flash monitor` or `idf.py menuconfig`.

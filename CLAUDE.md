# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build / Run

This repo contains **two build definitions**:

- **CMake (Qt6)**: `CMakeLists.txt` (currently lists a smaller set of sources)
- **qmake**: `GPSSeeder.pro` (lists additional sources such as `src/ui/coordinatewidget.*`, `src/map/fieldmanager.*`, etc.)

### CMake build (Qt6)

From repo root:

```sh
cmake -S . -B build
cmake --build build
```

Run the built app (path depends on generator/config):

```sh
./build/GPSSeeder
```

Notes:
- The CMake target is `GPSSeeder` (see `CMakeLists.txt`).
- Qt6 modules are required (Core/Gui/Widgets/Network/SerialPort/SerialBus/Quick/QuickWidgets/Positioning/Location/Sql).

### qmake build (Qt)

If using the qmake project:

```sh
qmake GPSSeeder.pro -o Makefile
make
```

On Windows with MinGW this is often:

```sh
mingw32-make
```

### Packaging (Windows)

`CMakeLists.txt` configures CPack with **NSIS**. After a successful CMake build:

```sh
cpack --config build/CPackConfig.cmake
```

## Tests / Lint

- No repo-level test targets were found (no `tests/` directory and no `ctest` configuration in the top-level `CMakeLists.txt`).
- No repo-level lint/format configuration was found (only some vendored third-party `.clang-format` files under `src/ThirdParty/`).

## High-level Architecture

GPSSeeder is a **Qt Widgets** application that reads **GNSS (NMEA)** and **IMU (binary frames)** data from serial ports, performs lightweight estimation, visualizes position/heading and generated coverage paths, and can persist data via SQLite.

### Major layers

1. **UI / orchestration** (`src/ui/`)
   - `MainWindow` is the main controller: creates sensors, connects signals/slots, and refreshes UI on timers.
   - UI form: `src/ui/mainwindow.ui`.

2. **Sensor acquisition (serial)** (`src/core/sensor/` + `src/core/fileio/`)
   - `SensorBase`, `ImuSensor`, `GpsSensor`: threading + lifecycle; emit new data to UI.
   - `ImuSerialLoader`: reads from serial and parses IMU binary frames.
   - `GnssSerialLoader`: reads from serial and parses NMEA sentences into `GNSS`.
   - Low-level serial wrapper: `src/core/fileio/serialport.*`.

3. **Data model / shared types** (`src/core/common/`)
   - `src/core/common/types.h` defines the shared `IMU` and `GNSS` structs (Eigen vectors + timestamps).
   - Logging singleton: `src/core/common/logger.*`.
   - Time sync helper: `src/core/common/time_sync.*`.

4. **Estimation / map-side state** (`src/map/`)
   - `src/map/statestimator.*`: maintains a sliding window of points and estimates velocity; also includes line-crossing detection used for seeding logic.
   - `src/map/coordinate.*`: coordinate helpers used by map/path components.

5. **Path / field generation & visualization** (`src/map/`)
   - `src/map/pathgenerator.*`: generates “bow/shuttle”-style coverage paths and provides seeding/headland segmentation.
   - Map display is implemented as a custom widget (see `src/map/mapwidget.*` in the CMake build).
   - Table/field UI interaction (CMake build): `src/map/tablewidget.*`.

6. **Persistence (SQLite)** (`src/database/`)
   - `src/database/dbmanager.*`: creates and writes to SQLite tables (paths, path points, seeding details) via QtSql.

### Data flow (runtime)

- Serial data is read and parsed by loaders:
  - GNSS: `GnssSerialLoader` parses NMEA messages into a `GNSS` struct.
  - IMU: `ImuSerialLoader` parses 0x55-framed packets (types like 0x51–0x54) into an `IMU` struct.
- `ImuSensor` / `GpsSensor` run in their own `QThread` and emit Qt signals carrying the parsed structs.
- `MainWindow` receives these signals (queued to the UI thread) and updates UI elements and map state on timers.

Key entrypoints:
- App startup: `src/main.cpp` creates `MainWindow` and initializes logging.
- UI wiring: `src/ui/mainwindow.{h,cpp}`.
- Sensor threading + emission: `src/core/sensor/sensorinterface.{h,cpp}`.

### Threading model (Qt)

- Each sensor object (`ImuSensor`, `GpsSensor`) is moved to a dedicated `QThread` (see `SensorBase::startThread()` in `src/core/sensor/sensorinterface.cpp`).
- UI receives sensor updates via signals/slots (typically `Qt::QueuedConnection`) to avoid cross-thread UI access.

### Time synchronization

- GNSS data provides a reference time.
- The GPS sensor computes a `dt` offset between local/system time and GNSS time using multiple valid samples, then emits a time-sync completion signal.
- IMU can apply the `dt` to align IMU timestamps to GNSS time (fields in `IMU` include `synced`/`dt`/`gpstime`).

## Notable repo details

- README mentions Gaode (AMap) usage; treat any API keys/config as **external configuration** and avoid hard-coding new secrets into the repo.
- There is a `scripts/nmea_generator.py` utility used to generate/simulate NMEA output over serial for testing.

## When editing

- If you add/remove source files, confirm whether the project is meant to be built via **CMake**, **qmake**, or both, since `CMakeLists.txt` and `GPSSeeder.pro` currently list different sets of sources.

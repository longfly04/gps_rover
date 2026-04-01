# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build / Run

This repo contains both a CMake build and a qmake project, but **CMake is the maintained build definition**.
`GPSSeeder.pro` is stale and references paths/files that no longer match the current tree (for example `src/ui/mapwidget.*`, `src/core/estimator/statestimator.*`, `coordinatewidget.*`, `fieldmanager.*`, and `predictivecontroller.*`). Treat `CMakeLists.txt` as the source of truth unless the user explicitly asks to restore qmake support.

### CMake build (Qt6)

From repo root:

```sh
cmake -S . -B build
cmake --build build
```

Run the app:

```sh
./build/GPSSeeder
```

Optional Windows Qt deployment for packaging:

```sh
cmake -S . -B build -DGPSSEEDER_WINDEPLOYQT=ON
cmake --build build
```

Package with CPack/NSIS after a successful build:

```sh
cpack --config build/CPackConfig.cmake
```

Notes:
- The CMake target is `GPSSeeder`.
- Required Qt6 modules are Core, Gui, Widgets, Network, SerialPort, SerialBus, Quick, QuickWidgets, Positioning, Location, and Sql.
- `GPSSeeder.iss` is also present for Inno Setup packaging and expects the deployed executable/DLL/plugin layout inside `build/`.

### UI regeneration

`src/ui/ui_mainwindow.h` is generated from `src/ui/mainwindow.ui`. Make structural UI changes in the `.ui` file, not the generated header.

If you need to regenerate it manually:

```sh
uic src/ui/mainwindow.ui -o src/ui/ui_mainwindow.h
```

## Tests / Validation

- No repo-level automated tests were found: no `tests/` directory, no `ctest` setup, and no single-test command.
- No repo-level lint/format configuration was found outside vendored third-party files.
- Manual validation helpers exist under `scripts/`:

```sh
python scripts/nmea_generator.py --help
python scripts/modbus_light_simulator.py --help
python scripts/modbus_motor_simulator.py --help
```

These scripts are useful for GPS/PLC simulation and are separate from the Qt build.

## High-level Architecture

GPSSeeder is a Qt Widgets desktop app for GNSS/IMU monitoring, local-frame field planning, trigger-line prediction, and PLC/Modbus-driven seeding workflows. `MainWindow` is the orchestration layer; sensor workers run on dedicated threads; planning and estimation operate in an OA local coordinate frame derived from user-selected O and A reference points.

### Major runtime pieces

1. **Application shell / orchestration**
   - `src/main.cpp` starts the app and logging.
   - `src/ui/mainwindow.{h,cpp,ui}` owns the sensors, timers, coordinate system, block planner, state estimator, Modbus client, and most workflow/UI logic.

2. **Sensor IO and threading**
   - `src/core/sensor/sensorinterface.*` defines `ImuSensor` and `GpsSensor`.
   - Each sensor object moves itself to a dedicated `QThread` and runs a blocking read loop there.
   - Watchdog/reconnect behavior is handled inside the collection loops rather than by Qt event-loop timers.

3. **Protocol parsing**
   - `src/core/fileio/imuserialloader.cpp` parses 0x55-framed IMU packets (0x51-0x54 accel/gyro/angle/mag).
   - `src/core/fileio/gnssserialloader.cpp` parses NMEA sentences and keeps reading past the first position fix to pick up nearby speed data.

4. **Coordinate system and field planning**
   - `src/map/coordinate.*` converts between WGS84, ENU, and the OA local XY frame.
   - OA convention is important throughout the app: +Y is along O→A, and +X is +Y rotated clockwise 90°.
   - `src/map/blockgenerator.*` generates blocks, headlands, control points, trigger lines, and seeding grids from the UI parameters.

5. **State estimation and trigger prediction**
   - `src/map/statestimator.*` maintains a sliding local-position window, estimates velocity by regression, infers travel direction, predicts trigger-line crossings, and supports deferred precise trigger commits.
   - `MainWindow` uses a single-shot `Qt::PreciseTimer` to commit predicted triggers at the scheduled time.

6. **Visualization**
   - `src/map/mapwidget.*` draws the vehicle pose, trajectory, trigger history, and block overlays.
   - `src/map/osmtilelayer.*` supplies the basemap with persistent local tile caching.
   - The current code uses **Esri World Imagery** tiles, even though the README still mentions Gaode/AMap.

7. **Table, persistence, and PLC integration**
   - `src/map/tablewidget.*` renders a compact forward/reverse trigger history table.
   - `src/database/dbmanager.*` stores paths, path points, and seeding details in SQLite.
   - PLC communication is implemented directly in `MainWindow` using `QModbusTcpClient`.

### Runtime data flow

1. The user configures IMU/GPS serial ports and PLC settings in the UI.
2. `MainWindow` starts `ImuSensor` and `GpsSensor`; each worker reads and parses data on its own thread.
3. Sensor signals are queued back to the UI thread. `MainWindow::initSignalsSlots()` deliberately coalesces high-rate IMU/GNSS updates into “latest sample only” UI refreshes so the event queue does not grow without bound.
4. GPS time sync averages the offset from the first 10 valid fixes, then propagates that offset so IMU timestamps can be aligned to GPS time.
5. After O/A and field parameters are configured, `BlockGenerator` produces the trigger plan and `StateEstimator` consumes sensor-derived `SensorData` to predict upcoming crossings.
6. `MapWidget` and `TableWidget` present the live pose, field layout, pending trigger, and committed trigger history.

## Repo-specific gotchas

- If you add or remove C++ sources, update `CMakeLists.txt`; only update `GPSSeeder.pro` if the user explicitly wants qmake support brought back into sync.
- `src/ui/ui_mainwindow.h` is generated and will be overwritten.
- `src/main.cpp` currently uses an absolute Windows icon path (`d:/GPS-Seeder/gps_rover/resource/icon.jpg`), which is brittle outside that machine.
- `default.json` contains sample serial defaults, but the normal runtime flow is UI-driven.

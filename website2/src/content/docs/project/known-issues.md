---
title: Limitations and known issues
description: What useTracker does badly, what is deprecated, and the specific bugs and documentation errors found in the code.
---

useTracker is useful but not polished. This page lists what it does badly and what
is wrong with it, so that nobody discovers these things halfway through a project.

## General limitations

**Linux is the tested platform.** The CMake build covers Windows and macOS and the
dependencies exist there, but those builds get little testing. This is the
longest-standing practical problem with the project.

**It is not easy enough for a beginner.** Using it well requires some
understanding of what the image-processing operations do. More examples and
tutorials would help most, and are the most useful contribution anyone could make.

**Tracking is more complicated than it needs to be.** Track Blobs has a dozen
parameters, several of which interact, and getting a good result takes more
fiddling than it should. Simplifying this is on the list.

**Some methods are dated.** Newer detection and tracking approaches are not
implemented. Contributions are welcome.

**Identity in dense groups is unreliable.** Individuals that overlap for long
enough will be confused. Markers help; heavy crowding still breaks it. For large
groups of unmarked animals, idtracker.ai is the better tool.

**Fixed cameras only.** Background subtraction assumes a mostly static viewpoint.

**No pose estimation.** Position, area, orientation and identity — not body
landmarks or body movements. SLEAP and DeepLabCut are the tools for that.

**Detection needs contrast.** Everything here works from a difference against the
background, a colour range, or a marker. Animals that blend into their substrate —
furred animals on bedding, for instance — are hard, and no amount of parameter
tuning fixes an image that does not contain the distinction.

## Deprecated plugins

These are still present but should not be used for new work.

| Plugin | |
| --- | --- |
| **Safe Erosion** | Does nothing — the implementation is entirely commented out, though it is listed in the interface and has help text describing behaviour it does not have. Use Erosion with Size 1. |
| **Simple Tags** | Superseded by Aruco, which is standard and better tested. |
| **Remote Control** | Bluetooth RFCOMM, Linux-only, off by default at build time. A keyboard-style clicker drives the Stopwatch instead, on any platform. |
| **Track Blobs → Replay** | History replay is not currently reliable; there is an open branch against it. |

Several other plugins would benefit from revision.

## Bugs and inconsistencies

### Results depend slightly on the CPU core count

Per-pixel plugins run on horizontal slices, one per core, and each slice is
processed as an independent image. Neighbourhood operations — erosion, dilation,
adaptive threshold — therefore treat slice boundaries as image borders, so a thin
row of pixels at each boundary is handled differently from the rest of the mask.

The effect is small, but it means the same settings file can give marginally
different masks on machines with different core counts.

Pinning the thread count by hand in the settings file avoids it:

```xml
<Configuration>
  <Threads>4</Threads>
```

The engine honours this key, but `Save Settings` never writes it, so it has to be
re-added after every save from the interface.

### Background sampling times lose their fraction

`BackgroundStartTime` and `BackgroundEndTime` are written as floats but read back
with an integer cast, so `12.5` becomes `12` on reload. Use whole seconds, or
check after loading.

### Adaptive Threshold's "Block Size" is a radius

The value is used as `2 × BlockSize + 1`, so entering 15 gives a 31-pixel window,
and the help's advice that it must be odd is meaningless. The default of 1 gives a
3-pixel window, too small to be useful.

The same plugin also thresholds the difference between the frame and the
**background image**, not the frame itself — which is invisible until someone
loads a background and the behaviour changes.

### In-app help quotes stale defaults

| Plugin | Help says | Code uses |
| --- | --- | --- |
| Background Difference | Threshold 42 | 20 |
| MOG2 | Threshold 16 | 0.0 |

MOG2 is the more consequential: 16 is OpenCV's own default and a sensible value,
while 0.0 is not, so a freshly added MOG2 behaves oddly until it is set.

### README shows a command-line flag that does not exist

The README documents headless runs as `-x pipeline.xml`. There is no `-x` option —
the settings file is passed with `-p` / `--parameters`.

### Keyboard shortcuts need a loaded source

The shortcut handler returns early when no video or camera is open, so
<kbd>Ctrl</kbd>+<kbd>O</kbd>, <kbd>Ctrl</kbd>+<kbd>L</kbd> and <kbd>Esc</kbd> do
nothing on an empty window. Use the menu.

### Registry name does not match the class

The plugin registered as `BackgroundDifference` is implemented by a class called
`ExtractMotion`. Harmless at runtime, confusing when reading the source or editing
a settings file.

## Repository

A 2.2 GB sample video (`MVI_0078.MP4`) is committed and tracked, so every clone
downloads it. `blobs.csv` and `update_status.txt` are also tracked artifacts.
Sample footage would be better hosted outside the source repository.

## Reporting something

Open an issue at
[github.com/AlexandreCampo/useTracker/issues](https://github.com/AlexandreCampo/useTracker/issues)
and attach the settings file — it describes the whole pipeline in one file, which
usually makes a report reproducible straight away.

Bear in mind this is maintained in spare time; see
[history and credits](/project/history/).

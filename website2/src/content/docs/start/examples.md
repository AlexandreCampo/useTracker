---
title: Examples
description: The settings files bundled with useTracker, and some typical pipelines.
---

The `examples/` directory contains commented settings files that can be loaded
directly. They are starting points, not finished configurations — parameters
depend on your camera, lighting and animals.

```bash
./build/useTracker -p examples/fish_pattern_tracking.xml -i yourvideo.mp4
```

Or `File → Load Settings…` in the interface, then `File → Open Source…`.

## Bundled files

| File | Contents |
| --- | --- |
| `underwater_enhance.xml` | Underwater white balance and dehazing, for footage in turbid water |
| `enhancement_gsoc.xml` | Enhancement feeding a GSOC background subtractor |
| `fish_pattern_tracking.xml` | KNN subtraction, erosion, and Pattern Tracker with boxes fitted to the mask blobs |

## Typical pipelines

### Animals in an arena, fixed camera

```
0: Zones of Interest      polygon around the arena
1: Background Diff Knn
2: Erosion                size 2
3: Extract Blobs          min size from a ruler measurement
4: Track Blobs            max entities = number of animals
```

The common case. If the animals stop moving for long periods, an adaptive
background model will gradually absorb them — either lower the learning rate, or
use a static background image instead (see
[zones and background](/guide/zones-background/)).

### Poor visibility

```
0: Zones of Interest
1: White Balance          Underwater mode
2: Dehaze
3: Background Diff Gsoc
4: Erosion                size 1
5: Extract Blobs
6: Pattern Tracker        seed from blobs
```

On turbid footage, repairing the image usually gains more than tuning the
detector. GSOC handles noisy, low-contrast images better than KNN, at a cost in
speed. Pattern Tracker follows targets by appearance, so it survives moments when
detection drops out.

Keep erosion gentle here — faint blobs disappear quickly.

### Tagged individuals

```
0: Zones of Interest
1: Sharpen                mild
2: Aruco                  mask shape Disc
```

Markers carry their own identity, so no tracking step is needed: group the output
by `id`. Most ArUco problems are physical — tags too small to resolve, glare from
glossy printing, curvature, or motion blur. See
[markers and detection](/plugins/markers/).

### Zone occupancy

```
0: Zones of Interest      one polygon per compartment
1: Background Diff Knn
2: Erosion
3: Extract Blobs
4: Track Blobs            max entities 1
```

Each detection is labelled with the region it falls in, so occupancy is a group
operation on the `zone` column rather than a geometry problem afterwards. A
**Timestep** of 0.2 s on the Processing Frame tab is usually plenty for this and
runs several times faster.

## Contributing examples

Working settings files for other study systems are welcome, especially with
comments explaining the choices. See [contributing](/project/contributing/).

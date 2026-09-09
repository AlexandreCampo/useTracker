---
title: Command line
description: Every command-line option.
---

The same binary is the interface and the batch processor. Without `--nogui` it
opens the window; with it, it processes and exits.

```bash
useTracker [options]
```

| Option | Argument | |
| --- | --- | --- |
| `-n`, `--nogui` | | Run without the interface |
| `-p`, `--parameters` | file.xml | Settings file: pipeline and parameters |
| `-i`, `--inputfile` | file… | Input video(s) or image; several are combined into one canvas |
| `-u`, `--usb` | n… | USB camera device number(s), likewise |
| `-m`, `--mask` | image | Zone-map image, pixel value = region number |
| `-b`, `--background` | image | Background image for Background Difference |
| `--start` | seconds | Start processing at this time |
| `-l`, `--length` | seconds | Process this many seconds |
| `--scale` | 0.1–1.0 | Downscale input frames |
| `--stitching` | file.xml | Multi-camera layout |
| `--calibration` | file.xml | Lens calibration |
| `--test` | script | Drive the interface from a script; development aid |
| `-h`, `--help` | | Option list |

A `-v, --version` flag exists in builds compiled with a version string defined.

The README currently shows `-x pipeline.xml` for headless runs. There is no `-x`
option — use `-p`.

## Examples

```bash
# one video
useTracker --nogui -p settings.xml -i video.mp4

# first 60 seconds only, at half resolution, to check the configuration
useTracker --nogui -p settings.xml -i video.mp4 --length 60 --scale 0.5

# live camera
useTracker -u 0

# several videos stitched into one canvas
useTracker --nogui -p settings.xml -i left.mp4 right.mp4 --stitching rig.xml
```

Passing `--start` or `--length` enables time boundaries, overriding the settings
file.

`--scale` does not change output coordinates, which are always written at source
resolution — but pixel-valued parameters such as blob Min Size act on the smaller
mask, so keep the scale consistent across a dataset.

Output is armed automatically with `--nogui`, so the **Output** ticks saved in the
settings file are all that is needed.

## Exit codes

`0` on success, `1` if the source could not be opened.

See [batch processing](/guide/batch/) for running a dataset.

---
title: Markers and detection
description: Aruco, Aruco Color, Simple Tags and the Yolo Detector.
---

## Aruco

Detects standard ArUco markers and reports id, position, orientation and size.

If identity matters and you can attach a tag, this is the most reliable option
available. Everything else infers identity from proximity or appearance and fails
when individuals overlap for long enough; a marker states it, and recovers
immediately once visible again.

| Parameter | Default | |
| --- | --- | --- |
| Min Size | 0.001 | Smallest accepted marker, as a fraction of the image |
| Max Size | 0.1 | Largest accepted, same units |
| Threshold 1 | 19 | Adaptive-threshold window for finding candidate squares |
| Threshold 2 | 7 | Adaptive-threshold constant |
| Mask Shape | None | `None`, `Square` or `Disc` — shape stamped into the mask |
| Mask Radius / Persp. Shift / Value | 1 / 0 / 0 | Perspective shift offsets the stamp for tags seen at an angle |

Sizes are fractions of the image, so the same settings work at any resolution or
downscale. Narrowing the range around your actual marker size is the cheapest way
to remove false detections.

The reported `angle` is a true orientation over the full 360°, unlike a blob's
principal axis. Because `id` is stable, marker output usually needs no tracking
step — group by `id`.

**Most failures are physical, not algorithmic:**

- **Size.** Aim for at least 30–40 pixels across; measure with the Ruler in a test
  recording before running the experiment.
- **Dictionary.** Use the smallest that covers your animals — 4×4 with 50 ids
  rather than 6×6 with 1000. Simpler patterns are distinguished far more reliably
  when small.
- **Flatness.** ArUco assumes a planar marker. A tag curved over a rounded back
  detects poorly.
- **Glare.** Print matte and diffuse the lighting; a specular highlight wipes out
  the pattern at exactly the angles that matter.
- **Motion blur.** Shorten the exposure and add light if detection drops when
  animals move fast.
- Keep a white border of at least one cell around the marker.

Mild [Sharpen](/plugins/enhancement/) before the detector reliably helps on soft
footage.

ArUco moved into OpenCV's `objdetect` module in 4.7; the build handles both the
old and new APIs.

## Aruco Color

A colour-coded marker scheme, with its own small library bundled with useTracker.
Encoding identity in colour as well as black and white means fewer cells are
needed for the same number of ids, so markers can be **physically smaller**.

That is the reason it exists: it has been used with 0.7 mm tags glued to ants'
backs, printed on an ordinary inkjet printer — where standard ArUco at that size
would need specialist printing and optics.

| Parameter | |
| --- | --- |
| Marker Cols / Rows | Cell grid dimensions; must match your printed markers |
| Saturation Threshold | Minimum saturation for a pixel to count as coloured rather than grey |
| Value Threshold | Minimum brightness to classify at all |
| AT Block Size / Constant | Adaptive threshold for finding the dark cell grid |
| Min / Max Marker Area | Accepted size range, **in pixels** (unlike Aruco) |
| Max Hue Deviation | Tolerance around each reference hue |
| Max Marker Range | Search tolerance when grouping cells |
| Dictionary / Reference Hues | The code set and its colour values |

Tune in order: grid dimensions, then size range, then the adaptive threshold, then
colour classification.

**Reference Hues must match what your printer actually produced**, not what you
specified — printed colours drift, and coloured lighting shifts them further. If
markers are found but ids are wrong, check this first: photograph a printed marker
under your experimental lighting and read the real hues with
[Color Segmentation](/plugins/segmentation/).

Colour buys density and costs robustness. Choose standard Aruco unless marker size
is genuinely the binding constraint. Colour markers also need constant neutral
lighting, locked camera white balance, and adequate bitrate — chroma subsampling
in H.264 and JPEG can smear small coloured cells beyond recognition.

## Simple Tags

An older striped-tag scheme predating the ArUco support. It works from the blob
list, so [Extract Blobs](/plugins/tracking/) must run first and the animal must
already be detected.

| Parameter | Default | |
| --- | --- | --- |
| Tag Width / Height | 100 / 20 | Normalised pattern dimensions |

Superseded by [Aruco](#aruco), which is standard, better tested, and has error
detection built into its dictionaries. Kept for existing configurations; not
recommended for new work. See [known issues](/project/known-issues/).

## Yolo Detector

Runs a YOLO model in ONNX format through OpenCV's DNN module. Detections are drawn
on the HUD, written to file, and stamped into the mask, so Extract Blobs and the
trackers consume them like any other detection — the front of a pipeline can be
swapped without changing the rest.

No model ships with useTracker.

```bash
pip install ultralytics
yolo export model=yolov8n.pt format=onnx
```

| Parameter | Default | |
| --- | --- | --- |
| Model (.onnx) | — | Saved in the settings file, reloaded automatically |
| Class Names | — | Optional text file, one name per line |
| Model Type | Auto | Detects v5 vs v8/v11 output layout from the tensor shape |
| Compute Target | CPU | `CPU`, `OpenCL`, `OpenCL FP16`, `Vulkan` |
| Input Size | 640 | 320 / 416 / 512 / 640 / 1280 |
| Confidence | 0.25 | Minimum score kept |
| NMS Threshold | 0.45 | Overlap above which duplicate boxes merge |
| Class Filter | empty | Comma-separated names or ids; empty keeps all |

**Input Size** matters most when animals are small in frame — a 640 network on 4K
footage downsamples enormously. Either raise it or use
[zones](/guide/zones-background/) to crop to the arena.

The GPU targets need a working OpenCL or Vulkan runtime; the plugin probes the
target on load and falls back to CPU silently, so check which target the dialog
reports. Expect a modest gain on integrated graphics. CUDA requires OpenCV itself
to be built with CUDA, which distribution packages are not.

CPU inference runs at a few frames per second at 640×640 — offline analysis
rather than live capture.

If you use this, keep the exact `.onnx` file with your results. An off-the-shelf
COCO model was trained on internet photographs and its accuracy on your species in
your arena is not something to assume — measure it.

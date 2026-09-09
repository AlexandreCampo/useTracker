---
title: Segmentation and morphology
description: Adaptive Threshold, Color Segmentation, Erosion and Dilation.
---

## Adaptive Threshold

Compares each pixel against the average of its own neighbourhood, so uneven
lighting stops mattering.

| Parameter | Default | |
| --- | --- | --- |
| Block Size | 1 | Neighbourhood **radius** — the window used is 2 × BlockSize + 1 |
| Constant (C) | 0 | Subtracted from the local mean; raise to reject more |
| Threshold Method | Mean | `Mean` or `Gaussian` weighting |
| Additive, Invert, Restrict to Zone | | Invert for bright objects on dark backgrounds |

Two things about this plugin are not obvious from the interface:

- The control is labelled *Block Size* and the help says it must be odd, but the
  value is doubled internally. Entering 15 gives a 31-pixel window. The default of
  1 gives a 3-pixel window, which is too small to be useful — set it before
  expecting anything.
- It thresholds the **difference between the frame and the background image**,
  not the frame itself. With no background loaded it behaves as a plain adaptive
  threshold; with one loaded it is closer to an illumination-robust background
  difference.

The window must be substantially larger than your animals, or an animal's own
pixels dominate its local average and it vanishes. Aim for a window 3–5 times an
animal's width, and enter half that.

Unlike the background models it has no temporal state at all, so a motionless
animal is never absorbed — a reasonable choice when animals rest often and
contrast is good.

## Color Segmentation

Keeps pixels whose colour falls in a range.

| Space | Channels |
| --- | --- |
| HSV | Hue 0–180, Saturation 0–255, Value 0–255 |
| BGR | Blue, Green, Red, 0–255 |

Use HSV. In BGR, the same object in shade and in light has completely different
coordinates, so any range wide enough for both admits much else. In HSV the hue
stays put and only Value moves, so you can be tight on hue and loose on
brightness.

OpenCV's hue is 0–180, not 0–360:

| Colour | Hue |
| --- | --- |
| Red | 0 and 180 (wraps) |
| Orange | 10–20 |
| Yellow | 25–35 |
| Green | 40–80 |
| Cyan | 85–95 |
| Blue | 100–130 |
| Magenta | 140–170 |

Red spans both ends, so one range cannot capture it — use two Color Segmentation
plugins (0–10 and 170–180) with **Additive** on for the second.

To tune: set Saturation Min around 60 and Value Min around 40 to drop greys and
dark pixels, narrow the hue around the target, then widen Value again as far as
possible for lighting tolerance.

Best on artificial marks. Most animals are broadly the colour of their substrate,
which is not a coincidence. Also note that JPEG and H.264 store colour at reduced
resolution, so small coloured marks need a decent bitrate.

## Erosion and Dilation

| Plugin | Effect on white regions |
| --- | --- |
| Erosion | Shrinks them — removes speckle, breaks thin bridges |
| Dilation | Grows them — fills holes, joins fragments |

Both take a single **Size** (default 1), in mask pixels — so it means something
different when the pipeline runs downscaled.

**Erosion then Dilation** removes noise while roughly preserving object size. It
is the usual answer to a speckly mask.

**Dilation then Erosion** fills holes and joins fragments while roughly
preserving size — for animals detected in pieces.

Size 1 or 2 is right more often than expected. Large erosion deletes small
animals in every frame, which in the output looks exactly like those animals not
being present. Raise it one step at a time while watching your smallest animal,
and stop at the last value where it survives.

For bulk noise rejection prefer **Min Size** in
[Extract Blobs](/plugins/tracking/): it discriminates by area and has no side
effect on what it keeps. Erosion earns its place when noise is *thin* rather than
merely small — such as a bridge between two animals that would otherwise merge
into one blob.

Both run in parallel slices, so results depend slightly on the core count; see
[known issues](/project/known-issues/).

## Safe Erosion

Listed under Morphology, and described in the help as an erosion that never fully
removes a small object. **Its implementation is commented out — it does nothing.**
Use Erosion with Size 1. See [known issues](/project/known-issues/).

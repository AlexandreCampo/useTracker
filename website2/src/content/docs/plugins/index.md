---
title: All plugins
description: Every plugin in useTracker, one line each, with links to the detailed pages.
---

34 plugins, grouped as they appear in the interface. Every plugin dialog also has
a **?** button showing help for its own parameters.

Some are old and not worth using; those are marked and listed in
[known issues](/project/known-issues/).

## [Image enhancement](/plugins/enhancement/)

Modify the frame in place, before detection. They never affect what Record Video
writes.

| Plugin | |
| --- | --- |
| White Balance | Colour casts. The Underwater mode restores the red channel absorbed by water. |
| Curves | Per-channel tone curves — levels, gamma, contrast, colour balance. |
| Clahe | Local contrast enhancement. |
| Dehaze | Removes veiling backscatter in turbid water or fog. |
| Denoise | Single-frame noise reduction, bilateral or NL-means. |
| Temporal Denoise | Multi-frame denoising. Much stronger, much slower. |
| Sharpen | Unsharp mask. |

## [Background subtraction](/plugins/background/)

Usually the first detector in a pipeline.

| Plugin | |
| --- | --- |
| Background Difference | Subtracts a fixed background image. Predictable; needs a stable scene. |
| Background Diff Knn | Adaptive, fast, robust. The usual first choice. |
| Background Diff Gsoc | Adaptive, better on noisy or low-contrast footage. Slower. |
| Background Diff Mog / Mog2 | Mixture of Gaussians. Mog2 can classify shadows. |
| Background Diff Gmg | Bayesian. Long warm-up; rarely the best option now. |
| Frame Difference | Compares consecutive frames. Detects motion, not objects. |
| Moving Average | Smooths the mask over time. |

## [Segmentation and morphology](/plugins/segmentation/)

| Plugin | |
| --- | --- |
| Adaptive Threshold | Thresholds against a local neighbourhood; copes with uneven lighting. |
| Color Segmentation | Keeps pixels inside a colour range, HSV or BGR. |
| Erosion | Shrinks white regions; removes speckle. |
| Dilation | Grows white regions; fills holes. |
| Safe Erosion | **Does nothing** — the implementation is commented out. |

## [Blobs and tracking](/plugins/tracking/)

| Plugin | |
| --- | --- |
| Extract Blobs | Turns the mask into measured objects. Required by both trackers. |
| Get Blobs Angles | Adds a major-axis orientation to each blob. |
| Track Blobs | Links blobs across frames into numbered identities. |
| Pattern Tracker | Follows targets by appearance; survives detection drop-outs. |

## [Markers and detection](/plugins/markers/)

| Plugin | |
| --- | --- |
| Aruco | Standard ArUco markers: id, position, orientation. |
| Aruco Color | Colour-coded markers, so tags can be physically smaller. |
| Simple Tags | Older striped-tag scheme. Superseded by Aruco. |
| Yolo Detector | Runs an ONNX YOLO model you supply. |

## [Zones, recording and output](/plugins/output/)

| Plugin | |
| --- | --- |
| Zones of Interest | Polygon regions drawn on the video. |
| Record Video | H.264 recording of the original frames. |
| Record Pixels | Every masked pixel to a file. Very large output. |
| Take Snapshots | Mask images, numbered by frame. |
| Stopwatch | Manual event scoring with keyboard shortcuts. |
| Remote Control | Bluetooth input. Linux only, off by default, deprecated. |

## Common controls

Most detectors share:

- **Additive** — off is AND (intersection with the existing mask), on is OR.
- **Restrict to Zone** — process only one [zone](/guide/zones-background/) region.
- **Invert** — where present, flips that plugin's result.

And every plugin has **Active** (whether it runs) and **Output** (whether it
writes its file, once output is armed).

## A note on defaults

A few default values in the code differ from the numbers quoted in the in-app
help, which was written against an older set. The values on these pages are the
ones the code uses; the discrepancies are listed in
[known issues](/project/known-issues/).

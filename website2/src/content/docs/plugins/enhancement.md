---
title: Image enhancement
description: White Balance, Curves, CLAHE, Dehaze, Denoise, Temporal Denoise and Sharpen.
---

These modify the frame in place, so every detector downstream sees the improved
image. [Record Video](/plugins/output/) still writes the original.

They change detection results, not just appearance — that is the point of them,
but it means the choice belongs in a description of the analysis.

Judge them on the **mask**, with a detector active, rather than on the picture.
Enhancement often makes footage look better to a person without helping detection.

## White Balance

Removes colour casts.

| Mode | |
| --- | --- |
| Gray World (auto) | Assumes the scene averages to grey |
| Simple (auto) | Histogram stretch per channel |
| Manual / Pick | Explicit gains, usually set by clicking a should-be-neutral pixel |
| Underwater | Compensates the red channel, then a gray-world pass |

| Parameter | Default | |
| --- | --- | --- |
| Saturation Threshold | 0.9 | Gray World only. Ignores over-saturated pixels when estimating the cast. |
| Gain R / G / B | 1.0 | Manual mode, range 0.2–5.0 |
| Strength | 1.0 | Underwater mode, range 0–2 |

The Underwater mode is worth trying on any submerged footage: water absorbs red
within a couple of metres, and the automatic modes handle that badly because they
assume a uniform illuminant shift. Push Strength until the blue-green cast goes;
beyond that the image turns pink.

The automatic modes recompute from each frame's content, so the correction shifts
slightly as animals move through. Manual gains are fixed numbers and behave
identically on every frame — preferable when it matters.

## Curves

Editable transfer curve per channel — Master first, then Red, Green, Blue on top.
Covers levels, gamma, contrast and colour balance.

In the editor: click to add a point, drag to move, right-click to remove. A
straight diagonal is no change; an S-shape adds contrast; a steep segment over a
narrow input range puts all the contrast where your subject's brightness sits.

Cheap, fully deterministic, and easy to describe. A reasonable alternative to
CLAHE when lighting is even.

## Clahe

Contrast Limited Adaptive Histogram Equalisation — equalises local regions
separately, so a dark corner and a bright centre are each handled on their own
terms. Works on the lightness channel, so colours are not distorted.

| Parameter | Default | |
| --- | --- | --- |
| Clip Limit | 3.0 | Contrast strength. Higher amplifies noise. |
| Tile Size | 8 | Grid is Tile Size × Tile Size. Smaller adapts more locally. |

Useful for unevenly lit scenes. If raising Clip Limit makes the *background* more
likely to be detected, it has gone too far. Tiles much smaller than an animal can
equalise the animal against itself.

## Dehaze

Dark Channel Prior dehazing — removes the veiling scatter that flattens contrast
in turbid water, fog or dust.

| Parameter | Default | Range | |
| --- | --- | --- | --- |
| Strength (omega) | 0.95 | 0–1 | How much haze to remove |
| Patch Size | 15 | 3–41 | Local estimate window. Larger is smoother and slower. |
| Min Transmission | 0.1 | 0.01–0.5 | Floor on recovered transmission |

If the murkiest parts of the frame come out noisy or over-corrected, raise Min
Transmission to 0.15–0.2 — that is the symptom this parameter exists for.

Expensive. Pairs with Underwater white balance: white balance fixes the colour,
dehaze fixes the contrast. Correct colour first.

## Denoise

Single-frame noise reduction.

| Method | |
| --- | --- |
| Bilateral (fast) | Edge-preserving smoothing |
| NL-means (slow, cleaner) | Better on textured noise, CPU-heavy |

| Parameter | Default | Range | Method |
| --- | --- | --- | --- |
| Diameter | 5 | 1–25 | Bilateral |
| Sigma Color | 50 | 1–150 | Bilateral — the strength control |
| Sigma Space | 50 | 1–150 | Bilateral |
| Strength | 3.0 | 1–30 | NL-means |
| Color Strength | 3.0 | 1–30 | NL-means |

Often unnecessary. Adaptive background subtractors already model per-pixel
variation and largely ignore noise, and a short erosion afterwards removes what
survives. Deactivate it and compare the mask before keeping it — this is one of
the most expensive plugins in a typical pipeline.

## Temporal Denoise

Multi-frame NL-means: denoises the centre frame of a window using the frames on
both sides. Patches are matched rather than averaged, so moving objects are not
smeared.

| Parameter | Default | Range | |
| --- | --- | --- | --- |
| Window (frames) | 5 | 3–11, odd | Output is delayed by half the window |
| Strength | 3.0 | 1–30 | |
| Color Strength | 3.0 | 1–30 | |

Much stronger than single-frame denoising, and the most expensive plugin here —
expect a few frames per second. Worth it when animals are barely above the noise
floor and single-frame filtering destroys them along with the noise. Offline use
only.

## Sharpen

Unsharp mask.

| Parameter | Default | Range |
| --- | --- | --- |
| Amount | 1.0 | 0–4 |
| Radius | 2.0 | 0.5–10 |

Radius sets what is sharpened, Amount how much. Denoise first if you use both —
sharpening noise makes it permanent.

It helps detection less often than expected, since background subtractors compare
each pixel to a model of itself. It does reliably help **marker detection** on
slightly soft footage.

---
title: Blobs and tracking
description: Extract Blobs, Get Blobs Angles, Track Blobs and Pattern Tracker.
---

## Extract Blobs

Finds connected white regions in the mask, measures each, and produces the blob
list that the trackers consume. Required before either tracker.

| Parameter | Default | |
| --- | --- | --- |
| Min Size | 10 | Minimum area in pixels. The main noise filter. |
| Max Size | 0 | Maximum area; 0 means no limit |
| Output File | — | |

Set Min Size from a measurement rather than by guessing — the **Ruler** tool in
Rect mode gives an animal's dimensions, and the blob area is roughly half the
bounding box. Leave headroom for frames where only part of an animal is detected.

Min Size is in **mask** pixels, so at 1/2 downscale an area of 400 measures 100.
Output is always converted back to source resolution.

Leave Max Size at 0 unless you have a specific large false positive to reject.
Setting it tight is risky: two animals that touch merge into one blob of roughly
double area, and a tight Max Size would discard both rather than reporting a
merged detection.

Enabling output on both this and Track Blobs is a useful pairing — the blob file
says what was detected, the track file says how detections were assigned. Frames
with fewer blobs than animals are detection failures; the right blob count with
swapped identities is a tracking failure.

```python
b = pd.read_csv("blobs.csv", sep="\t"); b.columns = b.columns.str.strip()
print(b.groupby("frame").size().value_counts().sort_index())
```

## Get Blobs Angles

Adds a major-axis orientation to each blob. No parameters. Must come **after**
Extract Blobs — without it, the `blob_angle` column is not meaningful.

The measured axis is a line, not an arrow: an animal facing east and one facing
west give the same value, and it flips between the two equivalents as the blob
shifts. So:

- Do not compute turning rates from raw consecutive angles.
- Fold to 0–180° and unwrap before analysing.
- For true heading use the direction of travel between successive positions, or a
  [marker](/plugins/markers/), which has a defined orientation.

```python
a = np.deg2rad(df["blob_angle"] % 180.0) * 2.0
df["angle_unwrapped"] = np.rad2deg(np.unwrap(a) / 2.0)
```

For a round blob the axis is determined by noise. Aggressive erosion also
destroys the elongation this measures, so keep morphology gentle if you need
orientation.

## Track Blobs

Links blobs across frames to a fixed set of numbered entities, by proximity and
predicted motion.

| Parameter | Default | |
| --- | --- | --- |
| Max Entities | 1 | Number of animals. Fixed, not a maximum. |
| Min Interdistance | 5.0 | Minimum spacing, so two entities do not settle on one animal |
| Max Motion/sec | 800.0 | Largest plausible speed in px/s |
| Extrapolation Decay | 0.8 | How fast predicted motion decays when undetected |
| Motion Estimator Length | 10 | Frames of history used for velocity |
| Motion Estimator Timeout | 1.0 | Seconds before the estimate is discarded |
| Use Virtual Entities | off | Keep a predicted entity alive through a drop-out |
| Virtual Lifetime / Delay | 1.0 / 0.3 | Seconds |
| Trail Length | 10 | HUD only |
| Replay | off | Deprecated, see [known issues](/project/known-issues/) |

**Max Motion/sec** is the most useful control. Measure it: with the Ruler, see how
far the fastest animal moves in a second, and set it somewhat above.

**Min Interdistance** at roughly one body length prevents two entities converging
on the same animal, which produces two identical trajectories and quietly halves
your sample.

The output has one row per entity per frame, detected or not. **The `assigned`
column is 1 for measured positions and 0 for predicted ones** — filter on it
before computing anything, unless you have deliberately decided otherwise.

Identity swaps when animals cross are the characteristic failure, and no
proximity tracker prevents them reliably. In rough order of effectiveness:
markers; Pattern Tracker; separating touching blobs with erosion; tighter
Max Motion/sec and Min Interdistance; or measuring the swap rate on a few clips
and reporting it.

Watching a few minutes with the HUD on and Trail Length at 20–30 makes swaps
obvious.

## Pattern Tracker

Follows each target by appearance: it stores the image patch around the target
and searches a limited neighbourhood each frame for the best match. Because it
matches appearance rather than proximity, it keeps going when the detector loses
the animal.

| Backend | |
| --- | --- |
| Template match | Local normalised cross-correlation. All parameters exposed; template adapts only when confident. |
| CSRT | OpenCV's correlation-filter tracker. Better with scale and appearance change; fewer controls. |

**Seeding** — click on the video (tick *Click on video to add targets*), or
automatically from mask blobs (*Seed from detected blobs*, with the plugin placed
after Extract Blobs), or from YOLO detections. Automatic seeding is what you want
for unattended runs.

| Parameter | Default | |
| --- | --- | --- |
| Search Distance | 40 | How far a target may move between frames |
| Fit box to mask blob | on | Take the box from the blob instead of a fixed square |
| Size Adapt Rate | 0.20 | How fast the box follows the blob |
| Template / Fallback Size | 48 | Box side when no blob is available |
| Match Threshold | 0.40 | Below this correlation the target is lost (Template only) |
| Update Threshold | 0.60 | Adapt the template only above this (Template only) |
| Update Rate | 0.10 | Adaptation blend factor (Template only) |
| Predict position | on | Centre the search on the predicted position |
| Max Lost Frames | 15 | Drop a target after this many misses |
| Max Targets | 10 | |
| Trail Length | 30 | HUD only |
| Seed from detected blobs | off | |
| Min Blob Size | 50 | Minimum blob area to seed from |
| Seed from YOLO / Min Confidence | off / 0.5 | |
| Confirm after N detections | 1 | Candidate becomes live after N detections |
| Show candidates | on | Draw unconfirmed candidates in orange |
| Merge overlapping | on | Drop the weaker of two overlapping targets |
| Merge Overlap (IoU) | 0.40 | |
| Merge Delay (frames) | 0 | Overlap must persist this long before merging |

Measure **Search Distance** rather than guessing — too small and a fast animal
escapes the window; too large and the search can lock onto a neighbour, at a cost
growing with the square of the radius.

The `score` column in the output is the diagnostic: healthy tracking sits well
above Match Threshold, and a target hovering just above it is about to fail or is
already following the background.

Raising **Confirm after N detections** to 2 or 3 stops noise blobs becoming
targets when seeding automatically. Raising **Merge Delay** to 5–10 frames stops
two animals that pass close being collapsed into one.

Targets are also stamped into the mask, so Extract Blobs and Track Blobs placed
after this plugin consume them normally.

## Which tracker

| | Pattern Tracker | Track Blobs |
| --- | --- | --- |
| Follows | Appearance | Proximity and predicted motion |
| Survives detection drop-out | Yes | Only with virtual entities |
| Handles a resting animal | Yes | Only if still detected |
| Number of individuals | Varies as targets appear and disappear | Fixed at Max Entities |
| Output | One row per active target | One row per entity per frame |
| Cost | Moderate | Cheap |

Track Blobs gives a regular, gap-free table for a fixed cast of animals. Pattern
Tracker is better when detection is unreliable.

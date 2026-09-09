---
title: First analysis
description: A short walkthrough — detect moving animals, extract blobs, track them, and write a file.
---

This builds a basic tracker on your own footage: background subtraction, noise
cleanup, blob extraction, tracking, and a text file. It takes about ten minutes.

Use a clip where the animals are difficult to see rather than an easy one.
Parameters tuned on the clearest part of a dataset usually fail on the rest.

## Open a video

`File → Open Source…`, or on the command line:

```bash
./build/useTracker -i trial_004.mp4
```

<kbd>Space</kbd> plays and pauses. <kbd>←</kbd> and <kbd>→</kbd> step one frame.
Pause somewhere the animals are moving.

The **Downscale** slider at the bottom right runs the pipeline on a smaller
frame, which makes parameter changes respond immediately. Set it to 1/2 while
tuning. Output coordinates are still written at full resolution, but
pixel-valued parameters like blob size act on the smaller image, so re-check
them at 1:1 before a real run.

## Detect moving objects

In the right panel, **Processing** tab, under *Available Plugins* → *Background
Subtraction*, double-click **Background Diff Knn**. It appears in the pipeline as
`0: Background Diff Knn`, and the video now shows the mask: white is foreground.

Play a few seconds so the model learns the scene, then pause.

Double-click the plugin to open its settings. **Dist2 Threshold** is the main
control — lower it (try 100) if the animals are faint, raise it if there is too
much noise. The **?** button in the dialog shows help for each parameter.

Aim for animals appearing as solid white shapes. Ignore speckle noise for now.

The blend slider in the toolbar fades between the mask and the original frame,
which is the easiest way to check the mask is actually on the animals.

## Clean up the mask

From *Morphology*, add **Erosion**. It shrinks white regions, removing isolated
noise pixels. Raise **Size** one step at a time and watch your smallest animal —
too much erosion deletes small animals in every frame.

If the animals end up fragmented, add **Dilation** after it.

## Extract blobs

From *Blobs*, add **Extract Blobs**. This finds connected white regions and
measures each one.

**Min Size** (in pixels of area) discards anything smaller. The **Ruler**
checkbox in the toolbar lets you measure an animal on the video, so you can set
this from a real number.

In the same dialog, tick **Output** and set **Output File** to `blobs.csv`.

## Track

From *Tracking*, add **Track Blobs**. This links blobs between frames into
numbered entities.

- **Max Entities** — the number of animals present.
- **Max Motion/sec** — the fastest plausible speed in pixels per second.
- **Trail Length** — how much of the past track is drawn.

Tick **Output**, set **Output File** to `tracks.csv`.

## Record

Ticking **Output** on a plugin is not enough on its own. Press **REC** in the
toolbar (or <kbd>Ctrl</kbd>+<kbd>R</kbd>) to start writing, then play through.
Press REC again to stop.

Files are opened when you arm REC and closed when you disarm it or press Stop.
Re-arming overwrites. In [batch mode](/guide/batch/) this is automatic.

## Save the settings

`File → Save Settings…` writes an XML file with the plugin order, all
parameters, and any zone polygons. This is what you reuse for the rest of the
dataset:

```bash
./build/useTracker --nogui -p settings.xml -i trial_005.mp4
```

## Check the result

```python
import pandas as pd
df = pd.read_csv("tracks.csv", sep="\t")
df.columns = df.columns.str.strip()      # header cells are space-padded
df = df[df["assigned"] == 1]             # keep measured, not predicted, rows
print(df.groupby("entity")[["x", "y"]].describe())
```

Turning the HUD on and watching a few minutes at 2× is worth doing before
processing a whole dataset — identity swaps when animals cross are easy to see
and hard to notice in a file.

## Next

- [Pipelines](/guide/pipelines/) — how the plugins fit together
- [Zones and background](/guide/zones-background/) — restricting to a region
- [Batch processing](/guide/batch/) — running a dataset
- [Examples](/start/examples/) — the bundled settings files

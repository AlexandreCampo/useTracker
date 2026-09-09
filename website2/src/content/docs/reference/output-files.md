---
title: Output files
description: The columns written by each plugin, and how to load them.
---

All text output is tab-separated with one header line. Coordinates and areas are
in **source-video pixels**, converted back automatically if the pipeline ran
downscaled.

:::caution[Header cells are space-padded]
Column names arrive as `"time "`, `" frame "` and so on. Data rows are cleanly
tab-separated. Strip the names when loading, or `df["frame"]` raises a `KeyError`.
:::

## Extract Blobs

One row per accepted blob per frame. No identity — blob 1 in one frame is
unrelated to blob 1 in the next.

```
time 	 frame 	 blob_x 	 blob_y 	 blob_angle 	 blob_size 	 blob_zone
```

| Column | |
| --- | --- |
| `time` | Seconds from the start of the source |
| `frame` | Displayed frame number |
| `blob_x`, `blob_y` | Centroid |
| `blob_angle` | Major-axis orientation — only meaningful with Get Blobs Angles in the pipeline, and ambiguous by 180° |
| `blob_size` | Area in pixels |
| `blob_zone` | Zone region |

## Track Blobs

One row per entity per frame, **detected or not**.

```
time 	 frame 	 lastFrameDetected 	 lastFrameNotDetected 	 entity 	 assigned 	 zone 	 size 	 x 	 y
```

| Column | |
| --- | --- |
| `entity` | Identity, 0 … Max Entities − 1 |
| `assigned` | **1 = measured, 0 = predicted position** |
| `lastFrameDetected` | Frame of the most recent real detection |
| `zone`, `size`, `x`, `y` | Region, area, position |

Filter on `assigned` before computing anything. Predicted rows invent motion
during exactly the periods when tracking was struggling.

```python
df = df[df["assigned"] == 1]
```

## Pattern Tracker

One row per active target per frame. Targets appear and disappear, so ids are not
contiguous.

```
time 	 frame 	 target_id 	 x 	 y 	 width 	 height 	 score
```

`score` is the match confidence — a sustained low value means the target is
failing or already following the background.

## Yolo Detector

```
time 	 frame 	 class_id 	 class_name 	 confidence 	 x 	 y 	 width 	 height
```

## Aruco, Aruco Color, Simple Tags

```
time 	 frame 	 id 	 x 	 y 	 angle 	 size 	 zone
```

`id` is a real identity, stable across the recording, so this usually needs no
tracking step. `angle` is unambiguous over 360°, unlike a blob's principal axis.

## Record Pixels

One row per masked pixel per frame — see the size warning on
[zones, recording and output](/plugins/output/).

```
time 	 frame 	 zone 	 x 	 y
```

## Stopwatch

```
time 	 event 	 type
```

`type` is `start` or `end`.

## Loading

```python
import pandas as pd

def load(path):
    df = pd.read_csv(path, sep="\t")
    df.columns = df.columns.str.strip()
    for c in df.select_dtypes("object"):
        df[c] = df[c].str.strip()
    return df
```

```r
library(readr)
tracks <- read_tsv("tracks.csv", trim_ws = TRUE)
```

## Speed

```python
import numpy as np

df = df.sort_values(["entity", "frame"])
g = df.groupby("entity")
df["dx"], df["dy"], df["dt"] = g["x"].diff(), g["y"].diff(), g["time"].diff()

# drop differences that span a detection gap
df.loc[g["frame"].diff() != 1, ["dx", "dy"]] = np.nan

df["speed"] = np.hypot(df.dx, df.dy) / df.dt        # px/s
```

The gap handling matters: if an entity was undetected for ten frames, the
difference across that gap is not a per-frame displacement, and treating it as one
produces a spurious spike.

For real units, measure a known distance with the Ruler tool and divide.

## Checking a run

The row count should be roughly predictable. A 10-minute 25 fps recording with 5
entities in Track Blobs gives 600 × 25 × 5 = 75 000 rows. One line means output
was never written; a very short file means little was detected.

```python
per_frame = tracks.groupby("frame")["assigned"].sum()
print(per_frame.mean(), (per_frame == 5).mean())
```

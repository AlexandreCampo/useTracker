---
title: Batch processing
description: Running a saved pipeline over a set of recordings without the interface.
---

The same binary runs headless with `--nogui`, driven by a settings file saved
from the interface.

```bash
useTracker --nogui -p settings.xml -i trial_004.mp4
```

Output is armed automatically in this mode, so the **Output** ticks saved in the
settings file are all that is needed — there is no REC step.

Progress goes to standard output:

```
Progress : 12% | frame 4321/36000 | time 172.8s
```

## Before a whole dataset

Run one video, check the file has roughly the number of rows it should, and time
it. A 10-minute 25 fps recording with 5 tracked entities gives 600 × 25 × 5 =
75 000 rows. A file with one line means output was never written; a very short
one means almost nothing was detected.

Multiply the runtime by the number of files before starting.

## Output filenames collide

Output paths come from the settings file, so every video writes to the same name
and only the last survives. Two ways round it.

**A directory per video** — relative paths in the settings file resolve against
the working directory:

```bash
SETTINGS="$PWD/settings.xml"
for f in videos/*.mp4; do
  name=$(basename "$f" .mp4)
  mkdir -p "results/$name"
  ( cd "results/$name" && useTracker --nogui -p "$SETTINGS" -i "$PWD/../../$f" )
done
```

**Rewrite the filename** into a per-video copy of the settings file. Note that
OpenCV stores strings double-quoted inside the tags:

```bash
for f in videos/*.mp4; do
  name=$(basename "$f" .mp4)
  sed "s|<OutputFilename>\"tracks.csv\"</OutputFilename>|<OutputFilename>\"out/${name}_tracks.csv\"</OutputFilename>|" \
      settings.xml > "tmp/${name}.xml"
  useTracker --nogui -p "tmp/${name}.xml" -i "$f"
done
```

## In parallel

The pipeline is multithreaded within one video, but running several processes
usually still helps:

```bash
ls videos/*.mp4 | xargs -P4 -I{} useTracker --nogui -p settings.xml -i {}
```

Start around half the core count and watch memory — each process holds its own
frame buffer.

## Catching failures

`useTracker` exits non-zero if the source cannot be opened.

```bash
for f in videos/*.mp4; do
  name=$(basename "$f" .mp4)
  if ! useTracker --nogui -p settings.xml -i "$f" > "logs/$name.log" 2>&1; then
    echo "$f" >> failures.txt
  fi
done
```

Then check the outputs are plausible, not merely present:

```bash
for d in results/*/; do
  n=$(wc -l < "$d/tracks.csv" 2>/dev/null || echo 0)
  [ "$n" -lt 1000 ] && echo "suspicious: $d ($n rows)"
done
```

## Combining results

```python
from pathlib import Path
import pandas as pd

frames = []
for p in sorted(Path("results").glob("*/tracks.csv")):
    df = pd.read_csv(p, sep="\t")
    df.columns = df.columns.str.strip()
    df["video"] = p.parent.name
    frames.append(df)

all_tracks = pd.concat(frames, ignore_index=True)
```

A quick per-video quality check is worth running before analysis. Sorting by
detection rate usually surfaces the recordings where something was physically
different — a light left on, the camera moved, cloudier water:

```python
q = all_tracks.groupby("video").agg(
    frames=("frame", "nunique"),
    detection_rate=("assigned", "mean"),
)
print(q.sort_values("detection_rate"))
```

## Other options

`--start` and `--length` restrict the time window; `--scale` downscales frames.
See the [command line reference](/reference/cli/).

## Real-time use

With a USB camera source (`-u 0`), the same pipeline runs live. This is how
useTracker has been used to drive electronic actuators from what the camera sees.
Keep the pipeline cheap — CPU neural-network inference and NL-means denoising
will not keep up with 25 fps.

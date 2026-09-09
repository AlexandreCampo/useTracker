---
title: Zones, recording and output
description: Zones of Interest, Record Video, Record Pixels, Take Snapshots, Stopwatch and Remote Control.
---

## Zones of Interest

Defines which parts of the frame are processed, and labels detections by region.
Polygons are drawn directly on the video, and/or loaded from a grayscale zone
image.

| Control | |
| --- | --- |
| Selected polygon region | Region number of the selected polygon |
| Delete Selected / Clear All | |
| Save ROIs… / Load ROIs… | Store polygons in a text file for reuse |
| Zone image | Load a grayscale image whose values are region numbers |

Polygons are embedded in the settings file, so a configuration is self-contained.
Place this plugin first, before any detector.

Drawing and use are covered in [zones and background](/guide/zones-background/).

## Record Video

Encodes to H.264 with FFmpeg.

| Parameter | Default | |
| --- | --- | --- |
| Output File | — | `.mp4`, `.mkv` or `.avi` |
| Preset | `medium` | x264 preset; use `veryfast` for live capture |
| Bitrate (kbps) | 250 | See below |

It records the **original frame**, taken before any plugin runs — enhancement is
not applied and the HUD is not burned in. There is no option to record the
processed view.

:::caution[The default bitrate is very low]
250 kbps produces heavy artifacts at any normal resolution. That matters beyond
appearance: if the recording is later analysed, those artifacts are detected as
movement and can smear small markers beyond recognition.

Reasonable starting points: 1000–2000 kbps at 640×480, 3000–5000 at 720p,
8000–12000 at 1080p, 25000–40000 at 4K.
:::

Disarm REC before quitting — the stream needs its trailer written, and killing
the process mid-recording can leave a file some players will not open.

## Record Pixels

Writes the coordinates of every masked pixel on every frame.

:::danger[Output size]
One line per foreground pixel per frame. 2000 foreground pixels at 25 fps for ten
minutes is 30 million lines, roughly 750 MB. A full recording can reach hundreds
of gigabytes.

Calculate the number before enabling it, and restrict with a zone, a short time
window and a **Timestep**.
:::

Its legitimate use is spatial density — occupancy heatmaps, space use, posture
distributions — where a centroid is not enough. For anything a centroid answers,
use [Extract Blobs](/plugins/tracking/) instead.

A workable configuration uses a zone covering only the arena, Timestep 0.5 s, and
1/2 downscale. For a density map that loses essentially nothing, since consecutive
frames are highly redundant.

```python
h, xe, ye = np.histogram2d(df["x"], df["y"], bins=[240, 135])
```

For files too large to load, accumulate in chunks — passing an explicit `range` so
every chunk uses the same bin edges.

## Take Snapshots

Saves mask images while output is armed, with the frame number appended to the
pattern you give.

| Parameter | |
| --- | --- |
| Output Dir/Pattern | Base path; create the directory first |

One image per frame is 90 000 files an hour at 25 fps, so pair it with a
**Timestep** — `Timestep 10.0` gives one snapshot every ten seconds.

Useful for documenting what a pipeline produced, checking where a long run went
wrong, or building a hand-scored validation set. Moving the plugin up and down the
pipeline and taking a snapshot at each position shows the analysis stage by stage.

## Stopwatch

Logs timed events triggered by keyboard shortcuts you define. Purely manual — its
value is that hand-scored events land in the same timebase as everything measured
automatically.

| Parameter | |
| --- | --- |
| Output File | Destination |

Shortcuts are a key, a name, and whether the event is instantaneous or has a
duration (press once to start, again to end). They are saved in the settings file,
so a scoring scheme is reusable.

Behaviour categories — aggression, courtship, feeding, vigilance — are
interpretations, not pixel measurements, and no amount of parameter tuning
produces them. Scoring at 1/4 playback speed is far more accurate than at full
speed, and the loop range makes re-scoring a difficult passage easy.

## Remote Control

Bluetooth RFCOMM input with a small on-screen indicator. Requires the
`USE_BLUETOOTH` build option, which is **off by default**, and is Linux-only.

Deprecated — see [known issues](/project/known-issues/). Any Bluetooth or USB
device that presents itself as a keyboard (a presentation clicker, foot pedal, or
macro pad) already drives the Stopwatch shortcuts on every platform with no code
at all.

---
title: Settings file
description: The structure of the XML file that stores a pipeline.
---

`File → Save Settings…` writes one XML file containing the plugin order, every
parameter, and any zone polygons. It drives both the interface and `--nogui`.

It is an OpenCV `FileStorage` document, so **strings are double-quoted inside the
tags**.

```xml
<?xml version="1.0"?>
<opencv_storage>
<Configuration>
  <StartTime>0.</StartTime>
  <DurationTime>0.</DurationTime>
  <Timestep>0.</Timestep>
  <UseTimeBounds>0</UseTimeBounds>
  <BackgroundFilename>"background.png"</BackgroundFilename>
  <BackgroundRecalculate>0</BackgroundRecalculate>
  <BackgroundFrames>11</BackgroundFrames>
  <BackgroundStartTime>0.</BackgroundStartTime>
  <BackgroundEndTime>0.</BackgroundEndTime>
  <BackgroundLowThreshold>0</BackgroundLowThreshold>
  <BackgroundHighThreshold>255</BackgroundHighThreshold>
  <BackgroundCalcType>median</BackgroundCalcType>
  <ZonesFilename>""</ZonesFilename>
  <Pipeline>
    <Plugin_0>
      <BackgroundDiffKnn>
        <Active>1</Active>
        <History>500</History>
        <Threshold>400.</Threshold>
        <ShadowDetection>0</ShadowDetection>
        <RestrictToZone>0</RestrictToZone>
        <Additive>0</Additive></BackgroundDiffKnn></Plugin_0>
    <Plugin_1>
      <ExtractBlobs>
        <Active>1</Active>
        <Output>1</Output>
        <MinSize>120</MinSize>
        <MaxSize>0</MaxSize>
        <OutputFilename>"blobs.csv"</OutputFilename>
        <RecordLabels>0</RecordLabels></ExtractBlobs></Plugin_1>
  </Pipeline>
</Configuration>
</opencv_storage>
```

## Structure

Keys outside `<Pipeline>` are the engine settings — the Processing Frame and
Background tabs.

Inside, one `<Plugin_N>` wrapper per stage, numbered in execution order, each
holding a single element named after the plugin's registry name. The wrapper works
around duplicate-key handling in OpenCV's XML reader; keep the numbering
sequential from 0 if you edit by hand.

Common plugin keys: `Active`, `Output`, `OutputFilename`, `Additive`,
`RestrictToZone`, `Zone`.

## Registry names

Mostly match the interface labels. The ones that differ:

| Interface | XML element |
| --- | --- |
| Background Difference | `BackgroundDifference` |
| Background Diff Knn / Gsoc / Mog / Mog2 / Gmg | `BackgroundDiffKnn`, `BackgroundDiffGsoc`, `BackgroundDiffMog`, `BackgroundDiffMog2`, `BackgroundDiffGmg` |
| Track Blobs | `TrackBlobs` |

The rest are the label without spaces: `ExtractBlobs`, `PatternTracker`,
`YoloDetector`, `ZonesOfInterest`, `WhiteBalance`, `TemporalDenoise`,
`AdaptiveThreshold`, `ColorSegmentation`, `MovingAverage`, `RecordVideo`,
`RecordPixels`, `TakeSnapshots`, `GetBlobsAngles`, `SimpleTags`, `ArucoColor`,
and so on.

## Thread count

One key is read but never written by `Save Settings`:

```xml
<Threads>4</Threads>
```

Without it, the engine uses the machine's core count, which slightly affects
neighbourhood operations at slice boundaries. Add it by hand if you need
identical results across machines — and re-add it after each save. See
[known issues](/project/known-issues/).

## Zone polygons

Zones of Interest embeds its polygons in its own element, so a settings file
carries the regions as well as the parameters.

## Editing by hand

The file is plain text, which makes some batch patterns possible.

```bash
# per-video output names
sed "s|<OutputFilename>\"tracks.csv\"</OutputFilename>|<OutputFilename>\"${name}_tracks.csv\"</OutputFilename>|" \
    settings.xml > "${name}.xml"

# a parameter sweep
for t in 100 200 400 800; do
  sed "s|<Threshold>400.</Threshold>|<Threshold>${t}.</Threshold>|" \
      settings.xml > "thr_${t}.xml"
done
```

Keep `Plugin_N` numbering sequential, keep strings quoted, and load the result in
the interface once before running a batch on it.

Being text, it also diffs — keeping it in version control alongside analysis
scripts gives a dated record of when a parameter changed.

## Known issue

`BackgroundStartTime` and `BackgroundEndTime` are written as floats but read back
as integers, so `12.5` reloads as `12`.

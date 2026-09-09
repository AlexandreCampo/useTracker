---
title: Troubleshooting
description: Common symptoms and what causes them.
---

Selecting each plugin in the pipeline in turn, and watching where the video stops
looking right, identifies the responsible stage in seconds. Most of what follows
is faster with that habit.

## Nothing is detected

- **Adaptive model not warmed up.** Play a few seconds first.
- **Plugin not active**, or a zone excluding everything — deactivate the Zones
  plugin to check.
- **Threshold too strict.** Lower Dist2 Threshold (KNN) or Threshold (Background
  Difference).
- **Two detectors ANDing each other away.** With Additive off they intersect; if
  they disagree the result is empty.
- **Min Size too high** in Extract Blobs. Set it to 1 temporarily.

## Everything is detected

- **Static background invalid** — recompute it, or the lighting has changed since.
- **Camera moved.** Everything after that point is wrong; split the video.
- **Threshold too low**, or the model has not warmed up.

## Animals vanish when they stop moving

Adaptive models absorb motionless objects. Lower the learning rate or Replace
Rate, use a static background, add Pattern Tracker after the subtractor, or use
markers. See [background subtraction](/plugins/background/).

## Identities keep swapping

Markers are the only robust fix. Otherwise: lower Max Motion/sec, raise Min
Interdistance, separate touching blobs with erosion then dilation, or use Pattern
Tracker. Watch with the HUD on and a long trail to see how often it happens.

## One animal becomes two blobs

Dilation then Erosion merges fragments. Or lower the detector threshold, or add
enhancement so the whole animal separates from the substrate.

## Two animals become one blob

Erosion then Dilation can break a thin connection. If they genuinely overlap, no
morphology helps — use Pattern Tracker with a merge delay, or markers. Do not use
Max Size to reject merged blobs; that discards both animals.

## No output file

All three are required: plugin **Active**, plugin **Output** ticked with a
filename, and output armed with **REC**. Also check the working directory — a bare
`tracks.csv` lands where you launched the program, not next to the video. Pressing
Stop closes output, and re-arming overwrites.

With `--nogui`, arming is automatic.

## The output has one line

Only the header. Either nothing was detected, or the run ended immediately — check
the source opened, and that `--start` is not past the end of the video.

## Playback is very slow

Use the Downscale slider while tuning. Add zones. Then find the expensive plugin:
Temporal Denoise, NL-means Denoise, YOLO on CPU and Dehaze dominate everything
else — untick **Active** one at a time.

Also check the build used `-DCMAKE_BUILD_TYPE=Release`.

## Stepping backwards is slow

Expected. Video frames depend on earlier frames, so stepping back beyond the
buffer requires seeking to a keyframe and decoding forward.

## Blob sizes changed after an upstream change

Expected — enhancement changes the frame, which changes the mask, which changes
blob areas. If it changed after moving the Downscale slider, that is also
expected: Min Size and morphology sizes act on the smaller mask, while output
stays at source resolution.

## Colours look wrong

An enhancement plugin is doing its job. Record Video still writes the original. If
an *automatic* white balance mode flickers between frames, switch to Manual —
frame-to-frame variation there looks like scene change to a background model.

## ArUco markers not detected

Usually physical: too small (aim for 30–40 px across), motion blur, glare from
glossy printing, a curved tag, or the wrong size range (Min/Max Size are fractions
of the image). Try Sharpen before the detector, and adjust Threshold 1 / 2 for
blurry footage.

## YOLO says no model loaded

Check the path is still valid, that the file is ONNX, and press **Reload Model**.
If a GPU target was selected, check which target the dialog reports — it falls
back to CPU silently.

## Build fails at find_package(OpenCV)

The named component is missing from your OpenCV, not from useTracker. The build
needs the contrib modules `bgsegm`, `xphoto`, `tracking` and `dnn`. See
[install](/start/install/).

## Escape does not quit

Keyboard shortcuts are only handled with a source loaded. Use the menu.

## Still stuck

Open an issue at
[github.com/AlexandreCampo/useTracker/issues](https://github.com/AlexandreCampo/useTracker/issues)
with your settings file attached.

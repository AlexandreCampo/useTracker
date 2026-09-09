---
title: Background subtraction
description: The six background models, plus Frame Difference and Moving Average — what they do and which to choose.
---

Background subtraction is usually the first detector in a pipeline. The choice of
model matters more than fine tuning.

**Start with Background Diff Knn.** If the footage is noisy or low-contrast, try
GSOC. If the scene is genuinely stable and you can get a clean empty-arena image,
Background Difference is the most predictable.

## Static or adaptive

| | Static (Background Difference) | Adaptive (KNN, GSOC, MOG, MOG2, GMG) |
| --- | --- | --- |
| Reference | A fixed image you provide | Learned continuously, per pixel |
| Warm-up | None | Needed |
| A motionless animal | Stays detected | Gradually absorbed into the background |
| Lighting drift, moving plants | Invalidates it | Handled |
| Describing it | Trivial | Depends on parameters and history |

Adaptive models are frozen while playback is paused — they learn only from real
advancing frames, so tuning a slider on one frame does not corrupt the model.

## Comparison

| Model | Speed | Noise robustness | Main parameter |
| --- | --- | --- | --- |
| KNN | Fast | Good | Dist2 Threshold (400) |
| GSOC | Slow | Best | Replace Rate (0.003) |
| MOG2 | Fast | Moderate | Threshold, Shadow Detection |
| MOG | Fast | Moderate | Num Mixtures (5) |
| GMG | Slow | Moderate | Learning Rate (0.05) |

MOG and GMG are kept mainly for compatibility with older configurations.

## Background Difference

Subtracts a fixed background image and keeps pixels differing by more than a
threshold. Registered in settings files as `BackgroundDifference`.

| Parameter | Default | |
| --- | --- | --- |
| Threshold | 20 | Minimum brightness difference counted as foreground |
| Additive, Restrict to Zone, Zone | — | as elsewhere |

Prepare the background image on the Background tab — see
[zones and background](/guide/zones-background/).

The in-app help quotes a default of 42; the code uses 20.

## Background Diff KNN

Keeps recent observations per pixel and asks whether the current value is close
to enough of them.

| Parameter | Default | |
| --- | --- | --- |
| History | 500 | Frames remembered. Higher is more stable, slower to adapt. |
| Dist2 Threshold | 400.0 | **The main control.** Squared colour distance — 400 is a distance of 20, 100 is 10. |
| Shadow Detection | off | Classify shadows separately |
| Learning Rate | −1.0 | −1 derives it from History |

If animals are faint, drop Dist2 Threshold to around 100 before adjusting
anything else. At 25 fps, History 500 is 20 seconds — animals resting longer than
that start being absorbed.

## Background Diff GSOC

Keeps a set of background samples per pixel; a pixel matching too few is
foreground. Samples are replaced at random over time and propagate to neighbours.

| Parameter | Default | Range | |
| --- | --- | --- | --- |
| Num Samples | 20 | 2–1023 | Samples stored per pixel |
| Replace Rate | 0.003 | 0–1 | **Main control.** Lower it if resting animals are absorbed. |
| Propagation Rate | 0.01 | 0–1 | Spreads background samples to neighbours. Lower it if small animals disappear. |
| Hits Threshold | 32 | | Matches over time before a sample is trusted. Independent of Num Samples. |

The best of the set on turbid, noisy or low-contrast footage, at a clear cost in
speed. Swapping it in for KNN changes nothing downstream.

## Background Diff MOG2 and MOG

Mixture of Gaussians — each pixel modelled as several Gaussians, which handles a
background with more than one normal state (ripple, swaying foliage).

**MOG2**

| Parameter | Default | |
| --- | --- | --- |
| History | 200 | |
| Threshold | 0.0 | Squared Mahalanobis distance. See note below. |
| Shadow Detection | off | The reason to choose MOG2 |
| Learning Rate | 0.05 | −1 auto-tunes |

The code initialises Threshold to `0.0` while the help quotes 16, which is
OpenCV's own default and a sensible value. Set it explicitly to 16 and tune from
there.

Shadow detection matters more than it sounds: an untreated shadow joins the
animal's blob and pulls the centroid toward it, varying with the light's
direction.

**MOG**

| Parameter | Default | |
| --- | --- | --- |
| History | 200 | |
| Num Mixtures | 5 | 1–8. More handles multi-modal backgrounds. |
| Background Ratio | 0.7 | Weight fraction counted as background |
| Noise Sigma | 15.0 | Expected image noise |
| Learning Rate | 0.05 | |

## Background Diff GMG

Bayesian per-pixel model. Only exposes **Learning Rate** (0.05). Needs a long
warm-up — the first stretch of output is unusable rather than merely imperfect.
Rarely the best choice today.

## Frame Difference

Compares consecutive frames and marks what changed.

| Parameter | Default | |
| --- | --- | --- |
| Threshold | 1 | Minimum change. The default is very sensitive; 10–25 is more realistic on compressed footage. |
| Use Pipeline | on | Difference the running mask instead of the raw frame |
| Invert, Additive, Restrict to Zone | | |

This detects **motion, not objects**: an animal that stops disappears entirely,
and a uniformly coloured animal often produces a hollow outline. Poor as a
primary detector for animals that pause; useful combined with another detector
using Additive off, so you keep only things that are both the right colour and
currently moving.

## Moving Average

Averages the mask over the last *N* frames and keeps pixels active often enough.
Suppresses flickering detections.

| Parameter | Default | |
| --- | --- | --- |
| Length | 10 | Frames averaged |
| Threshold | 5 | How many of those a pixel must be active in |
| Centered (no time lag) | off | See below |
| Clear History | button | Reset after changing detector parameters |

A plain trailing average lags moving objects by about half the window, which is a
real positional error. **Centered** compensates by displaying the frame Length/2
behind the pipeline, at the cost of that much playback latency. If you use this
plugin, use Centered.

Not suitable for fast-moving targets, which are never in one place long enough to
survive the average.

## Tuning any of them

1. Play a few seconds so adaptive models warm up.
2. Select the plugin so the video shows its mask.
3. Move the main threshold until the animals are solid. Accept noise at this
   stage.
4. Use the blend slider to confirm the mask is on the animals, including
   extremities.
5. Remove the noise with [erosion](/plugins/segmentation/) and blob **Min Size**,
   not by tightening the threshold — tightening it costs you the faintest animals.

## When animals stop moving

The characteristic failure of adaptive models:

1. Lower the learning rate (KNN) or Replace Rate (GSOC), or raise History.
2. Use Background Difference with a static image.
3. Add [Pattern Tracker](/plugins/tracking/) after the subtractor — it follows
   appearance and survives detection drop-outs.
4. Use [markers](/plugins/markers/).

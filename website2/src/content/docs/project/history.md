---
title: History and credits
description: Where useTracker came from, who has used it, and how to refer to it.
---

useTracker was written by **Alexandre Campo** at the Unit of Social Ecology (USE)
of the Université Libre de Bruxelles, starting in 2015. The lab has since been
reorganised and is now the BASS lab. The name is both the lab's acronym and an
acronym of *Universal Simultaneous Event Tracker*.

## Why it was written

At the time, the options for video analysis in ethology were EthoVision, which
was expensive, and [SwissTrack](https://en.wikibooks.org/wiki/Swistrack), which
was no longer maintained. Neither fitted, so useTracker was written for the lab's
own experiments.

The pipeline-of-plugins design is taken directly from SwissTrack. A node graph
with explicit inputs and outputs, closer to a modular synthesiser, would be more
expressive — but also a step up in complexity, and a linear sequence fits the way
frames are processed well enough.

It was written before deep learning was practical for this kind of work: there
was little GPU capacity, no established culture of training sets, and limited CPU.
The classical image-processing approach reflects that period, and remains useful
because it needs no training data and runs on any machine.

## Who has used it

Around thirty people, mostly master's and PhD students in the lab, along with
some interest from outside via GitHub. It has been used on **ants, cockroaches,
fish, tadpoles and robots**, with and without ArUco tags, and tried on mice with
less success — detection depends on contrast against the background, which
furred animals on bedding do not provide.

The work was mostly on animal groups, since the lab studies collective behaviour:
trajectories, detecting individuals, and density measurements. It contributed to
several theses and publications.

The colour-marker library was developed to fit more identities into smaller tags,
and was used with 0.7 mm tags glued to ants' backs, printed on an ordinary inkjet
printer. That result was never published.

## Since then

Development was heaviest between 2015 and 2018, then largely paused. In 2026 the
codebase was modernised — CMake, SDL2 and Dear ImGui in place of wxWidgets,
OpenCV 4, FFmpeg 5 — and a set of plugins was added for poor-visibility
underwater footage, prompted by work on assessing biodiversity from fish
recordings.

Newer tools such as idtracker.ai and other learning-based trackers do some things
much better, particularly maintaining identity in large groups. They also tend to
be more specialised, and several are no longer maintained. useTracker is not
best-in-class at any one task; it is a general-purpose multitool that has
accumulated a lot of small adaptations to real experiments.

## Status

Maintained on a best-effort basis by one person alongside a full-time job. Small
fixes and features get done when time allows. There is no support commitment, and
no undertaking to solve problems specific to your setup.

Licensed under the **GNU General Public License v3**.

## Citing it

There is no paper for useTracker yet. One may be written; for now, cite the
software directly with the version or commit you used.

```bibtex
@software{usetracker,
  author  = {Campo, Alexandre},
  title   = {useTracker},
  url     = {https://github.com/AlexandreCampo/useTracker},
  note    = {GNU General Public License v3}
}
```

If your analysis depends on a specific algorithm, cite that algorithm too — most
of the image processing is **OpenCV's**, and the methods behind the background
subtractors (Zivkovic; KaewTraKulPong & Bowden; Godbehere et al.), dehazing (He et
al.), non-local means (Buades et al.), CLAHE (Zuiderveld), ArUco (Garrido-Jurado
et al.) and CSRT (Lukežič et al.) each have their own references.

Attaching the settings file used for an analysis is more informative than any
prose description of it.

## Acknowledgements

SwissTrack, for the design; OpenCV, FFmpeg, SDL2, Dear ImGui, CLI11 and
portable-file-dialogs, which do most of the actual work; and the students whose
experiments drove nearly every feature in it.

If you publish work using useTracker, mentioning it in an issue is welcome — it
helps show the tool is useful, and gives other people a worked example in their
own field.

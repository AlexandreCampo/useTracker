---
title: Contributing
description: Reporting problems, improving the documentation, and writing plugins.
---

useTracker is GPL v3 and maintained in spare time. Contributions are welcome;
the most useful ones are probably not code.

Repository:
[github.com/AlexandreCampo/useTracker](https://github.com/AlexandreCampo/useTracker)

## Most wanted

**Examples and tutorials.** The main barrier to using useTracker is knowing what
the operations do and what parameter ranges are useful. A commented settings file
for a study system not yet covered, or a short walkthrough for a particular kind
of footage, helps more than most code changes.

**Testing on Windows and macOS.** The build is written for them but rarely
exercised. A report either way is valuable.

**Parameter guidance from real experience.** "For *Daphnia* in a 6 cm dish, these
settings work" is knowledge that is otherwise stuck in one lab.

## Reporting a problem

Attach the settings file — it describes the entire pipeline in one attachment.
Also useful: the useTracker and OpenCV versions, your platform, what you expected,
what happened, and a short clip if it can be shared.

## Documentation

This site is in `website/` and every page has an **Edit page** link at the bottom.

```bash
cd website
npm install
npm run dev
```

Pages are Markdown under `src/content/docs/`; the sidebar is defined in
`astro.config.mjs`.

Corrections matter most — something wrong here is worse than something missing.

## Code

The [known issues](/project/known-issues/) page doubles as a work list, and
several entries are small and self-contained.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

| Path | |
| --- | --- |
| `App.cpp` | Entry point, plugin registry, headless loop |
| `AppGui.cpp` | The interface |
| `ImageProcessingEngine.*` | Frame buffer, threading, pipeline execution |
| `Pipeline.*` | Shared buffers and per-thread slices |
| `PipelinePlugin.h` | The plugin interface |
| `plugins/` | One pair of files per plugin |
| `capture/` | Video, image, USB and multi-source capture |
| `examples/` | Commented settings files |

## Writing a plugin

A plugin implements `Apply()`, plus `LoadXML`/`SaveXML` if it has parameters.

```cpp
class MyPlugin : public PipelinePlugin
{
public:
    int threshold = 10;

    MyPlugin() { registryName = "MyPlugin"; multithreaded = true; }

    void Apply() override;
    void Reset() override;                  // (re)allocate on size change
    void LoadXML(cv::FileNode& fn) override;
    void SaveXML(cv::FileStorage& fs) override;
    void OutputHud(cv::Mat& hud) override;  // optional overlay
};
```

Inside `Apply()` you have `pipeline->frame`, `pipeline->marked`,
`pipeline->zoneMap` and `pipeline->parent->blobs`.

Register it in `App.cpp` and add the source file to `CORE_SOURCES` in
`CMakeLists.txt`:

```cpp
NewPipelinePluginVector["MyPlugin"] = &CreatePipelinePluginVector<MyPlugin>;
```

Things worth getting right:

- **`multithreaded`** — only `true` if the plugin works correctly on a horizontal
  slice in isolation. Anything needing the whole frame must leave it `false`.
- **Honour `Additive` and `Restrict to Zone`** if the plugin writes the mask;
  users expect them on every detector. `ColorSegmentation.cpp` shows the pattern.
- **Save every parameter.** One missing from `SaveXML` silently resets on reload.
- **Add it to a category** in the `categories` list in `AppGui.cpp`, and write a
  `PluginHelpText` entry. Uncategorised plugins appear under *Other*.
- **Do not modify the frame** unless it is an enhancement plugin.
- **Document it** — add it to the relevant page under
  `website/src/content/docs/plugins/`.

## Pull requests

One change per pull request, matching the surrounding style, and say what you
tested it on. Update the docs in the same request if behaviour changes.

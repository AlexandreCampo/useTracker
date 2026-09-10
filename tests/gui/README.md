The Darkroom smoke test drives actual mouse and keyboard controls. It covers
plugin search, adding and removing stages, both kinds of reordering, editing a
parameter, help, saving/reloading XML, output toggling, playback, stepping,
zoom, all four inspector tabs, replacing the source, and window/UI scaling.
Screenshots cover the working and empty layouts. The first-frame check catches
the previous black preview before playback started.

From the repository root, with Python 3, FFmpeg and a built executable:

```sh
python tests/gui/run_darkroom.py build-redesign/useTracker --output /tmp/usetracker-gui
```

On Linux the default uses SDL's offscreen OpenGL driver. Use `--onscreen` to
exercise a real desktop window. Fixtures and plugin output files are temporary;
logs, screenshots and the saved analysis XML remain in the output directory.
Tests do not read or modify the user's `imgui.ini`.

The existing `--test` script format now also accepts `resize W H`, `ui-scale N`,
`text ...`, `key ctrl LETTER`, and `expect PROPERTY VALUE`. Expectations return
a nonzero process status on failure. `move` waits one frame so hover-dependent
controls (including ImGui tabs) respond consistently to the following click.

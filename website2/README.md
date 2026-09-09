# useTracker — website 2

An independent redesign of the project website. The original `../website` is
untouched. This version runs on a different port so both can be compared.

```bash
cd website2
npm ci
npm run dev
```

Open **http://localhost:4322**. The original website defaults to port 4321.
The dependencies match the original site: Astro 5, Starlight 0.37, and self-hosted
fonts. Use a Node version supported by the `engines` field in `package.json` and
a compatible npm release.

```bash
npm run build         # static output in dist/
npm run check:links   # check built internal links
npm run preview      # preview the production build on port 4322
```

## What changed

- A custom homepage with five switchable visual identities, plus independent
  compact/spacious density controls; responsive layouts down to small phones.
- An interactive canvas illustration with trajectory, detection, and zone views,
  a pause/play control, and a seekable timeline. The paths are simulated and
  labelled as such, not experimental results or a screenshot of the app.
- A keyboard-accessible workflow explorer, expandable plugin descriptions, and
  a citation dialog with a BibTeX copy button.
- All 21 existing documentation articles, with a matching theme and a new
  field-guide overview at `/docs/`. The old article URLs still work.
- Installation instructions updated against the repository's packaging scripts
  and CMake requirements. Other documentation is retained, including its explicit
  limitations and mockup labels.

## Editing

`src/pages/index.astro` contains the homepage content. Shared components and the
two interactive diagrams live in `src/components/`. `src/styles/home.css` styles
the homepage; `src/styles/custom.css` styles the documentation. The documentation
articles remain in `src/content/docs/`.

## Temporary theme comparison

The first design is saved in commit **`1605be9`**. The **Themes** button offers:

| Direction | Typography and palette |
| --- | --- |
| Lab bench (default) | Plain sans serif, graphite and signal orange; crop brackets and instrument ticks |
| Journal | Georgia, book paper and oxblood; a traditional publication style |
| Blueprint | IBM Plex Sans, white and cobalt; light technical figures |
| Darkroom | Monospaced headings, charcoal and amber; an instrument display |
| First shot | Original green palette and Instrument Serif accents |

**Compact** uses more horizontal space, larger body text and less vertical padding.
**Spacious** keeps the first layout's spacing. The **Compare with the first
version** button selects First shot + Spacious together. Theme and density are
saved locally and applied before paint on both the homepage and documentation.

Preset metadata lives in `src/lib/appearance.ts`; colours, typography, and density
rules live in `src/styles/themes.css`. `ThemeInit.astro` applies saved preferences,
and `ThemeSwitcher.astro` supplies the temporary chooser. The canvas reads its
palette from the same CSS variables, without resetting playback or the selected
display mode. No new dependencies are required.

There are no analytics, remote font requests, or external runtime services. The
animation pauses when offscreen or in a background tab. Reduced-motion settings
start the illustration paused; the visitor can choose to play it.

## Deployment

This is a static site. The build creates `dist/`; no server runtime is required.
No deployment workflow or domain configuration has been changed. The original
website's GitLab Pages configuration remains in `../website/.gitlab-ci.yml`.

Before publishing, set `site` (and `base` for a subdirectory) in
`astro.config.mjs` to the intended location. Homepage links use the configured
base. Documentation Markdown links are rewritten by the local remark plugin.
Run the build and link check after changing the base.

To verify a subdirectory build without changing the configuration file:

```bash
SITE_BASE=/useTracker npm run build
SITE_BASE=/useTracker npm run check:links
```

// @ts-check
import { defineConfig } from 'astro/config';
import starlight from '@astrojs/starlight';
import remarkBaseLinks from './scripts/remark-base-links.mjs';

const base = process.env.SITE_BASE || '/';

// `site` is used for canonical URLs and the sitemap — set it to wherever the
// site is actually served. For GitLab Pages that is typically
//   https://<group>.gitlab.io/<project>
// in which case also set `base: '/<project>'`. With a custom domain (configured
// in GitLab under Settings → Pages), keep base: '/'.
//
// Internal links are root-relative and rewritten by Starlight, so changing
// `base` needs no edits to any page.
//
// `publicDir` is renamed from Astro's default `public/` because GitLab Pages
// requires the *built output* to be a directory called `public/`.
export default defineConfig({
  site: 'https://usetracker.org',
  base,
  publicDir: './static',
  markdown: { remarkPlugins: [[remarkBaseLinks, { base }]] },

  integrations: [
    starlight({
      title: 'useTracker',
      description:
        'Free software to detect and track animals in video recordings, built as a pipeline of image-processing plugins.',

      logo: {
        light: './src/assets/logo-light.svg',
        dark: './src/assets/logo-dark.svg',
      },

      favicon: '/favicon.svg',

      social: [
        {
          icon: 'github',
          label: 'GitHub',
          href: 'https://github.com/AlexandreCampo/useTracker',
        },
      ],

      // The website lives in a private repository, so a public "Edit page"
      // link would send visitors somewhere they cannot reach. Re-enable it
      // with the GitLab URL if the site repo is ever opened up:
      //   baseUrl: 'https://gitlab.com/<group>/<project>/-/edit/main/'

      lastUpdated: true,
      customCss: ['./src/styles/custom.css', './src/styles/themes.css'],
      components: {
        SiteTitle: './src/components/DocsSiteTitle.astro',
        ThemeProvider: './src/components/ThemeInit.astro',
        ThemeSelect: './src/components/ThemeSwitcher.astro',
      },

      sidebar: [
        { label: 'The field guide', slug: 'docs' },
        {
          label: 'Getting started',
          items: [
            { label: 'Install', slug: 'start/install' },
            { label: 'First analysis', slug: 'start/first-analysis' },
            { label: 'Examples', slug: 'start/examples' },
          ],
        },
        {
          label: 'Using useTracker',
          items: [
            { label: 'The window', slug: 'guide/window' },
            { label: 'Pipelines', slug: 'guide/pipelines' },
            { label: 'Zones and background', slug: 'guide/zones-background' },
            { label: 'Batch processing', slug: 'guide/batch' },
          ],
        },
        {
          label: 'Plugins',
          items: [
            { label: 'All plugins', slug: 'plugins' },
            { label: 'Image enhancement', slug: 'plugins/enhancement' },
            { label: 'Background subtraction', slug: 'plugins/background' },
            { label: 'Segmentation and morphology', slug: 'plugins/segmentation' },
            { label: 'Blobs and tracking', slug: 'plugins/tracking' },
            { label: 'Markers and detection', slug: 'plugins/markers' },
            { label: 'Zones, recording and output', slug: 'plugins/output' },
          ],
        },
        {
          label: 'Reference',
          items: [
            { label: 'Command line', slug: 'reference/cli' },
            { label: 'Settings file', slug: 'reference/settings-file' },
            { label: 'Output files', slug: 'reference/output-files' },
            { label: 'Troubleshooting', slug: 'reference/troubleshooting' },
          ],
        },
        {
          label: 'Project',
          items: [
            { label: 'History and credits', slug: 'project/history' },
            { label: 'Limitations and known issues', slug: 'project/known-issues' },
            { label: 'Contributing', slug: 'project/contributing' },
          ],
        },
      ],
    }),
  ],
});

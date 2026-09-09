// Check local page links, fragment identifiers, and linked assets in a static build.
// SITE_BASE must match the build when publishing under a subdirectory.
import fs from 'node:fs';
import path from 'node:path';

const dist = path.resolve(process.argv[2] || 'dist');
const base = `/${(process.env.SITE_BASE || '').replace(/^\/+|\/+$/g, '')}`.replace(/\/$/, '');
if (!fs.existsSync(dist)) throw new Error('Run npm run build first.');
const pages = new Map();
const links = [];
const origin = 'https://local.invalid';
const decode = value => value.replace(/&amp;/g, '&').replace(/&quot;/g, '"');
function walk(dir) {
  for (const entry of fs.readdirSync(dir, { withFileTypes: true })) {
    const filename = path.join(dir, entry.name);
    if (entry.isDirectory()) { walk(filename); continue; }
    if (!entry.name.endsWith('.html')) continue;
    const relative = path.relative(dist, filename).split(path.sep).join('/');
    const route = `${base}/${relative.replace(/index\.html$/, '')}`;
    const html = fs.readFileSync(filename, 'utf8');
    const ids = new Set([...html.matchAll(/\bid="([^"]*)"/g)].map(match => decode(match[1])));
    pages.set(route, ids);
    for (const match of html.matchAll(/\bhref="([^"]+)"/g)) {
      const href = decode(match[1]);
      if (/^(?:[a-z][a-z\d+.-]*:|\/\/)/i.test(href)) continue;
      links.push({ from: route, url: new URL(href, origin + route) });
    }
  }
}
walk(dist);
const errors = new Set();
for (const { from, url } of links) {
  const route = decodeURIComponent(url.pathname);
  const page = pages.get(route) || pages.get(route.replace(/\/?$/, '/'));
  if (page) {
    const id = decodeURIComponent(url.hash.slice(1));
    if (id && !page.has(id)) errors.add(`${from} → ${route}#${id} (missing fragment)`);
    continue;
  }
  if (base && !route.startsWith(base + '/')) { errors.add(`${from} → ${route} (outside site base)`); continue; }
  const file = path.join(dist, route.slice(base.length).replace(/^\//, ''));
  if (!fs.existsSync(file) || !fs.statSync(file).isFile()) errors.add(`${from} → ${route} (missing target)`);
}
if (errors.size) {
  console.error([...errors].join('\n'));
  console.error(`${errors.size} broken link(s).`);
  process.exit(1);
}
console.log(`All ${links.length} internal links and fragments resolve across ${pages.size} pages.`);

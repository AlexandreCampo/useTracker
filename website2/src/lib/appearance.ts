/** Temporary design-study controls. The first design is kept as a comparison. */
export const themes = [
  { id: 'lab', name: 'Lab bench', description: 'Graphite, signal orange, plain sans serif.', colors: ['#f3f3f0', '#26292c', '#b34420'] },
  { id: 'journal', name: 'Journal', description: 'Book paper, oxblood, traditional serif.', colors: ['#faf8f2', '#282627', '#863741'] },
  { id: 'blueprint', name: 'Blueprint', description: 'White, cobalt, technical typography.', colors: ['#f7f9fc', '#183555', '#2557ae'] },
  { id: 'darkroom', name: 'Darkroom', description: 'Charcoal, amber, instrument display.', colors: ['#171b20', '#edf0f4', '#e7b665'] },
  { id: 'original', name: 'First shot', description: 'The original green and italic serif.', colors: ['#f5f5ee', '#233a30', '#245b44'] },
] as const;

export type Theme = typeof themes[number]['id'];
export type Density = 'compact' | 'spacious';
export const themeKey = 'usetracker-appearance';
export const densityKey = 'usetracker-density';
export const isTheme = (value: unknown): value is Theme => themes.some(theme => theme.id === value);

export function applyAppearance(theme: Theme, density: Density) {
  const root = document.documentElement;
  root.dataset.siteTheme = theme;
  root.dataset.density = density;
  root.dataset.theme = theme === 'darkroom' ? 'dark' : 'light';
  document.querySelector('meta[name="theme-color"]')?.setAttribute('content', themes.find(item => item.id === theme)!.colors[0]);
  try {
    localStorage.setItem(themeKey, theme);
    localStorage.setItem(densityKey, density);
  } catch { /* Private browsing may disallow storage; the controls still work. */ }
  document.dispatchEvent(new CustomEvent('usetracker:appearance'));
}

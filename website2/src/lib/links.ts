/** Keep the custom pages usable when Astro is hosted under a subdirectory. */
export function local(path = '') {
  return `${import.meta.env.BASE_URL.replace(/\/$/, '')}/${path.replace(/^\//, '')}`;
}

export const repository = 'https://github.com/AlexandreCampo/useTracker';

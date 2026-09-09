/** Prefix documentation links for static hosting under a subdirectory. */
export default function remarkBaseLinks({ base = '/' } = {}) {
  const prefix = base.replace(/\/$/, '');
  return function transform(tree) {
    if (!prefix) return;
    function visit(node) {
      if ((node.type === 'link' || node.type === 'definition') &&
          node.url?.startsWith('/') && !node.url.startsWith('//')) {
        node.url = `${prefix}${node.url}`;
      }
      node.children?.forEach(visit);
    }
    visit(tree);
  };
}

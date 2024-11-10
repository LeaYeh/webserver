// .github/scripts/check_verb.js
const isVerb = require('is-verb').default;

const word = process.argv[2];
if (!word) {
  console.error('No word provided to check');
  process.exit(1);
}

if (!isVerb(word.toLowerCase())) {
  console.error(`Error: Word "${word}" is not a verb`);
  process.exit(1);
}

process.exit(0);

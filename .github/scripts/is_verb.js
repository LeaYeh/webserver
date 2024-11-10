// .github/scripts/check_verb.js
const capitalized = require('is-verb').capitalized;

const word = process.argv[2];
if (!word) {
  console.error('No word provided to check');
  process.exit(1);
}

if (!capitalized(word)) {
  console.error(`Error: Word "${word}" is not a capizalized verb`);
  process.exit(1);
}

process.exit(0);

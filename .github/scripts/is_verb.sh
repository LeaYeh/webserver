#!/bin/bash

is_verb() {
  node -e "
    const isVerb = require('is-verb').default;
    const word = process.argv[1];
    if (!isVerb(word)) {
      console.error('Error: First word after type must be a verb. Found: ' + word);
      process.exit(1);
    }
  " "$1"
}

# Get all commit messages if it's a PR, otherwise just the latest commit
if [ "$GITHUB_EVENT_NAME" == "pull_request" ]; then
  COMMITS=$(git log --format=%B "$BASE_SHA..$HEAD_SHA")
else
  COMMITS=$(git log -1 --format=%B)
fi

# Check each commit message
echo "$COMMITS" | while read -r commit; do
  # Skip merge commits
  if [[ $commit == Merge* ]]; then
    continue
  fi

  # Extract the part after the colon and get the first word
  AFTER_COLON=$(echo "$commit" | sed 's/^[^:]*: //')
  FIRST_WORD=$(echo "$AFTER_COLON" | awk '{print $1}')

  # Skip empty messages
  if [ -z "$FIRST_WORD" ]; then
    continue
  fi

  # Check if the word is a verb
  is_verb "$FIRST_WORD"
done

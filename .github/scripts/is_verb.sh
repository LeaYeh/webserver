#!/bin/bash

# Function to check if a string is a capitalized verb using is-verb
is_verb() {
  node -e "
    const capitalized = require('is-verb').capitalized;
    const word = process.argv[1];
    process.exit(capitalized(word) ? 0 : 1);
  " "$1"
}

# Get all commit messages if it's a PR, otherwise just the latest commit
if [ "$GITHUB_EVENT_NAME" == "pull_request" ]; then
  COMMITS=$(git log --format=%s "$BASE_SHA..$HEAD_SHA")
else
  COMMITS=$(git log -1 --format=%s)
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

  echo "Checking word: $FIRST_WORD"

  # Check if the word is a capitalized verb
  if ! is_verb "$FIRST_WORD"; then
    echo "Error: '$FIRST_WORD' is not a capitalized verb."
    exit 1
  fi
done

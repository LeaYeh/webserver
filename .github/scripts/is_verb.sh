#!/bin/bash

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
  if ! node .github/scripts/is_verb.js "$FIRST_WORD"; then
    exit 1
  fi
done

#!/usr/bin/env bash

# Find all staged cpp files, and exit early if there aren't any.
CPP_FILES=()
while IFS=$'\n' read -r line; do CPP_FILES+=("$line"); done \
  < <(git diff --name-only --cached --diff-filter=AM | grep --color=never -E "\.(c|cpp|h)$" | grep -v -E "^third_party")

if [ ${#CPP_FILES[@]} -eq 0 ]; then
  exit 0
fi

error_handle() {
  readonly EXIT_CODE=$1
  echo "If you don't want the pre-commit check, run"
  echo "git commit --no-verify"
  exit "${EXIT_CODE}"
}

if ! command -v clang-format >/dev/null; then
  echo 'clang-format not on path; can not format. Please install clang-format.'
  error_handle 2
fi

CHANGED_FILES=()
while IFS=$'\n' read -r line; do CHANGED_FILES+=("$line"); done \
  < <(git diff --name-only "${CPP_FILES[@]}")
if [ ${#CHANGED_FILES[@]} -gt 0 ]; then
  echo 'You have unstaged changes to some files in your commit; skipping '
  echo 'auto-format. Please stage, stash, or revert these changes. You may '
  echo 'find `git stash -k` helpful here.'
  echo 'Files with unstaged changes:' "${CHANGED_FILES[@]}"
  error_handle 1
fi

# Format all staged files, then exit with an error code if any have uncommitted
# changes.
echo 'Formatting staged CPP files . . .'

clang-format -i "${CPP_FILES[@]}"

CHANGED_FILES=()
while IFS=$'\n' read -r line; do CHANGED_FILES+=("$line"); done \
  < <(git diff --name-only "${CPP_FILES[@]}")
if [ ${#CHANGED_FILES[@]} -gt 0 ]; then
  echo 'Reformatted staged files. Please review and stage the changes.'
  echo 'Files updated: ' "${CHANGED_FILES[@]}"
  error_handle 1
else
  exit 0
fi

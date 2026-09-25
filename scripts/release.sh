#!/usr/bin/env bash
#
# Release a new version of cpp-unicodelib.
#
# Usage: ./release.sh [--run]
#
# By default, runs in dry-run mode (no changes made).
# Pass --run to actually update files, commit, tag, and push.
#
# The version is vMAJOR.MINOR.PATCH, where MAJOR.MINOR is the Unicode
# version the release supports and PATCH counts the library releases made on
# that Unicode version. So there is no bump type to choose: it follows from
# Unicode_Version in unicodelib.h.
#
# This script:
#   1. Reads the current version (CPPUNICODELIB_VERSION) and the Unicode
#      version (Unicode_Version) from unicodelib.h
#   2. Checks that the working directory is clean
#   3. Verifies CI status of the latest commit
#   4. Determines the next version:
#        - Unicode_Version moved past the current MAJOR.MINOR
#          → the first release on it (e.g., 17.0.2 → 18.0.0)
#        - otherwise → patch bump (e.g., 18.0.0 → 18.0.1)
#   5. Updates unicodelib.h
#   6. Commits, tags (vX.Y.Z), and pushes

set -euo pipefail

cd "$(dirname "$0")/.."

DRY_RUN=1
while [ $# -gt 0 ]; do
  case "$1" in
    --run)
      DRY_RUN=0
      shift
      ;;
    *)
      echo "Usage: $0 [--run]"
      exit 1
      ;;
  esac
done

# --- Step 1: Read the current version and the Unicode version ---
CURRENT_VERSION=$(sed -n 's/^#define CPPUNICODELIB_VERSION "\([^"]*\)"/\1/p' unicodelib.h)
UNICODE_VERSION=$(sed -n 's/^constexpr auto Unicode_Version = "\([^"]*\)";/\1/p' unicodelib.h)
IFS='.' read -r V_MAJOR V_MINOR V_PATCH <<< "$CURRENT_VERSION"
IFS='.' read -r U_MAJOR U_MINOR _ <<< "$UNICODE_VERSION"

echo "==> Current version: $CURRENT_VERSION (Unicode $UNICODE_VERSION)"

# --- Step 2: Check working directory is clean ---
if [ -n "$(git status --porcelain --untracked-files=no)" ]; then
  echo "Error: working directory is not clean"
  exit 1
fi

# --- Step 3: Check CI status of the latest commit ---
echo ""
echo "==> Checking CI status of the latest commit..."

HEAD_SHA=$(git rev-parse HEAD)
HEAD_SHORT=$(git rev-parse --short HEAD)
echo "    Latest commit: $HEAD_SHORT"

# Fetch all workflow runs for the HEAD commit. A commit can accumulate
# multiple runs of the same workflow (reruns), so judge only the most
# recent run of each workflow.
RUNS=$(gh run list --commit "$HEAD_SHA" \
         --json name,status,conclusion,headSha,createdAt |
       jq '[group_by(.name)[] | max_by(.createdAt)]')

NUM_RUNS=$(echo "$RUNS" | jq 'length')

if [ "$NUM_RUNS" -eq 0 ]; then
  echo "Error: No CI runs found for commit $HEAD_SHORT."
  echo "       Push it and wait for CI to complete before releasing."
  exit 1
fi

echo "    Found $NUM_RUNS workflow run(s):"

FAILED=0
RUNNING=0
while IFS=$'\t' read -r name status conclusion; do
  # A run that hasn't completed yet has an empty conclusion; don't treat it
  # as a failure — the release should wait until CI finishes.
  if [ "$status" != "completed" ]; then
    echo "      [ .. ] $name (still running)"
    RUNNING=1
    continue
  fi

  if [ "$conclusion" = "success" ]; then
    echo "      [ OK ] $name"
  else
    echo "      [FAIL] $name ($conclusion)"
    FAILED=1
  fi
done < <(echo "$RUNS" | jq -r '.[] | [.name, .status, .conclusion] | @tsv')

if [ "$RUNNING" -eq 1 ]; then
  echo ""
  echo "Error: Some CI checks are still running. Wait for them to complete before releasing."
  exit 1
fi

if [ "$FAILED" -eq 1 ]; then
  echo ""
  echo "Error: Some CI checks failed. Fix them before releasing."
  exit 1
fi

echo "    All CI checks passed."

# --- Step 4: Determine new version ---
echo ""
if [ "$U_MAJOR.$U_MINOR" != "$V_MAJOR.$V_MINOR" ]; then
  NEW_VERSION="$U_MAJOR.$U_MINOR.0"
  echo "==> Unicode $U_MAJOR.$U_MINOR is new → first release on it"
else
  NEW_VERSION="$V_MAJOR.$V_MINOR.$((V_PATCH + 1))"
  echo "==> Same Unicode $U_MAJOR.$U_MINOR → patch bump"
fi

if git rev-parse -q --verify "refs/tags/v$NEW_VERSION" >/dev/null; then
  echo "Error: tag v$NEW_VERSION already exists"
  exit 1
fi

IFS='.' read -r N_MAJOR N_MINOR N_PATCH <<< "$NEW_VERSION"
VERSION_HEX=$(printf "0x%02x%02x%02x" "$N_MAJOR" "$N_MINOR" "$N_PATCH")

if [ "$DRY_RUN" -eq 1 ]; then
  echo "==> [DRY RUN] New version: $NEW_VERSION ($VERSION_HEX)"
else
  echo "==> New version: $NEW_VERSION ($VERSION_HEX)"
fi

# --- Step 5: Update files ---
echo ""
if [ "$DRY_RUN" -eq 1 ]; then
  echo "==> [DRY RUN] Would update unicodelib.h:"
  echo "    CPPUNICODELIB_VERSION     = \"$NEW_VERSION\""
  echo "    CPPUNICODELIB_VERSION_NUM = \"$VERSION_HEX\""
  echo ""
  echo "==> [DRY RUN] Would commit, tag v$NEW_VERSION, and push."
  echo ""
  echo "==> Dry run complete. No changes were made."
else
  echo "==> Updating unicodelib.h..."
  sed -i '' "s/#define CPPUNICODELIB_VERSION \"[^\"]*\"/#define CPPUNICODELIB_VERSION \"$NEW_VERSION\"/" unicodelib.h
  sed -i '' "s/#define CPPUNICODELIB_VERSION_NUM \"0x[0-9a-fA-F]*\"/#define CPPUNICODELIB_VERSION_NUM \"$VERSION_HEX\"/" unicodelib.h
  echo "    CPPUNICODELIB_VERSION     = \"$NEW_VERSION\""
  echo "    CPPUNICODELIB_VERSION_NUM = \"$VERSION_HEX\""

  # --- Step 6: Commit, tag, and push ---
  echo ""
  echo "==> Committing and tagging..."
  git add unicodelib.h
  git commit -m "Release v$NEW_VERSION"
  git tag "v$NEW_VERSION"

  echo ""
  echo "==> Pushing..."
  git push && git push --tags

  echo ""
  echo "==> Released v$NEW_VERSION"
fi

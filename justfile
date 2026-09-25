set shell := ["bash", "-cu"]

default:
    @just --list

# The version follows Unicode_Version in unicodelib.h: a new Unicode version
# gives X.Y.0, otherwise the patch number goes up.
# Tag and push a release (dry-run; --run to execute)
release *args:
    @./scripts/release.sh {{args}}

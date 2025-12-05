#!/bin/bash

cd "$(dirname "$0")/.."
set -e

uv run scripts/update.py
uv run scripts/gen_unicode_names.py UCD > unicodelib_names.h

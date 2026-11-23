# Mod Development

This document will describe the stable mod API after the SDK interface is implemented.

## Planned package

Each mod will have a directory containing:

- `mod.json` — mod identity, version, compatibility, and library filename.
- A native Android library, normally `lib<mod-id>.so`.

## Current status

The package layout is a draft. No stable ABI or mod lifecycle callbacks have been published yet. Avoid distributing mods against this early skeleton until the SDK version and ABI are documented.

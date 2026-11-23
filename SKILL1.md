---
name: gd-docs
description: Authoritative reference for the Geometry Dash data layer — the Boomlings/GD server API (endpoints, params, response objects, secrets, gjp2, chk hashes), client file formats (CCLocalLevels.dat, level string, gamesave keys), GD's encodings (XOR keys, salts, RobTop cipher, level passwords, vault codes), player physics (gamemode velocities, hitboxes, orbs, pads, force blocks), and the full 2.2 object table — 3986 object IDs and every property key of 123 object and trigger classes. Use WHENEVER the user works with GD levels, saves or servers outside the editor — parsing or building a level string, decoding a save file, writing a GD API client, bot or private server, what property key 51 means on a Move Trigger, what object ID 1611 is, how to compute a chk or gjp2, a gamemode's jump velocity. Also triggers on French phrasing (décoder un niveau GD, API Boomlings, propriétés d'objet GD). Never answer from memory — these are exact values that shift between versions; look them up.
---

# Geometry Dash documentation

Two bundled sources, both current as of GD 2.2:

- `references/docs/` — the [gd-docs](https://github.com/gd-docs/gd-docs) project (boomlings.dev), 137 pages: server API, file formats, encodings, physics.
- `references/data/` — [gd-info-explorer](https://github.com/FlowVix/gd-info-explorer) by FlowVix: 3986 object IDs and the property schema of 123 object/trigger classes.

Both are reached through two scripts. They resolve their own data paths, so call them from anywhere.

Snapshot dates, licences and the one transformation applied to the upstream markdown are in `references/SOURCES.md` — read it when the user asks where an answer comes from, or before claiming the data is current.

## Look it up, don't recall it

Almost every answer here is an exact value — property key `51`, XOR key `26364`, secret `Wmfd2893gb7`, object ID `1611`. A value recalled from memory that is off by one produces code that silently corrupts a save or gets rejected by the server, and it looks just as confident as the right one. So run the lookup even when you are sure, and quote what came back.

2.2 also renumbered and added a great deal. Anything you remember from 2.1 is a liability now.

## Route the question

| The user is asking about | Run |
| --- | --- |
| An object ID — what it is, what properties it accepts | `gd.py obj 1611` |
| An object by name | `gd.py obj "pulse"` |
| Every property of a class or trigger | `gd.py class MoveTrigger` |
| A bare property key — who uses it, what it means | `gd.py prop 51` |
| A property by name | `gd.py prop target_group` |
| The meaning of a raw object string | `gd.py parse "1,901,2,15,3,15,51,4"` |
| Enum values (easing, z layer, trigger layer, colors) | `gd.py enum Easing` |
| A particle string's field order | `gd.py particles` |
| One endpoint's params, response, example | `gd.py doc endpoints/levels/downloadGJLevel22` |
| Anything else in the docs | `gd.py search "<regex>" -i`, then `gd.py doc <path>` |
| Decoding or encoding real data | `gdcrypt.py <command>` |

When `search` returns a hit, fetch just that section — `gd.py doc <path> "<heading>"` — rather than the whole file. Use `gd.py toc <path>` first if you need the heading names.

## scripts/gd.py — lookup

```
obj <id|name>        object identity, its class, and every property that class accepts
class <name>         all properties of a class, inherited ones included
classes [filter]     list the 123 classes
prop <key|name>      reverse lookup: which classes use this key, and as what
enum [name]          enum variants and their integer values
types                every value type in the schema
particles            the ordered fields of a particle string
parse <objstring>    decode `key,value,...;` pairs into named properties
search <regex> [-i]  grep the documentation, prints path:line
doc <path> [heading] print a doc file, or one section of it
toc <path>           headings of a doc file
ls [dir]             list doc files with sizes
```

`parse` accepts a file path as well as inline text, so a level string decoded with `gdcrypt.py` can be piped straight through it. Keys it reports as `(not in <class>)` are genuinely absent from the schema — say so rather than inventing a meaning.

## scripts/gdcrypt.py — encodings

```
savefile <decode|encode> <path> [-o out] [--mac]   CCGameManager.dat / CCLocalLevels.dat
levelstring <decode|encode> <data|file> [--official]  gzip + URL-safe base64
gjp2 <password>                                    the 2.2 auth value
gjp <encode|decode> <value>                        the pre-2.2 password encoding
password <decode|encode> <value>                   level copy password
chk <values...> --key K [--salt S]                 request integrity hash
xor <data|file> --key K [--static]                 raw XOR cipher
b64 <encode|decode> <data|file>                    URL-safe base64, padding-tolerant
robtop <encode|decode> <data> --key K [--static]   base64 + XOR combo
uploadseed <levelstring|file>                      seed2 for level upload
leaderboard-seed <jumps> <pct> <secs> [played]     classic level leaderboard seed
keys                                               XOR keys, salts and secrets, annotated
```

Prefer running these over describing them. A user asking "what's in my save file" wants the decoded contents, not a snippet they have to run themselves — decode it and answer from what came out.

`gdcrypt.py keys` is the single source of truth for constants; read it instead of quoting a key from memory. Only `savefile --mac` needs a third-party package (`pycryptodome`); everything else is stdlib.

## A worked chain

The three layers compose. Going from a raw server response to named properties:

```bash
# 1. the response field `4:` of downloadGJLevel22 is the compressed level string
python3 scripts/gdcrypt.py levelstring decode "$K4" > level.txt

# 2. the first `;`-segment is the header (kA/kS keys), the rest are objects
python3 scripts/gd.py parse level.txt | head -40

# 3. anything unclear in the header
python3 scripts/gd.py doc resources/client/level-components/level-start.md
```

Going the other way — building a level string — is the same steps reversed, with
`gd.py class <trigger>` giving the keys to emit and `gdcrypt.py levelstring encode` at the end.

## Where things live

`references/INDEX.md` maps all 137 doc pages with a one-line summary and a size flag for each. Read it when `search` misses or when you need to see what a whole area covers.

The broad shape:

- `endpoints/` — one page per `.php` endpoint, plus `generic.md` for how requests are formed and `index.md` for the full list with base URLs.
- `resources/server/` — the `key:value:key:value` objects endpoints return (level, user, comment, song, leaderboard score…).
- `resources/client/` — save file and level file internals: `gamesave.md`, `level.md`, and `level-components/` for the level string, colors, guidelines, particles, replays.
- `reference/` — secrets, salts, keys, official songs, and `player_physics/` for gamemode velocities, hitboxes, orbs and pads, force blocks.
- `topics/` — the encodings (`encryption/`), gjp, level passwords, tags, shops, vault codes.

Doc cross-links are doc-root-absolute: a link to `/topics/gjp` means `references/docs/topics/gjp.md`, which `gd.py doc topics/gjp` fetches. The same rule covers images — `/assets/images/shake_tag.gif` is a real file at `references/docs/assets/images/`. `topics/tags.md` leans on three GIFs to show what the fade-in, delay and shake text tags actually do; view them rather than describing the effect from the prose alone. Three further images in `tags.md`, `secrets.md` and `songs.md` point at raw.githubusercontent.com URLs upstream and need the network — the colour-tags one has a local copy in the same folder.

## Traps worth knowing before you write code

**Requests fail without two non-obvious headers.** Send to the `www.` subdomain with an *empty* User-Agent, or Cloudflare answers 1020. Requests are `POST` with `Content-Type: application/x-www-form-urlencoded`. Rate limits are real: roughly 20 `downloadGJLevel` per minute, ~2/s elsewhere, plus longer-term caps.

**Base64 here is URL-safe (`-` and `_`) and frequently unpadded.** A standard decoder throws on it. `gdcrypt.py b64` handles both.

**Property key `1` is the object ID and is not part of the schema's `@Common`.** `gd.py parse` handles it; a hand-rolled parser must too.

**An object ID with no explicit class falls back to `Common`.** That is expected, not a gap in the data.

**Two object property tables exist and only one is current.** `references/data/schema.txt` (via `gd.py`) is the 2.2 source of truth. The table inside `resources/client/level-components/level-string.md` is the older 2.1-era one, kept for reference — use it only for historical questions.

**`gjp` became `gjp2` in 2.2** — SHA-1 of password + salt, not the old XOR+base64. Both are in `gdcrypt.py`.

**Official levels are stored with the first 13 base64 characters stripped.** Pass `--official` to `gdcrypt.py levelstring` and it prepends `H4sIAAAAAAAAA` back.

**Physics values are in Vels**: 1 Vel = 60 units/sec, 1 block = 30 units.

## Adjacent skills

This skill covers GD's data — formats, server API, object keys. It does not cover using the editor's UI (that's `gd-editor-guide`), writing C++ mods (`geode-modding`), or the GDHistory archive (`gdhistory-api`). Reach for those when the question is about those instead.

## When the docs fall short

Some pages carry TODOs or a `(?)` on an uncertain value, and a few endpoints are documented only partially — that is honest upstream uncertainty. Report it as uncertain rather than smoothing it over, and say which page it came from so the user can judge. If something is absent entirely, say so; boomlings.dev and the two GitHub repos are where a newer answer would appear.

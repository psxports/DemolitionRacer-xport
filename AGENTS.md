# Demolition Racer

Resolve `[XPORT_ROOT]` from `xport-project.json` and read `[XPORT_ROOT]/AGENTS.md`. Keep only stable game-specific facts and task-routing links here; put detailed evidence under `status`. Never create, edit, move, delete or regenerate `README.md`.

## Project facts

- Native short name: `DR`; language: C
- Solution: `src/platform/win/DR.sln`; Debug executable: `bin/DR_debug.exe`; Release executable: `bin/DR.exe`; working directory: `bin`; intermediates: `_build`
- Runtime data: `bin/DATA`; Red Book output when applicable: `bin/MUSIC`
- Before relying on them, record reviewed image identities, language decision, dummy scope, hooks/layouts, adapter contract and evidence links here

## Reviewed startup evidence

- Boot image: `SLUS_009.69`, selected by `orig/SYSTEM.CNF`; entry `0x80047B04`; runtime GP `0x80064A10` from the adjacent LUI/ADDIU at `0x80047B80`
- Boot image SHA-256: `f0a1daff84339138f65dfd7a3d0a5a43fb05e03fa7388305b9495627778ef90d`
- Auxiliary image: `MOVPLAY.EXE`; entry `0x800C14B8`; runtime GP `0x800E0454` from the adjacent LUI/ADDIU at `0x800C14DC`
- Auxiliary image SHA-256: `7d20d8b936e02c392852ca2775a6bc8582ce35bf043f2b8c8f8dd4146a88cc0b`
- Immutable canonical exports: `orig/images`; acceptance receipt: `status/ida/accepted/startup-ida-v1.json`
- PsyQ review: `status/ghidra/classification.json`; 568 accepted SDK boundaries, 24 conflicting API classifications remain TODO; wrappers are unresolved and SKIP does not prove runtime equivalence
- Configured native root: `SLUS_009.69:0x80047B04`; reviewed SDK startup boundary, SKIP; no game callees translated during startup
- Language target: C; original game source language remains unproven; no game stubs or guessed implementations are authorized by startup
- Recording image: `SLUS_009.69`; record role `user`, comparison role `lockstep`; role directories and ports come only from `xport-project.json`
- The reviewed CUE contains one MODE2/2352 data track and no AUDIO tracks; Red Book conversion is disabled
- GPU graph-type byte: `SLUS_009.69:0x8005CC28`, referenced by original ResetGraph `0x8003D630` and SetGraphDebug `0x8003D7A4`; listings: `orig/images/SLUS_009.69/functions/8003D630.mips.txt`, `orig/images/SLUS_009.69/functions/8003D7A4.mips.txt`
- Native movies use the original MOVPLAY controller through shared PsyQ STR/VLC/MDEC; `sub_8002BC5C` loads and executes the native movie entry
- Native menu and race music use extracted DATA/*.XA streams; CD/SPU routing is initialized by CdInit and CdReset(1); media evidence: `status/draft-c/native-media-runtime-evidence.json`


- Native launch repair routing and current limitations: `status/draft-c/native-launch-latest.json`; current movie/music integration: `status/draft-c/native-media-latest.json`; folder loading contract: `status/draft-c/data-folder-release.json`

- Native frame skipping is disabled at dr_startup with gpu_set_frame_skip(0)

- Final Release uses shared xcrt.c; build and runtime evidence, restored paths and explicit limitations: `status/draft-c/final-release-evidence.json`

- Native two-player restoration and current runtime scope: `status/draft-c/two-player-evidence.json`

- Native league qualification and next-stage routing, exercised with original finish-cheat input: `status/draft-c/league-advancement-evidence.json`

- Native resources use xport_fopen under bin/DATA; DD.IDX supplies file sizes, extracted resource files supply payloads, and XA/STR use cd_mount_stream_file; CUE/BIN and DD.DAT are not runtime dependencies

- Web Release: build with `make CONFIG=Release` in `src/platform/web`; output `../.github/site/DR`; AP_WEB uses MUSIC/1.WAV..16.WAV through RedBook, XA and STR are excluded; evidence: `status/draft-c/web-redbook-release.json`

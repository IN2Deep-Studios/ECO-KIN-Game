---
applyTo: "**"
---
# Echohearts production intake and repair instructions (2026-10-06)

This supplements the repository's existing `.github/copilot-instructions.md`. Read the actual nearby files and the canonical [project index](https://github.com/Dlomotion/Echohearts-Rebearth/blob/main/MASTER_PROJECT_INDEX.md) before changing code or canon. A repository instruction can guide Copilot in this repository; it does not grant access to other repositories or make Copilot execute work autonomously.

## Repository role
Supporting game prototype. Inspect its actual engine/framework before edits; reconcile mechanics with canonical design and report prototype-only results as such.

## Requested work and priority
- Turn user requests into a bounded issue/branch/PR with acceptance criteria, then inspect code and logs, implement the smallest connected slice, run available checks, and report exact evidence and limitations. Fix the underlying cause of failing code; do not merely generate disconnected examples.
- Production order: UE5.8 foundation and runtime proof; Issue #10 animation benchmark; 4–6 Eco-Kin vertical slice; Growth Rite; Event Sovereign reservation/save/recovery; bounded registry/UI/save; later online, seasonal and war systems. Preserve the current canonical production order when newer records revise it.
- Main campaign is real time. The first tactical Arena proof is one bounded local encounter after shared identity, stats, and runtime work; matchmaking, ranked, reconnect, and online replication require separate evidence. PR #6 manuscript missions need region, character, Dex and outcome dependencies; use Era 3 “War on Humanity” pending canonical text review. Do not call either PR's prose playable.
- Preserve nine Rebearth Essences from the current canon lock; flag older 12-element text as a conflict rather than editing every source blindly. Preserve the 125-ID Permanent Dex authority and the 1,120-name historical pool as archive.
- Four canon attributes: Vibrance, Density, Harmony, Purity. Anima-Link links player strain to Eco-Kin damage. PvE carries 8 unique Eco-Kin with 3 active; PvP roster up to 6 unique Eco-Kin. Care and voluntary tasks start with 5 worker slots and expand by area level. Never trade sentient Eco-Kin or eggs.

## New intake: survival, depository, narrative
- The pasted `EchoheartsMetabolicComponent` and `EchoheartsSaveEngine` snippets are **uncompiled proposals**, not merged features. Their joined include directives are invalid; do not paste them uncorrected or assume the class/module/API names match the real build. Validate inputs, authority, inventory consumption, save versioning and bounds.
- Food, water, shelter and care can affect internal stamina/sustain conditions; ordinary food must not automatically raise Purity. Environmental stress should be bounded, deterministic for authoritative state, and saved only where appropriate. Make forage → deposit → consume → save/reload → visible restoration the first bounded design-to-runtime candidate.
- Mead-Hall food depository, alkaline crops, pastoral care and proposed buffs are design intake. Eco-Kin assistance remains voluntary. Preserve crops as potential item records and distinguish mass capacity from unit counts. The 2-hour buff, +15% turn velocity, +5 m grappling range, infinite preservation, named boss, creator reveal and restoration ledger outcomes need balance/story review; do not encode as locked facts.
- Professor Jackson and Valley of Ruins scene is a narrative proposal. Reconcile A.E.G.I.S. bracer vs “Echo-Pendant,” Kindling spelling, titles, variants and Era chronology before canon promotion. Do not mark fictional in-world log sheets as engine test evidence.
- Available uploaded images must be matched to approved identities through art QA and provenance before use. No image asset was included by this instruction file.

## Git, assets, CI and verification
- Work on a topic branch with reviewed diffs and a PR. Inspect existing `.gitignore`, `.gitattributes`, CI and actual engine version before editing. Do not run `git init` in an existing repository, `git add .` blindly, or copy the sample UE5.7/self-hosted Shipping workflow into a UE5.8 project. Do not blanket-ignore `Build/`; track `.uasset`/`.umap` in LFS according to the established asset policy, reviewing other binary extensions case by case.
- Never auto-download a GitHub code snippet into the game. Check source license and provenance; implement an original, reviewed fix.
- On a trusted, appropriately isolated UE5.8 runner, use the real project path and report clean clone/LFS, UHT, editor compile/launch, map/PIE, packaged Development, save/load and multiplayer or platform tests separately. A standalone C++17 or web build proves only its own scope. Avoid exposing a public-repository self-hosted workstation to untrusted workflow code.
- Use CANON / APPROVED-PENDING / REFERENCE-ONLY / NOT YET VERIFIED / VERIFIED accurately. State exact commands, results, artifacts and remaining gaps. A class declaration, design page or checked box never proves runtime, network, security or save/reload success.

# Executed results

Six builds/runs on October 11, 2026: three intended baseline failures and three
fixed passes. Every configure and build succeeded. This counts execution modes,
not six distinct bugs.

| Case | Baseline | Fixed |
| --- | --- | --- |
| json-c public API client | Exit 1: expected `41`, got `EF BF BD 41` | Exit 0: reset/error-reset checks and no-reset continuation pass |
| LibTomMath static parent install | Exit 1: parent's `doc/tommath.3` absent | Exit 0: installed manpage matches source |
| LibTomMath shared parent install | Exit 1: parent's `doc/tommath.3` absent | Exit 0: installed manpage matches source |

[results.json](results.json) records exact revisions, fixture SHA-256 hashes,
tool versions and each exit status. The six text logs contain configure, build
and run/install output. `$WORK` and `$COLLECTION` replace local directory prefixes;
trailing whitespace is removed; other output is retained. Expected baseline errors
are preserved.
`passed: true` means the expected outcome was observed: a specific failure on
baseline rows, and successful execution on fixed rows.

Source acquisition for these receipts was `git archive` from existing public
repository checkouts, using the full pinned commit IDs. No production code was
changed. The case READMEs give equivalent clone/fetch instructions for readers;
those network acquisition commands were not rerun for these receipts.

Execution is macOS arm64, Apple Clang 21.0.0 and CMake 4.4.3. Other platforms,
full project suites and upstream CI are outside these collection receipts.
Earlier upstream-PR test results remain separate.

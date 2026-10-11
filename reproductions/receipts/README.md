# Executed results

Executed on October 11, 2026: three bugs, each exercised before and after its
patch. Every configure and build succeeded.

| Case | Baseline | Fixed |
| --- | --- | --- |
| json-c public API client | Exit 1: expected `41`, got `EF BF BD 41` | Exit 0: reset/error-reset checks and no-reset continuation pass |
| LibTomMath static parent install | Exit 1: parent's `doc/tommath.3` absent | Exit 0: installed manpage matches source |
| LibTomMath shared parent install | Exit 1: parent's `doc/tommath.3` absent | Exit 0: installed manpage matches source |
| PCRE2 8/16/32-bit interpreter and JIT | Four whole-pattern matches are wrong in each of six modes; two local-subroutine controls pass | All six matches are correct in every mode; each JIT result carries its execution marker |

[results.json](results.json) records exact revisions, fixture SHA-256 hashes,
tool versions and each exit status. The six text logs contain configure, build
and run/install output. [pcre2-results.json](pcre2-results.json) records the two
PCRE2 builds and twelve CLI runs, including matched text and JIT markers. Its
fourteen logs preserve build and match output. PCRE2 exits 0 on both revisions;
the matched text determines whether the defect was reproduced.
`$WORK` and `$COLLECTION` replace local directory prefixes;
trailing whitespace is removed; other output is retained. Expected baseline errors
are preserved.
`passed: true` means the expected outcome was observed: a specific failure on
baseline rows, and correct execution on fixed rows.

Source acquisition for these receipts was `git archive` from existing public
repository checkouts, using the full pinned commit IDs. PCRE2's pinned SLJIT
source was exported separately. No production code was changed. The case
READMEs give equivalent clone/fetch instructions for readers;
those network acquisition commands were not used to generate the build receipts.
They were subsequently executed in separate clean clones: all six source trees
match the tested revisions, including PCRE2's SLJIT gitlink.
[source-availability.json](source-availability.json)
records that check; builds were not repeated for it.

Execution is macOS arm64, Apple Clang 21.0.0 and CMake 4.4.3. Other platforms,
full project suites and upstream CI are outside these collection receipts.
Earlier upstream-PR test results remain separate.

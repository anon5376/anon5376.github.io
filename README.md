# qeqx

[GitHub: anon5376](https://github.com/anon5376)

I work on small, reproducible failures in infrastructure libraries: find the
wrong behavior, add a regression, make the smallest fix, and test both revisions.

## Failure cases

[Three reproducible cases](reproductions/README.md): json-c parser reset,
LibTomMath parent installation and PCRE2 whole-pattern recursion. Includes pinned
revisions, fixtures, licenses and fresh before/after logs on macOS arm64.
PCRE2 covers all three library widths with interpreter and JIT execution.

## Merged

No qualifying infrastructure patches merged yet.

## Submitted

These remain open. Test results refer to the tested revisions.

| Project | Fix | Local checks |
| --- | --- | --- |
| [c-ares #1326](https://github.com/c-ares/c-ares/pull/1326) | Parse compressed names in legacy DNS records; builds on afonsojanu's earlier approach. | Three baseline failures; 86 parser tests and 1,110 non-live IPv4/IPv6 tests pass. |
| [libffi #1029](https://github.com/libffi/libffi/pull/1029) | Correct Darwin ARM64 stack slots for small non-HFA structs, in calls and closures. | Independent baseline call/closure failures; 1,746 tests pass, five unsupported. |
| [PCRE2 #1036](https://github.com/PCRE2Project/pcre2/pull/1036) | Preserve backtracking needed by whole-pattern recursion. | Baseline mismatches across 8/16/32-bit interpreter/JIT; fixed CTest 4/4 passes. |
| [json-c #986](https://github.com/json-c/json-c/pull/986) | Clear a pending Unicode surrogate when resetting the parser. | Three baseline string failures; CMake 29/29 and Meson 27/27 pass. |
| [LibTomMath #612](https://github.com/libtom/libtommath/pull/612) | Find the library's manpage when installing through a parent project. | Baseline install fails; static/shared parent installs and native arithmetic tests pass. |

Checked October 11, 2026. Open submissions are separate from merged work.

# qeqx

[GitHub: anon5376](https://github.com/anon5376)

I work on small, reproducible failures in infrastructure libraries: find the
wrong behavior, add a regression, make the smallest fix, and test both revisions.

## Failure cases

[Six reproducible cases](reproductions/README.md): Hiredis async timeouts, json-c parser reset,
LibTomMath parent installation, PCRE2 whole-pattern recursion and libffi's
Darwin stack arguments, plus c-ares' compressed SRV targets. Includes pinned
revisions, fixtures, licenses and fresh before/after logs on macOS arm64.
PCRE2 covers all three library widths with interpreter and JIT execution.
libffi covers calls and closures against native controls on macOS arm64.
c-ares compares compressed and literal SRV targets through its public parser,
without sending DNS requests.
Hiredis checks pending command deadlines and runtime timeout changes through
its poll adapter and a silent local TCP peer; no Redis server is needed.

## Merged

No qualifying infrastructure patches merged yet.

## Submitted

These remain open. Test results refer to the tested revisions.

| Project | Fix | Local checks |
| --- | --- | --- |
| [Hiredis #1379](https://github.com/redis/hiredis/pull/1379) | Preserve pending command timeouts and libevent deadlines; apply runtime timeout changes. | 187 local tests pass; full fork CI 9/9 and paired regressions 5/5 pass. |
| [c-ares #1326](https://github.com/c-ares/c-ares/pull/1326) | Parse compressed names in legacy DNS records; builds on afonsojanu's earlier approach. | Three baseline failures; 86 parser tests and 1,110 non-live IPv4/IPv6 tests pass. |
| [libffi #1029](https://github.com/libffi/libffi/pull/1029) | Correct Darwin ARM64 stack slots for small non-HFA structs, in calls and closures. | Independent baseline call/closure failures; 1,746 tests pass, five unsupported. |
| [PCRE2 #1036](https://github.com/PCRE2Project/pcre2/pull/1036) | Preserve backtracking needed by whole-pattern recursion. | Baseline mismatches across 8/16/32-bit interpreter/JIT; fixed CTest 4/4 passes. |
| [json-c #986](https://github.com/json-c/json-c/pull/986) | Clear a pending Unicode surrogate when resetting the parser. | Three baseline string failures; CMake 29/29 and Meson 27/27 pass. |
| [LibTomMath #612](https://github.com/libtom/libtommath/pull/612) | Find the library's manpage when installing through a parent project. | Baseline install fails; static/shared parent installs and native arithmetic tests pass. |

Checked October 11, 2026. Open submissions are separate from merged work.

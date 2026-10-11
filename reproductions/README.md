# Infrastructure failure cases — qeqx

Six bugs, with pinned source revisions and clients that exercise the real
library or installation path. Each recipe builds into a fresh temporary folder.

| Case | Expected failure before the patch | Fixed behavior |
| --- | --- | --- |
| [Hiredis async timeout](hiredis-async-timeout/README.md) | Enqueueing keeps postponing a pending timeout. An intermediate PR revision also misses runtime timeout changes. | The deadline expires and every queued callback is notified; enabling and shortening the timeout work. |
| [json-c reset](json-c-reset/README.md) | A new document inherits an unfinished Unicode surrogate; the decoded string has extra bytes. | Reset clears that state; continuing without reset still preserves a surrogate pair. |
| [LibTomMath parent install](libtommath-parent/README.md) | Installation looks for the manpage in the parent project and fails. | The library's own manpage installs in both static and shared builds. |
| [PCRE2 recursion](pcre2-recursion/README.md) | Whole-pattern recursion returns the wrong substring in all three library widths, with interpreter and JIT execution. | All six fixture matches are correct in every mode; local subroutine controls are preserved. |
| [libffi Darwin stack](libffi-darwin-stack/README.md) | A mixed struct and following integer arrive with the wrong values through calls and closures. | Both public API modes receive the same values as native calls on macOS arm64. |
| [c-ares SRV target](c-ares-srv/README.md) | A received legacy-compressed SRV target is rejected; the literal control passes. | Both replies decode the same target, priority, weight and port through the public API. |

The linked upstream PRs are open. These cases record local behavior, not upstream
acceptance. The initial receipts use macOS arm64, Apple Clang 21 and CMake 4.4.3.
Other platforms have not been executed for this collection.

Requirements: Git, CMake, a C compiler and a build tool supported by CMake.
The recipes require no optional library dependencies. Source-fetch commands
need network access to GitHub; PCRE2 also fetches its pinned SLJIT submodule.
The libffi source build also needs Autoconf, Automake and Libtool; its
documentation build is disabled. Its probe requires native macOS arm64.
The Hiredis case uses loopback TCP and the standard poll adapter's event hooks;
it needs no Redis server or libevent and does not exercise the libevent adapter.

The probes and collection documentation use the MIT license in [LICENSE](LICENSE),
except the adapted LibTomMath fixture, which retains its Unlicense notice,
and PCRE2's public-domain testdata fixture.
Upstream source stays under each project's own license. Copies of those notices
are included beside the cases; this collection does not redistribute libraries
or build products.

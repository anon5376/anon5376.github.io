# Infrastructure failure cases — qeqx

Three small bugs, with pinned source revisions and clients that exercise the real
library or installation path. Each recipe builds into a fresh temporary folder.

| Case | Expected failure before the patch | Fixed behavior |
| --- | --- | --- |
| [json-c reset](json-c-reset/README.md) | A new document inherits an unfinished Unicode surrogate; the decoded string has extra bytes. | Reset clears that state; continuing without reset still preserves a surrogate pair. |
| [LibTomMath parent install](libtommath-parent/README.md) | Installation looks for the manpage in the parent project and fails. | The library's own manpage installs in both static and shared builds. |
| [PCRE2 recursion](pcre2-recursion/README.md) | Whole-pattern recursion returns the wrong substring in all three library widths, with interpreter and JIT execution. | All six fixture matches are correct in every mode; local subroutine controls are preserved. |

The linked upstream PRs are open. These cases record local behavior, not upstream
acceptance. The initial receipts use macOS arm64, Apple Clang 21 and CMake 4.4.3.
Other platforms have not been executed for this collection.

Requirements: Git, CMake, a C compiler and a build tool supported by CMake.
The recipes require no optional library dependencies. Source-fetch commands
need network access to GitHub; PCRE2 also fetches its pinned SLJIT submodule.

The probes and collection documentation use the MIT license in [LICENSE](LICENSE),
except the adapted LibTomMath fixture, which retains its Unlicense notice,
and PCRE2's public-domain testdata fixture.
Upstream source stays under each project's own license. Copies of those notices
are included beside the cases; this collection does not redistribute libraries
or build products.

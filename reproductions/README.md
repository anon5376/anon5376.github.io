# Infrastructure failure cases — qeqx

Two small bugs, with pinned source revisions and clients that exercise the real
library or installation path. Each recipe builds into a fresh temporary folder.

| Case | Expected failure before the patch | Fixed behavior |
| --- | --- | --- |
| [json-c reset](json-c-reset/README.md) | A new document inherits an unfinished Unicode surrogate; the decoded string has extra bytes. | Reset clears that state; continuing without reset still preserves a surrogate pair. |
| [LibTomMath parent install](libtommath-parent/README.md) | Installation looks for the manpage in the parent project and fails. | The library's own manpage installs in both static and shared builds. |

The linked upstream PRs are open. These cases record local behavior, not upstream
acceptance. The initial receipts use macOS arm64, Apple Clang 21 and CMake 4.4.3.
Other platforms have not been executed for this collection.

Requirements: Git, CMake, a C compiler and a build tool supported by CMake.
No dependencies are downloaded by the build recipes. The source-fetch commands
need network access to GitHub.

The probes and collection documentation use the MIT license in [LICENSE](LICENSE),
except the adapted LibTomMath fixture, which retains its Unlicense notice.
Upstream source stays under each project's own license. Copies of those notices
are included beside the cases; this collection does not redistribute libraries
or build products.

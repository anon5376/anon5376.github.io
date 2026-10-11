# json-c: reset must start a new document

[Upstream PR #986](https://github.com/json-c/json-c/pull/986)

Before: `9e99c1cfe77573496e9ea6da5153e4fd7e2cbde0`.
Fixed: `7662139501603434eba79aa7b916d1497d3d9cf4`.

The client feeds an unfinished `"\uD834` to a tokener, resets it, then parses
`"\u0041"` as a new document. The result should contain only byte `41`.
Before the patch it contains `EF BF BD 41`. Later checks also cover a lone low
surrogate after reset, reset following an invalid escape, and continuation
without reset. The client exits on the first incorrect result.

Run from the collection root. Change `revision` to the fixed SHA for the second
run. The PR ref is fetched so the unmerged fix is available; checkout uses the
exact SHA rather than whichever commit happens to be the latest PR head.

```sh
case_root="$PWD/json-c-reset"
work=$(mktemp -d)
revision=9e99c1cfe77573496e9ea6da5153e4fd7e2cbde0
git clone https://github.com/json-c/json-c.git "$work/source"
git -C "$work/source" fetch origin refs/pull/986/head
git -C "$work/source" checkout --detach "$revision"
cmake -S "$case_root" -B "$work/build" \
  -DJSON_C_SOURCE="$work/source" -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF -DBUILD_APPS=OFF
cmake --build "$work/build" --target json-c-reset-probe --parallel 4
"$work/build/json-c-reset-probe"
```

Expected client exit: `1` before the patch, `0` after it. Configure and build
must succeed in both runs; a build failure does not reproduce this bug.
The temporary folder is retained for inspection. See [receipts](../receipts/README.md)
for the executed results and source hashes.

The probe uses the public json-c API. It does not copy or replace parser code.
Its license is MIT; [UPSTREAM-COPYING](UPSTREAM-COPYING) records json-c's notices.

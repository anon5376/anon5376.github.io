# LibTomMath: install from a parent project

[Upstream PR #612](https://github.com/libtom/libtommath/pull/612)

Before: `ae40a87a920099a7d9d00979570e0c8d917a1fd7`.
Fixed: `a9e6bc630f7f241c503e4fe1e1189ed523feb2e6`.

The parent CMake project adds LibTomMath as a subdirectory, builds it and installs
it into a temporary prefix. Before the patch, installation searches for the
parent's absent `doc/tommath.3`. The patch uses the library's source directory.

This parent fixture was introduced by PR #612; it did not exist at the baseline.
The collection adapts it to take an explicit source-directory argument and runs
that same fixture against both revisions. No baseline production code is patched.

Run from the collection root. Change `revision` to the fixed SHA for the second
run, and repeat with `shared=ON` for shared-library coverage.

```sh
case_root="$PWD/libtommath-parent"
work=$(mktemp -d)
revision=ae40a87a920099a7d9d00979570e0c8d917a1fd7
shared=OFF
git clone https://github.com/libtom/libtommath.git "$work/source"
git -C "$work/source" fetch origin refs/pull/612/head
git -C "$work/source" checkout --detach "$revision"
cmake -S "$case_root" -B "$work/build" \
  -DLIBTOMMATH_SOURCE="$work/source" -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS="$shared" -DBUILD_TESTING=OFF \
  -DCMAKE_EXPORT_NO_PACKAGE_REGISTRY=ON \
  -DCMAKE_INSTALL_PREFIX="$work/install"
cmake --build "$work/build" --parallel 4
cmake --install "$work/build" --config Release
cmp "$work/source/doc/tommath.3" "$work/install/share/man/man3/tommath.3"
```

Configure and build must succeed for both revisions. Baseline install is expected
to fail; fixed install and the manpage comparison must pass. Stop the baseline
recipe at its expected install failure. This manpage path applies to macOS and
Linux; the collection has only been executed on macOS. Windows intentionally
skips manpage installation and is outside this case.

Release is explicit. A separate default-build-type/export problem is outside
this case. Package-registry export is disabled so the recipe does not write to
the user's global CMake registry. The temporary folder is retained. See [receipts](../receipts/README.md).
The adapted fixture retains [LibTomMath's Unlicense](UPSTREAM-LICENSE).

# libffi: mixed struct at the Darwin arm64 stack boundary

[Upstream PR #1029](https://github.com/libffi/libffi/pull/1029)

Before: `bc553867367246d140cd156f060bd0409f57f157`.
Fixed: `b83a92d0c237ef119eb1ee8d136a3d318466ca10`.

This public-API probe passes eight `long` arguments, an `int`, a 12-byte
`{float, float, int}` struct, and a final `int`. Run `--call` and `--closure`
separately: the first checks `ffi_call`, and the second checks a native call
into an `ffi_closure`. Each run first calls the same target directly as a native
ABI control. The probe prints received values, field mismatches, and the
expected and observed result.
The return value is an argument-validation marker: `1` for correct arguments,
`0` for a mismatch. No arithmetic is performed on corrupted floating values.

Run from the collection root. Change `revision` to the fixed SHA for the second
run, and start with a fresh temporary folder for each revision.

```sh
case_root="$PWD/libffi-darwin-stack"
work=$(mktemp -d)
revision=bc553867367246d140cd156f060bd0409f57f157
git clone https://github.com/libffi/libffi.git "$work/source"
git -C "$work/source" fetch origin refs/pull/1029/head
git -C "$work/source" checkout --detach "$revision"
(cd "$work/source" && ./autogen.sh)
mkdir "$work/build"
(cd "$work/build" && "$work/source/configure" --disable-docs --disable-shared \
  --enable-static --disable-multi-os-directory 'CFLAGS=-O2 -g')
make -C "$work/build" --jobs 4
cc -O2 -Wall -Wextra -Werror -I"$work/build/include" -I"$work/build" \
  "$case_root/probe.c" "$work/build/.libs/libffi.a" -o "$work/probe"
"$work/probe" --call
"$work/probe" --closure
```

Expected probe exit: `1` for a reproduced mismatch before the patch, `0` after
the fix, and `2` when setup is unsupported or the platform is outside native
macOS arm64. A compile or build failure is not a reproduction. The source
build's `autogen.sh` requires the Autoconf, Automake, Libtool, and Texinfo tools
listed in libffi's README. Documentation generation is disabled here.

The [executed receipts](../receipts/libffi-results.json) use fresh source
archives of both exact commits on macOS arm64. Both builds and probe compiles
passed; both baseline modes exited `1`, and both fixed modes exited `0`.
The native controls passed in all four runs. The clone/fetch recipe above was
not used for those builds; both commits were checked through the GitHub API.
Uninitialized padding can make some baseline values vary between runs.

The probe is licensed under this collection's MIT license. The copied
[upstream license](UPSTREAM-LICENSE) preserves libffi's license notice.

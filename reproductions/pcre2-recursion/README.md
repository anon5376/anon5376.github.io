# PCRE2: whole-pattern recursion changes the matched text

[Upstream PR #1036](https://github.com/PCRE2Project/pcre2/pull/1036)

Before: `8e42d9ae6ba8025ea8f5bdea2ca4d3c208460cb8`.
Fixed: `9c0fd64f59f70d8d97db603ae1033bf3b2e6cc64`.

Pattern `(?:b(?R)a|a+)` should match all of `baa`. Before the patch, an
optimization prevents necessary backtracking and returns `aa` instead.
The fixture also exercises nested recursion, `(?0)`, and separate local
subroutine calls. Expected group-zero matches, in order:

```text
baa
bbaaa
baaa
baa
xaa
bxaaa
```

Run from the collection root. Change `revision` to the fixed SHA for the second
build. `pcre2test` prints the matched text; it may exit 0 even when that text is
wrong, so compare the output with the six literal results above.

```sh
case_root="$PWD/pcre2-recursion"
work=$(mktemp -d)
revision=8e42d9ae6ba8025ea8f5bdea2ca4d3c208460cb8
git clone https://github.com/PCRE2Project/pcre2.git "$work/source"
git -C "$work/source" fetch origin refs/pull/1036/head
git -C "$work/source" checkout --detach "$revision"
git -C "$work/source" submodule update --init --recursive
cmake -S "$work/source" -B "$work/build" \
  -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DBUILD_STATIC_LIBS=ON \
  -DPCRE2_BUILD_PCRE2_8=ON -DPCRE2_BUILD_PCRE2_16=ON -DPCRE2_BUILD_PCRE2_32=ON \
  -DPCRE2_SUPPORT_JIT=ON -DPCRE2_BUILD_TESTS=ON -DPCRE2_BUILD_PCRE2GREP=OFF \
  -DPCRE2_SUPPORT_LIBREADLINE=OFF -DPCRE2_SUPPORT_LIBEDIT=OFF \
  -DPCRE2_SUPPORT_LIBBZ2=OFF -DPCRE2_SUPPORT_LIBZ=OFF
cmake --build "$work/build" --target pcre2test --parallel 4
for width in 8 16 32; do
  "$work/build/pcre2test" "-$width" "$case_root/cases.txt"
  "$work/build/pcre2test" "-$width" -jitverify "$case_root/cases.txt"
done
```

Before the patch, the four whole-pattern recursion results differ from the
expected list in every mode; the last two local-subroutine controls still pass.
After the patch, all six matches must equal the expected list in every mode.
With `-jitverify`, each group-zero result must also carry `(JIT)`, proving JIT
actually ran. Missing markers or a build error do not reproduce this defect.

These recipes enable JIT for the tested macOS arm64 host. A platform without
PCRE2 JIT support cannot execute the JIT half of the recipe. No optional
compression or line-editing dependencies are required. The submodule step
fetches PCRE2's required SLJIT source; both pinned revisions use SLJIT commit
`39c508d1b8fbae332d60c7450db20236913089da`.

The fixture is the regression block from `testdata/testinput5` at the fixed
revision above. PCRE2 places its testdata in the public domain. This README uses
the collection's MIT license. [UPSTREAM-LICENCE.md](UPSTREAM-LICENCE.md) retains
the full upstream terms, including the separate library and JIT licences.
Library source and binaries are not redistributed here.

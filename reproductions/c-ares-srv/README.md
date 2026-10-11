# c-ares: receive a compressed SRV target

[Upstream PR #1326](https://github.com/c-ares/c-ares/pull/1326)

Before: `f4156c12c8b36f5cb8f8a53658d44b24263c6ad1`.
Fixed: `966257ce9f321fcae3651d6bb1a84cdda3de1aff`.

Two offline DNS replies describe the same SRV answer: target `example.com`,
priority 10, weight 20, port 30. The literal control encodes the target as
labels; the other uses `c0 0c`, a pointer to the name in the question. Their
answer owner uses the same compression pointer in both replies. The client
uses only the public `ares_dns_parse` and record getter APIs, checking the
answer count, type, class and decoded fields.

The literal reply passes on both revisions. Before the patch, the compressed
target is rejected with `ARES_EBADNAME`; after it, all fields decode correctly.
This is receive-side compatibility: [RFC 2782](https://www.rfc-editor.org/rfc/rfc2782.html)
prohibits senders from compressing SRV targets, while
[RFC 3597 section 4](https://www.rfc-editor.org/rfc/rfc3597.html#section-4)
recommends accepting older compressed SRV records on receipt. This case does
not exercise the writer, SIG or NAPTR records, or live service discovery.
The patch builds on [afonsojanu's earlier design](https://github.com/c-ares/c-ares/pull/1290).

Run from the collection root. Change `revision` to the fixed SHA for the second
run, using a fresh temporary folder for each revision.

```sh
case_root="$PWD/c-ares-srv"
work=$(mktemp -d)
revision=f4156c12c8b36f5cb8f8a53658d44b24263c6ad1
git clone https://github.com/c-ares/c-ares.git "$work/source"
git -C "$work/source" fetch origin refs/pull/1326/head
git -C "$work/source" checkout --detach "$revision"
cmake -S "$case_root" -B "$work/build" \
  -DCARES_SOURCE="$work/source" -DCMAKE_BUILD_TYPE=Release \
  -DCARES_STATIC=ON -DCARES_SHARED=OFF -DCARES_INSTALL=OFF \
  -DCARES_BUILD_TESTS=OFF -DCARES_BUILD_TOOLS=OFF
cmake --build "$work/build" --target c-ares-srv-probe --parallel 4
"$work/build/c-ares-srv-probe" --literal
"$work/build/c-ares-srv-probe" --compressed
```

Expected exit: literal `0` on both revisions; compressed `1` before the patch
and `0` after it. Exit `1` indicates a decoded-field mismatch or the specific
`ARES_EBADNAME` rejection; it is expected only for the baseline compressed run.
Exit `2` indicates a usage, initialization or a different parse error.
Both configure and build must succeed.
No DNS requests, channels or sockets are created by this client. The temporary
folder is retained for inspection.

The probe uses this collection's MIT license. [UPSTREAM-LICENSE](UPSTREAM-LICENSE)
preserves c-ares' notices. [Executed receipts](../receipts/cares-results.json)
record two fresh clone/fetch builds and all four probe runs on macOS arm64.

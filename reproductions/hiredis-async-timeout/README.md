# hiredis: async command timeouts under sustained enqueue and runtime changes

This standalone POSIX C client reproduces three async timeout cases against a
silent loopback TCP peer. It exercises async timeout behavior through the
public command and timeout functions. The probe also reads public context
fields (`err`, connection flags and output buffer) and calls
the poll adapter's event hook to pause write dispatch. It is an external client
reproduction, not a public-API-only test. It does not test the libevent adapter
or claim Windows support.

The peer accepts the connection and never reads or replies. Each mode first
sends a `PING`, then polls until hiredis reports the connection established and
the output buffer empty, leaving its reply pending:

- `--enqueue` starts with a 200 ms command timeout, queues another `PING`,
  pauses write dispatch, and queues another `PING` every 10 ms for up to 750 ms.
  Write dispatch is paused again after each enqueue.
- `--enable` starts without a command timeout, applies a 200 ms timeout with
  `redisAsyncSetTimeout` while the initial reply is pending, queues one `PING`,
  and pauses write dispatch.
- `--shorten` starts with a 3 s timeout, changes it to 200 ms while the first
  reply is pending, queues one `PING`, and pauses write dispatch.

Each mode polls for at most 750 ms after its trigger. Exit `0` means the
connection reports `REDIS_ERR_TIMEOUT`, at least two callbacks were queued,
every callback completed with the timeout error, no replies arrived, and
unsent output remains. Exit `1` means this contract was not observed; exit `2`
means usage or setup failed. The output prints actual elapsed microseconds,
error code, queued/timeouts/replies and unsent byte count. The listener accept
wait is bounded to three seconds.

Expected exit codes:

| Revision | `--enqueue` | `--enable` | `--shorten` |
| --- | ---: | ---: | ---: |
| Baseline `058ebcdf36c5b05b759c613564fb958ec363c90f` | 1 | 0 | 0 |
| Our initial PR #1379 patch `c4399445d86de001cbc1adc50782b3fb5c918736` | 0 | 1 | 1 |
| Fixed revision `47089f79890783f8ab6f585878a50c4e1f5d9650` | 0 | 0 | 0 |

The intermediate row is our own initial patch, not an upstream release. It
fixed sustained enqueue but failed to arm or shorten a pending timeout when
the timeout was changed at runtime. The fixed revision addresses those
regressions as well. [Executed receipts](../receipts/hiredis-results.json)
record fresh source checkouts, three static builds and all nine expected
outcomes on macOS arm64.

Run each revision from the collection root in a fresh source checkout and
build directory. The branch fetch makes the PR ref available; checkout pins
the exact revision. No upstream source patch is applied.

```sh
set -eu
case_root="$PWD/hiredis-async-timeout"
revision=058ebcdf36c5b05b759c613564fb958ec363c90f
work=$(mktemp -d)
git clone https://github.com/redis/hiredis.git "$work/source"
git -C "$work/source" fetch origin refs/pull/1379/head
git -C "$work/source" checkout --detach "$revision"
cmake -S "$case_root" -B "$work/build" \
  -DHIREDIS_SOURCE="$work/source" -DCMAKE_BUILD_TYPE=Release
cmake --build "$work/build" --target hiredis-async-timeout-probe --parallel 4
for mode in --enqueue --enable --shorten; do
  case "$revision:$mode" in
    058ebcdf36c5b05b759c613564fb958ec363c90f:--enqueue) expected=1 ;;
    c4399445d86de001cbc1adc50782b3fb5c918736:--enable|c4399445d86de001cbc1adc50782b3fb5c918736:--shorten) expected=1 ;;
    *) expected=0 ;;
  esac
  if "$work/build/hiredis-async-timeout-probe" "$mode"; then actual=0; else actual=$?; fi
  printf '%s expected-exit=%s actual-exit=%s\n' "$mode" "$expected" "$actual"
  test "$actual" -eq "$expected" || exit 1
done
```

Repeat with a fresh `work` directory and `revision` set to the intermediate
and fixed SHAs. The probe does not install hiredis, run its upstream test
suite, or require Redis or libevent. Its listener accepts only the local probe
connection.

The probe uses the collection's MIT license. [UPSTREAM-LICENSE](UPSTREAM-LICENSE)
contains hiredis' upstream license text.

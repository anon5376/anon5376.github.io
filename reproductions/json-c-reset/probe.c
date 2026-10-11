/* SPDX-License-Identifier: MIT */
#include <stdio.h>
#include <string.h>
#include <json.h>
#include <json_tokener.h>

static int expect_bytes(const char *label, struct json_object *obj,
                        const unsigned char *expected, int expected_len)
{
    const char *actual;
    int actual_len;
    int i;

    if (obj == NULL || !json_object_is_type(obj, json_type_string)) {
        fprintf(stderr, "%s: result is not a JSON string\n", label);
        return 1;
    }
    actual = json_object_get_string(obj);
    actual_len = json_object_get_string_len(obj);
    if (actual_len != expected_len || memcmp(actual, expected, (size_t)expected_len) != 0) {
        fprintf(stderr, "%s: expected length %d bytes", label, expected_len);
        for (i = 0; i < expected_len; i++) fprintf(stderr, " %02X", expected[i]);
        fprintf(stderr, "; got length %d bytes", actual_len);
        for (i = 0; i < actual_len; i++) fprintf(stderr, " %02X", (unsigned char)actual[i]);
        fputc('\n', stderr);
        return 1;
    }
    printf("PASS %s: length=%d bytes", label, actual_len);
    for (i = 0; i < actual_len; i++) printf(" %02X", (unsigned char)actual[i]);
    putchar('\n');
    return 0;
}

static int incomplete_high(struct json_tokener *tok)
{
    static const char input[] = "\"\\uD834";
    struct json_object *obj = json_tokener_parse_ex(tok, input, (int)(sizeof(input) - 1));
    enum json_tokener_error err = json_tokener_get_error(tok);
    if (obj != NULL || err != json_tokener_continue) {
        fprintf(stderr, "incomplete high surrogate: expected NULL + continue, got obj=%p error=%s\n",
                (void *)obj, json_tokener_error_desc(err));
        if (obj) json_object_put(obj);
        return 1;
    }
    printf("PASS incomplete high surrogate: object=NULL error=%s\n", json_tokener_error_desc(err));
    return 0;
}

static int finish(struct json_tokener *tok, const char *label,
                  const char *input, const unsigned char *expected, int expected_len)
{
    struct json_object *obj = json_tokener_parse_ex(tok, input, (int)strlen(input) + 1);
    enum json_tokener_error err = json_tokener_get_error(tok);
    int result;
    if (err != json_tokener_success || obj == NULL) {
        fprintf(stderr, "%s: expected success, got %s (object=%p)\n",
                label, json_tokener_error_desc(err), (void *)obj);
        if (obj) json_object_put(obj);
        return 1;
    }
    result = expect_bytes(label, obj, expected, expected_len);
    json_object_put(obj);
    return result;
}

int main(void)
{
    static const unsigned char a[] = {0x41};
    static const unsigned char replacement[] = {0xEF, 0xBF, 0xBD};
    static const unsigned char musical_symbol[] = {0xF0, 0x9D, 0x84, 0x9E};
    struct json_tokener *tok = json_tokener_new();
    struct json_object *obj;
    enum json_tokener_error err;
    int failed = 0;
    if (tok == NULL) { fputs("cannot allocate tokener\n", stderr); return 2; }

    if (incomplete_high(tok)) { failed = 1; goto done; }
    json_tokener_reset(tok);
    if (finish(tok, "reset then U+0041", "\"\\u0041\"", a, sizeof(a))) { failed = 1; goto done; }

    if (incomplete_high(tok)) { failed = 1; goto done; }
    json_tokener_reset(tok);
    if (finish(tok, "reset then lone low surrogate", "\"\\uDD1E\"", replacement, sizeof(replacement))) { failed = 1; goto done; }

    if (incomplete_high(tok)) { failed = 1; goto done; }
    obj = json_tokener_parse_ex(tok, "\\uZZZZ\"", 8);
    err = json_tokener_get_error(tok);
    if (obj != NULL || err == json_tokener_continue || err == json_tokener_success) {
        fprintf(stderr, "invalid escape after high surrogate: expected fatal error, got %s (object=%p)\n",
                json_tokener_error_desc(err), (void *)obj);
        if (obj) json_object_put(obj);
        failed = 1;
        goto done;
    }
    printf("PASS invalid escape after high surrogate: object=NULL error=%s\n", json_tokener_error_desc(err));
    json_tokener_reset(tok);
    if (finish(tok, "fatal error reset then U+0041", "\"\\u0041\"", a, sizeof(a))) { failed = 1; goto done; }

    if (incomplete_high(tok)) { failed = 1; goto done; }
    if (finish(tok, "continued high+low surrogate", "\\uDD1E\"", musical_symbol, sizeof(musical_symbol))) { failed = 1; goto done; }

done:
    json_tokener_free(tok);
    return failed ? 1 : 0;
}

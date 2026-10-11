/* SPDX-License-Identifier: MIT */
#include <stdio.h>
#include <string.h>
#include <ares.h>
#include <ares_dns_record.h>

/* Both replies have one SRV answer: example.com, priority 10, weight 20,
 * port 30. Only the RDATA length and target encoding differ. The answer
 * owner uses the same compression pointer in both replies. */
static const unsigned char literal[] = {
    0x12, 0x34, 0x81, 0x80, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00,
    0x07, 'e', 'x', 'a', 'm', 'p', 'l', 'e',
    0x03, 'c', 'o', 'm', 0x00, 0x00, 0x21, 0x00, 0x01,
    0xc0, 0x0c, 0x00, 0x21, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x3c, 0x00, 0x13,
    0x00, 0x0a, 0x00, 0x14, 0x00, 0x1e,
    0x07, 'e', 'x', 'a', 'm', 'p', 'l', 'e',
    0x03, 'c', 'o', 'm', 0x00
};

static const unsigned char compressed[] = {
    0x12, 0x34, 0x81, 0x80, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00,
    0x07, 'e', 'x', 'a', 'm', 'p', 'l', 'e',
    0x03, 'c', 'o', 'm', 0x00, 0x00, 0x21, 0x00, 0x01,
    0xc0, 0x0c, 0x00, 0x21, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x3c, 0x00, 0x08,
    0x00, 0x0a, 0x00, 0x14, 0x00, 0x1e, 0xc0, 0x0c
};

static int check_reply(const char *label, const unsigned char *packet,
                       size_t length)
{
    ares_dns_record_t *record = NULL;
    const ares_dns_rr_t *rr;
    const char *target;
    ares_status_t status;
    unsigned short priority, weight, port;
    int result = 1;

    status = ares_dns_parse(packet, length, 0, &record);
    printf("%s: parse status=%d (%s)\n", label, (int)status,
           ares_strerror((int)status));
    if (status != ARES_SUCCESS) {
        ares_dns_record_destroy(record);
        return status == ARES_EBADNAME ? 1 : 2;
    }
    if (ares_dns_record_rr_cnt(record, ARES_SECTION_ANSWER) != 1) {
        fprintf(stderr, "%s: expected exactly one answer\n", label);
        goto done;
    }
    rr = ares_dns_record_rr_get_const(record, ARES_SECTION_ANSWER, 0);
    if (rr == NULL || ares_dns_rr_get_type(rr) != ARES_REC_TYPE_SRV ||
        ares_dns_rr_get_class(rr) != ARES_CLASS_IN) {
        fprintf(stderr, "%s: expected an IN SRV answer\n", label);
        goto done;
    }
    target = ares_dns_rr_get_str(rr, ARES_RR_SRV_TARGET);
    priority = ares_dns_rr_get_u16(rr, ARES_RR_SRV_PRIORITY);
    weight = ares_dns_rr_get_u16(rr, ARES_RR_SRV_WEIGHT);
    port = ares_dns_rr_get_u16(rr, ARES_RR_SRV_PORT);
    printf("%s: target=%s priority=%u weight=%u port=%u\n", label,
           target == NULL ? "<NULL>" : target, (unsigned int)priority,
           (unsigned int)weight, (unsigned int)port);
    if (target == NULL || strcmp(target, "example.com") != 0 ||
        priority != 10 || weight != 20 || port != 30) {
        fprintf(stderr, "%s: decoded SRV fields differ from the fixture\n",
                label);
        goto done;
    }
    printf("PASS %s\n", label);
    result = 0;
done:
    ares_dns_record_destroy(record);
    return result;
}

int main(int argc, char **argv)
{
    int status, result;
    int use_compressed;

    if (argc != 2 || (strcmp(argv[1], "--literal") != 0 &&
                      strcmp(argv[1], "--compressed") != 0)) {
        fprintf(stderr, "usage: %s --literal|--compressed\n", argv[0]);
        return 2;
    }
    use_compressed = strcmp(argv[1], "--compressed") == 0;
    status = ares_library_init(ARES_LIB_INIT_ALL);
    if (status != ARES_SUCCESS) {
        fprintf(stderr, "library init: %s\n", ares_strerror(status));
        return 2;
    }
    if (use_compressed) {
        result = check_reply("compressed target", compressed,
                             sizeof(compressed));
    } else {
        result = check_reply("literal target", literal, sizeof(literal));
    }
    ares_library_cleanup();
    return result;
}

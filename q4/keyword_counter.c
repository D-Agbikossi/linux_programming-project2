#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "keyword_counter.h"

// Count occurrences (including overlapping) of `keyword` in the file stream.
// Matches spanning a chunk boundary are handled via a small "tail" buffer.
size_t count_keyword_in_file(const char *file_path, const char *keyword, size_t klen)
{
    if (klen == 0)
        return 0;

    FILE *f = fopen(file_path, "rb");
    if (!f)
    {
        fprintf(stderr, "Failed to open %s: %s\n", file_path, strerror(errno));
        exit(EXIT_FAILURE);
    }

    const size_t CHUNK = 64 * 1024;
    char chunk[CHUNK];

    size_t tail_cap = klen > 0 ? (klen - 1) : 0;
    char *tail = tail_cap > 0 ? (char *)malloc(tail_cap) : NULL;
    if (tail_cap > 0 && !tail)
    {
        fprintf(stderr, "malloc(tail) failed\n");
        exit(EXIT_FAILURE);
    }

    char *combined = (char *)malloc(tail_cap + CHUNK);
    if (!combined)
    {
        fprintf(stderr, "malloc(combined) failed\n");
        exit(EXIT_FAILURE);
    }

    size_t tail_len = 0;
    size_t count = 0;

    while (1)
    {
        size_t nread = fread(chunk, 1, CHUNK, f);
        if (nread == 0)
        {
            if (ferror(f))
            {
                fprintf(stderr, "fread failed for %s\n", file_path);
                exit(EXIT_FAILURE);
            }
            break; // EOF
        }

        size_t combined_len = tail_len + nread;
        if (tail_len > 0)
            memcpy(combined, tail, tail_len);
        memcpy(combined + tail_len, chunk, nread);

        // Only scan when we have enough bytes to match the full keyword.
        if (combined_len >= klen)
        {
            for (size_t i = 0; i + klen <= combined_len; i++)
            {
                if (memcmp(combined + i, keyword, klen) == 0)
                    count++;
            }
        }

        // Update tail for next loop.
        if (klen > 1)
        {
            size_t new_tail_len = combined_len < tail_cap ? combined_len : tail_cap;
            if (new_tail_len > 0)
                memcpy(tail, combined + (combined_len - new_tail_len), new_tail_len);
            tail_len = new_tail_len;
        }
        else
        {
            tail_len = 0;
        }
    }

    free(combined);
    free(tail);
    fclose(f);
    return count;
}


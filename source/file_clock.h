#ifndef MP3PATCH_FILE_CLOCK_H
#define MP3PATCH_FILE_CLOCK_H
/* File sources send the usual positive granule edges. Negative row strobes give
   the file writer exact capture times across unrelated host and source rates. */
static inline int file_row_marker(int row) {
    if (!row)
        return 2;
    return row % 18 ? -(row % 18 + 1) : 1;
}
#endif

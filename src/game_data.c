#include "draft_signatures.h"
#include "psx_spu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static sint32 stream_sector_origin;

static void data_failure(const char *path)
{
    xport_message_error("Demolition Racer: DATA", path);
    abort();
}

static void data_path(char destination[256], const char *path)
{
    size_t length = 5;
    strcpy(destination, "DATA/");
    if (!strncmp(path, "cdrom:", 6))
        path += 6;
    while (*path == '/' || *path == '\\')
        ++path;
    while (*path && *path != ';')
    {
        char value = *path++;
        if (length >= 255 || value == ':' || (value == '.' && *path == '.'))
            data_failure("Invalid DATA path");
        destination[length++] = value == '\\' ? '/' : value;
    }
    destination[length] = 0;
}

static FILE *data_open(const char *path, char resolved[256], long *length)
{
    FILE *file;
    data_path(resolved, path);
    file = xport_fopen(resolved, "rb");
    if (!file)
        data_failure(resolved);
    if (fseek(file, 0, SEEK_END) || (*length = ftell(file)) < 0 || fseek(file, 0, SEEK_SET))
    {
        fclose(file);
        data_failure(resolved);
    }
    return file;
}

uint32 dr_data_read(const char *path, uint32 destination, uint32 capacity)
{
    char resolved[256];
    long length;
    FILE *file = data_open(path, resolved, &length);
    uint32 bytes = (uint32)length;
    if (bytes > capacity || fread(psx_addr(destination, bytes), 1, bytes, file) != bytes)
    {
        fclose(file);
        data_failure(resolved);
    }
    fclose(file);
    return bytes;
}

uint32 dr_data_catalog(void)
{
    char resolved[256];
    long length;
    uint32 index, record;
    FILE *file = data_open("DD.IDX", resolved, &length);
    if (length < 20480 || fread(psx_addr(0x800839D4, 20480), 1, 20480, file) != 20480)
    {
        fclose(file);
        data_failure(resolved);
    }
    fclose(file);
    w_u32(0x80065180, 0);
    for (index = 0; index < 512; ++index)
    {
        record = 0x800839D4 + index * 40;
        if (!r_u8(record))
            return index * 40;
        if (!memchr(psx_addr(record, 32), 0, 32))
            data_failure("Invalid DATA catalog entry");
        /* Retain archive-relative sector metadata for legacy size lookups */
        w_u32(record + 36, r_u32(record + 36) + 10);
        w_u32(0x80065180, index + 1);
    }
    return 512;
}

CdlFILE *dr_data_stream(CdlFILE *result, const char *path)
{
    char resolved[256];
    uint8 header[12], chunk[8], position[4];
    long length;
    uint32 data_offset = 0, data_bytes;
    FILE *file = data_open(path, resolved, &length);
    data_bytes = (uint32)length;
    if (fread(header, 1, sizeof(header), file) != sizeof(header))
        goto invalid;
    if (!memcmp(header, "RIFF", 4) && !memcmp(header + 8, "CDXA", 4))
    {
        uint32 cursor = 12;
        while (cursor <= (uint32)length && (uint32)length - cursor >= 8)
        {
            uint32 bytes;
            if (fseek(file, (long)cursor, SEEK_SET) || fread(chunk, 1, 8, file) != 8)
                goto invalid;
            memcpy(&bytes, chunk + 4, sizeof(bytes));
            if (bytes > (uint32)length - cursor - 8)
                goto invalid;
            if (!memcmp(chunk, "data", 4))
            {
                data_offset = cursor + 8;
                data_bytes = bytes;
                break;
            }
            cursor += 8 + bytes + (bytes & 1);
        }
        if (!data_offset)
            goto invalid;
    }
    if (data_bytes < 2352 || data_bytes % 2352 ||
        fseek(file, (long)(data_offset + 12), SEEK_SET) || fread(position, 1, 4, file) != 4 || position[3] != 2)
        goto invalid;
    stream_sector_origin = CdPosToInt((CdlLOC *)position);
    fclose(file);
    if (!cd_mount_stream_file(resolved))
        data_failure(resolved);
    memset(result, 0, sizeof(*result));
    CdIntToPos(0, &result->pos);
    result->size = data_bytes / 2352 * 2048;
    return result;
invalid:
    fclose(file);
    data_failure(resolved);
    return NULL;
}

void dr_data_stream_position(uint8 *position)
{
    /* Extracted sectors retain disc MSF headers; game seeks are file-relative */
    sint32 sector = CdPosToInt((CdlLOC *)position) - stream_sector_origin;
    CdIntToPos(sector, (CdlLOC *)position);
}

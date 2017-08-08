#include "AppCore.h"

#include "PSDGzip.h"
#include "zlib.h"


namespace PoseidonCore
{

PSDGzip::PSDGzip()
{
}

PSDGzip::~PSDGzip()
{
}

ngx_int_t PSDGzip::Deflate(u_char *inData, size_t ilen, u_char *outData, size_t &olen)
{
    z_stream c_stream; /* compression stream */
    int err;

    c_stream.zalloc = (alloc_func)0;
    c_stream.zfree = (free_func)0;
    c_stream.opaque = (voidpf)0;

    err = deflateInit(&c_stream, Z_BEST_SPEED);
    if (err != Z_OK) {
        PSDError("deflateInit error");
        return NGX_ERROR;
    }

    c_stream.next_out = outData;
    c_stream.avail_out = (uInt)olen;

    
    c_stream.next_in = inData;
    c_stream.avail_in = (uInt)ilen;

    do {
        err = deflate(&c_stream, Z_NO_FLUSH);
        if (err == Z_STREAM_ERROR) {
            PSDError("deflate error:%d", err);
            deflateEnd(&c_stream);
            return NGX_ERROR;
        }
    }
    while (c_stream.avail_in != 0);

    for (;;) {
        err = deflate(&c_stream, Z_FINISH);
        if (err == Z_STREAM_END) {
            break;
        }
        if (err != Z_OK) {
            PSDError("deflate error:%d", err);
            deflateEnd(&c_stream);
            return NGX_ERROR;
        }
    }
    
    err = deflateEnd(&c_stream);
    if (err != Z_OK) {
        PSDError("deflateEnd error");
        return NGX_ERROR;
    }

    olen = c_stream.total_out;
    return NGX_OK;
}

ngx_int_t PSDGzip::Inflate(u_char *inData, size_t ilen, u_char *outData, size_t &olen)
{
    int err;
    z_stream d_stream; /* decompression stream */

    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;

    d_stream.next_in  = inData;
    d_stream.avail_in = (uInt)ilen;

    err = inflateInit(&d_stream);
    if (err != Z_OK) {
        PSDError("inflateInit error");
        return NGX_ERROR;
    }

    do {
        d_stream.next_out = outData;            /* discard the output */
        d_stream.avail_out = (uInt)olen;
        err = inflate(&d_stream, Z_NO_FLUSH);
        if (err == Z_STREAM_END) break;
        if (err != Z_OK) {
            PSDError("inflate error %d", err);
            inflateEnd(&d_stream);
            return NGX_ERROR;
        }
    } while(d_stream.avail_in != 0);

    err = inflateEnd(&d_stream);
    if (err != Z_OK) {
        PSDError("inflateEnd err %d", err);
    }
    olen = d_stream.total_out;
    return NGX_OK;
}

} // namespace PoseidonCore


#include "ge_internal.h"
#include <zlib.h>

#define CHUNK 16384
static void zlib_inflate(ge_Buffer* out, ge_Buffer* in, bool gzip);
static void zlib_deflate(ge_Buffer* out, ge_Buffer* in, bool gzip);

ge_Buffer* geDecompress(void* in, int insz, int mode){
	ge_Buffer* out = (ge_Buffer*)geMalloc(sizeof(ge_Buffer));
	if(mode == GE_COMPRESSION_ZLIB){
		ge_Buffer b_in = { in, insz, 0 };
		zlib_inflate(out, &b_in, false);
	}
	if(mode == GE_COMPRESSION_GZIP){
		ge_Buffer b_in = { in, insz, 0 };
		zlib_inflate(out, &b_in, true);
	}
	return out;
}

ge_Buffer* geCompress(void* in, int insz, int mode){
	ge_Buffer* out = (ge_Buffer*)geMalloc(sizeof(ge_Buffer));
	if(mode == GE_COMPRESSION_ZLIB){
		ge_Buffer b_in = { in, insz, 0 };
		zlib_deflate(out, &b_in, false);
	}
	return out;
}

static void zlib_inflate(ge_Buffer* b_out, ge_Buffer* b_in, bool gzip){
	int ret;
	unsigned have;
	z_stream strm;
	unsigned char in[CHUNK];
	unsigned char out[CHUNK];

	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	if(gzip){
		ret = inflateInit2(&strm, 16 + MAX_WBITS);
	}else{
		ret = inflateInit(&strm);
	}
	if (ret != Z_OK){
		return;
	}

	/* decompress until deflate stream ends or end of file */
	do {
		strm.avail_in = geBufferRead(b_in, in, CHUNK);
		if (strm.avail_in == 0){
			break;
		}
		strm.next_in = in;

		/* run inflate() on input until output buffer not full */
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			switch (ret) {
				case Z_NEED_DICT:
					gePrintDebug(0x102, "ZLIB: Z_NEED_DICT\n");
					ret = Z_DATA_ERROR;	 /* and fall through */
				case Z_DATA_ERROR:
					gePrintDebug(0x102, "ZLIB: Z_DATA_ERROR\n");
				case Z_MEM_ERROR:
					gePrintDebug(0x102, "ZLIB: Z_MEM_ERROR\n");
					(void)inflateEnd(&strm);
					return;
			}
			have = CHUNK - strm.avail_out;
			if (geBufferWrite(b_out, out, have) != have) {
				(void)inflateEnd(&strm);
				return;
			}
		} while (strm.avail_out == 0);

		/* done when inflate() says it's done */
	} while (ret != Z_STREAM_END);

	/* clean up and return */
	(void)inflateEnd(&strm);
}

static void zlib_deflate(ge_Buffer* b_out, ge_Buffer* b_in, bool gzip){
	int ret, flush;
	unsigned have;
	z_stream strm;
	unsigned char in[CHUNK];
	unsigned char out[CHUNK];

	/* allocate deflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
	if(gzip){
		//TODO
		//ret = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, 16 + MAX_WBITS);
	}else{
		ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
	}
	if (ret != Z_OK){
		return;
	}

	/* compress until end of file */
	do {
		strm.avail_in = geBufferRead(b_in, in, CHUNK);
		if (strm.avail_in < 0){
			break;
		}
		flush = (strm.avail_in == 0) ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in = in;

		/* run deflate() on input until output buffer not full, finish
		   compression if all of source has been read in */
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = deflate(&strm, flush);	/* no bad return value */
			have = CHUNK - strm.avail_out;
			if(geBufferWrite(b_out, out, have) != have){
				(void)deflateEnd(&strm);
				return;
			}
		} while (strm.avail_out == 0);

		/* done when last data in file processed */
	} while (flush != Z_FINISH);

	/* clean up and return */
	(void)deflateEnd(&strm);
}

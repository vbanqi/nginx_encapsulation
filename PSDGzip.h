#ifndef __POSEIDONCORE_P_S_D_GZIP_H_INCLUDED__
#define __POSEIDONCORE_P_S_D_GZIP_H_INCLUDED__


namespace PoseidonCore
{

class PSDGzip
{
public:
	PSDGzip();
	~PSDGzip();


    ngx_int_t Deflate(u_char *inData, size_t ilen, u_char *outData, size_t &olen);

    ngx_int_t Inflate(u_char *inData, size_t ilen, u_char *outData, size_t &olen);

private:
}; // class PSDGzip

} // namespace PoseidonCore

#endif // ifndef __POSEIDONCORE_P_S_D_GZIP_H_INCLUDED__


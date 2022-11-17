#include <ap_int.h>

#include "xf_security/sha224_256.hpp"

#define M_SZ 8

void DUT_FUNC( //
    hls::stream<ap_uint<8 * M_SZ> >& msg_strm,
    hls::stream<ap_uint<64> >& len_strm,
    hls::stream<bool>& end_len_strm,
    //
    hls::stream<ap_uint<256> >& hash_strm,
    hls::stream<bool>& end_hash_strm) {

#pragma HLS INTERFACE mode=axis port=msg_strm
#pragma HLS INTERFACE mode=axis port=len_strm
#pragma HLS INTERFACE mode=axis port=end_len_strm
#pragma HLS INTERFACE mode=axis port=hash_strm
#pragma HLS INTERFACE mode=axis port=end_hash_strm
#pragma HLS INTERFACE mode=s_axilite port=return

    xf::security::sha256(msg_strm, len_strm, end_len_strm, hash_strm, end_hash_strm);
}

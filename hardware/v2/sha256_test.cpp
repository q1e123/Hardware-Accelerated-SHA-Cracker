#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <ap_int.h>

#include "xf_security/sha224_256.hpp"

// For verification
#include <fstream>
#define NUM_TESTS 1000

#define M_SZ 8

void load_string(int len, char* in,
		hls::stream<ap_uint<8 * M_SZ> >& msg_strm,
		hls::stream<ap_uint<64> >& len_strm,
		hls::stream<bool>& end_len_strm) {
	len_strm.write(len);
	end_len_strm.write(false);

	ap_uint<8 * M_SZ> m;
	int n = 0;
	for (std::string::size_type i = 0; i < len; ++i) {
#pragma HLS LOOP_TRIPCOUNT min = 1024 max = 1024
		if (n == 0) {
			m = 0;
		}
		m.range(7 + 8 * n, 8 * n) = (unsigned)(in[i]);
		++n;
		if (n == M_SZ) {
			msg_strm.write(m);
			n = 0;
		}
	}
	if (n != 0) {
		msg_strm.write(m);
	}

	end_len_strm.write(true);
}

void write_result(char out[32],
		hls::stream<ap_uint<256> >& hash_strm,
	    hls::stream<bool>& end_hash_strm) {

    ap_uint<256> h256 = hash_strm.read();
    bool x = end_hash_strm.read();

	for (int i = 0; i < 32; ++i) {
		out[i] = (unsigned char)(h256.range(7 + 8 * i, 8 * i).to_int() & 0xff);
	}
	x = end_hash_strm.read();
}

void hash(int text_length, char* text_input, char result[32]){
#pragma HLS INTERFACE m_axi depth=1024 port=text_input
#pragma HLS INTERFACE s_axilite port=text_length
#pragma HLS INTERFACE m_axi depth=32 port=result
#pragma HLS INTERFACE s_axilite port=return

    static hls::stream<ap_uint<8 * M_SZ> > msg_strm("msg_strm");
    static hls::stream<ap_uint<64> > len_strm("len_strm");
    static hls::stream<bool> end_len_strm("end_len_strm");
    static hls::stream<ap_uint<256> > hash_strm("hash_strm");
    static hls::stream<bool> end_hash_strm("end_hash_strm");

#pragma HLS DATAFLOW
	load_string(text_length, text_input, msg_strm, len_strm, end_len_strm);
	xf::security::sha256(msg_strm, len_strm, end_len_strm, hash_strm, end_hash_strm);
	write_result(result, hash_strm, end_hash_strm);
}

void printSha(char sha[32]) {
	int i;
	for (i = 0; i < 32; i++)
	{
	    printf("%02X", sha[i]);
	}
	printf("\n");
}

int main(int argc, const char* argv[]) {
	const char message[] =
	        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopqabcdabcd"
	        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopqabcdabc";
	char sha[32];
	int nerror = 0;
	int ncorrect = 0;

	std::ifstream ifile;
	ifile.open("gld.dat");
	/* these values can be generated with
	 *   echo -n "abc" | sha256sum,
	 * where -n prevents echo to add \n after abc.
	 */

	for (int i = 0; i < NUM_TESTS; ++i) {
		int len = i % 128;
		char m[128];
		if (len != 0) memcpy(m, message, len);
		m[len] = 0;
		char h[32];
		// SHA256((const unsigned char*)message, len, (unsigned char*)h);
		ifile.read((char*)h, 32);

		hash(len, m, sha);

		if (memcmp(h, sha, 32)) {
			++nerror;
			std::cout << "\nmessage: \"" << m << "\"(" << len << ")\n";
			std::cout << "return: "; printSha(sha);
			std::cout << "golden: "; printSha(h);
		} else {
			++ncorrect;
		}
	}
	ifile.close();

	if (nerror) {
		std::cout << "\nFAIL: " << nerror << " errors found.\n";
	} else {
		std::cout << "\nPASS: " << ncorrect << " inputs verified, no error found.\n";
	}
	return nerror;
}

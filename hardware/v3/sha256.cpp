#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define AP_INT_MAX_W 4096
#include <ap_int.h>
#include "ap_axi_sdata.h"


#include "xf_security/sha224_256.hpp"

// For verification
#include <fstream>
#define NUM_TESTS 1000

#define M_SZ 8

#define _STR_CON(a, b) a##b
#define STR_CON(a, b) _STR_CON(a, b)

void output(hls::stream<ap_uint<256> >& hash_strm_in,
        hls::stream<bool>& end_hash_strm,
		hls::stream<ap_axiu<256, 0, 0, 0> >& hash_strm_out) {
	bool last = end_hash_strm.read();
	while(1) {
		bool current = end_hash_strm.read();

		ap_axiu<256, 0, 0, 0> data;
		data.data = hash_strm_in.read();
		data.keep = 0xffffffff;
		data.last = last^current;
		hash_strm_out.write(data);

		if(current)
			break;
		last = current;
	}
	//ap_axiu<256, 0, 0, 0> data;
	//data.last = 1;
	//hash_strm_out.write(data);
}

void DUT_FUNC( //
    hls::stream<ap_uint<8 * M_SZ> >& msg_strm,
    hls::stream<ap_uint<64> >& len_strm,
    hls::stream<bool>& end_len_strm,
    //
    hls::stream<ap_axiu<256, 0, 0, 0> >& hash_strm){

#pragma HLS INTERFACE mode=axis port=msg_strm
#pragma HLS INTERFACE mode=axis port=len_strm
#pragma HLS INTERFACE mode=axis port=end_len_strm
#pragma HLS INTERFACE mode=axis port=hash_strm
//#pragma HLS INTERFACE mode=axis port=end_hash_strm
#pragma HLS INTERFACE mode=s_axilite port=return

    static hls::stream<ap_uint<256> > hash_strm_int("hash_strm_int");
    static hls::stream<bool> end_hash_strm("end_hash_strm");

#pragma HLS DATAFLOW
    xf::security::sha256(msg_strm, len_strm, end_len_strm, hash_strm_int, end_hash_strm);
    output(hash_strm_int, end_hash_strm, hash_strm);
}

struct Test {
    std::string msg;
    unsigned char h256[32];
    Test(const char* m, const void* h) : msg(m) { memcpy(h256, h, 32); }
};

std::string hash2str(char* h, int len) {
    std::ostringstream oss;
    std::string retstr;

    // check output
    oss.str("");
    oss << std::hex;
    for (int i = 0; i < len; ++i) {
        oss << std::setw(2) << std::setfill('0') << (unsigned)h[i];
    }
    retstr = oss.str();
    return retstr;
}

int main(int argc, const char* argv[]) {
    const char message[] =
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopqabcdabcd"
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopqabcdabc";

    std::vector<Test> tests;
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
        unsigned char h[32];
        // SHA256((const unsigned char*)message, len, (unsigned char*)h);
        ifile.read((char*)h, 32);
        tests.push_back(Test(m, h));
    }
    ifile.close();

    int nerror = 0;
    int ncorrect = 0;
    hls::stream<ap_uint<8 * M_SZ> > msg_strm("msg_strm");
    hls::stream<ap_uint<64> > len_strm("len_strm");
    hls::stream<bool> end_len_strm("end_len_strm");
    hls::stream<ap_axiu<256, 0, 0, 0> > hash_strm("hash_strm_in");

    for (std::vector<Test>::const_iterator test = tests.begin(); test != tests.end(); ++test) {
        // std::cout << "\nmessage: \"" << (*test).msg << "\"\n";

        // prepare input
        ap_uint<8 * M_SZ> m;
        int n = 0;
        int cnt = 0;
        for (std::string::size_type i = 0; i < (*test).msg.length(); ++i) {
            if (n == 0) {
                m = 0;
            }
            m.range(7 + 8 * n, 8 * n) = (unsigned)((*test).msg[i]);
            ++n;
            if (n == M_SZ) {
                msg_strm.write(m);
                ++cnt;
                n = 0;
            }
        }
        if (n != 0) {
            msg_strm.write(m);
            ++cnt;
        }
        len_strm.write((unsigned long long)((*test).msg.length()));
        end_len_strm.write(false);

#ifdef DEBUG_VERBOSE
        std::cout << "\nmessage: \"" << (*test).msg << "\"(" << (*test).msg.length() << ") (" << cnt << " words)\n";
#endif
    }
    end_len_strm.write(true);

    std::cout << "\n" << NUM_TESTS << " inputs ready...\n";

    // call module
    DUT_FUNC(msg_strm, len_strm, end_len_strm, hash_strm);

    // check result
    for (std::vector<Test>::const_iterator test = tests.begin(); test != tests.end(); ++test) {
#ifdef DEBUG_VERBOSE
        std::cout << "\nmessage: \"" << (*test).msg << "\"(" << (*test).msg.length() << ")\n";
#endif

        ap_axiu<256, 0, 0, 0> data = hash_strm.read();
        ap_uint<256> h256 = data.data;

        unsigned char h[32];
        for (int i = 0; i < 32; ++i) {
            h[i] = (unsigned char)(h256.range(7 + 8 * i, 8 * i).to_int() & 0xff);
        }

#ifdef DEBUG_VERBOSE
        std::cout << "return: " << hash2str((char*)h, 32) << "\n";
#endif

        if (memcmp((*test).h256, h, 32)) {
            ++nerror;
#ifndef DEBUG_VERBOSE
            std::cout << "\nmessage: \"" << (*test).msg << "\"(" << (*test).msg.length() << ")\n";
            std::cout << "return: " << hash2str((char*)h, 32) << "\n";
#endif
            std::cout << "golden: " << hash2str((char*)(*test).h256, 32) << "\n";
        } else {
            ++ncorrect;
        }
    }

    if (nerror) {
        std::cout << "\nFAIL: " << nerror << " errors found.\n";
    } else {
        std::cout << "\nPASS: " << ncorrect << " inputs verified, no error found.\n";
    }
    return nerror;
}

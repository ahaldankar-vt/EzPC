#ifndef LEAKY_DELTA_OT_H__
#define LEAKY_DELTA_OT_H__
#include "OT/emp-ot.h"
#include "utils/emp-tool.h"
namespace sci {
#ifdef __GNUC__
	#ifndef __clang__
		#pragma GCC push_options
		#pragma GCC optimize ("unroll-loops")
	#endif
#endif

template<typename T>
class LeakyDeltaOT: public IKNP<T> { public:
	LeakyDeltaOT(T * io): IKNP<T>(io, false){
	}
	
	void send_dot(block128 * data, int length) {
		this->send_cot(data, length);
		this->io->flush();
		block128 one = makeBlock128(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFE);
		for (int i = 0; i < length; ++i) {
			data[i] = data[i] & one;
		}
	}
	void recv_dot(block128* data, int length) {
		bool * b = new bool[length];
		this->prg.random_bool(b, length);
		this->recv_cot(data, b, length);
		this->io->flush();

		block128 ch[2];
		ch[0] = zero_block();
		ch[1] = makeBlock128(0, 1);
		block128 one = makeBlock128(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFE);
		for (int i = 0; i < length; ++i) {
			data[i] = (data[i] & one) ^ ch[b[i]];
		}
		delete[] b;
	}
};

#ifdef __GNUC_
	#ifndef __clang___
		#pragma GCC pop_options
	#endif
#endif
}
#endif// LEAKY_DELTA_OT_H__

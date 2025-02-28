#ifndef EMP_AG2PC_HELPER_H__
#define EMP_AG2PC_HELPER_H__
#include "GC/emp-tool.h"
#include "GC/config.h"

using std::future;
using std::cout;
using std::endl;

namespace sci {

template<typename T>
void joinNclean(vector<future<T>>& res) {
	for(auto &v: res) v.get();
	res.clear();
}

template<typename T, int B>
void send_partial_block(T * io, const block128 * data, int length) {
	for(int i = 0; i < length; ++i) {
		io->send_data(&(data[i]), B);
	}
}

template<typename T, int B>
void recv_partial_block(T * io, block128 * data, int length) {
	for(int i = 0; i < length; ++i) {
		io->recv_data(&(data[i]), B);
	}
}

template<typename T>
block128 coin_tossing(PRG128 prg, T* io, int party) {
	block128 S, S2;
	char dgst[Hash::DIGEST_SIZE];
	prg.random_block(&S, 1);
	if(party == ALICE) {
		Hash::hash_once(dgst, &S, sizeof(block128));
		io->send_data(dgst, Hash::DIGEST_SIZE);
		io->recv_block(&S2, 1);
		io->send_block(&S, 1);
	} else {
		char dgst2[Hash::DIGEST_SIZE];
		io->recv_data(dgst2, Hash::DIGEST_SIZE);
		io->send_block(&S, 1);
		io->recv_block(&S2, 1);
		Hash::hash_once(dgst, &S2, sizeof(block128));
		if (memcmp(dgst, dgst2, Hash::DIGEST_SIZE)!= 0)
			error("cheat CT!");
	}
	io->flush();
	return S ^ S2;
}


}
#endif// __HELPER

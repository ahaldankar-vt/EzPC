#ifndef EMP_AG2PC_FEQ_H__
#define EMP_AG2PC_FEQ_H__
#include "GC/emp-tool.h"

namespace sci {

template<typename T>
class Feq { public:
	Hash h;
	T* io = nullptr;
	int party;
	Feq(T* io, int party) {
		this->io = io;
		this->party = party;
	}
	void add_block(const block128 & in) {
		h.put(&in, sizeof(block128));
	}

	void add_data(const void * data, int len) {
		h.put(data, len);
	}

	void dgst(char * dgst) {
		h.digest(dgst);
	}
	bool compare() {
		char AR[Hash::DIGEST_SIZE+16];
		char dgst[Hash::DIGEST_SIZE];
		h.digest(AR);
		if(party == ALICE) {
			PRG128 prg;
			prg.random_data(AR+Hash::DIGEST_SIZE, 16);
			Hash::hash_once(dgst, AR, Hash::DIGEST_SIZE+16);
			io->send_data(dgst, Hash::DIGEST_SIZE);
			io->recv_data(dgst, Hash::DIGEST_SIZE);
			io->send_data(AR+Hash::DIGEST_SIZE, 16);
		} else {
			io->recv_data(dgst, Hash::DIGEST_SIZE);
			io->send_data(AR, Hash::DIGEST_SIZE);
			io->recv_data(AR+Hash::DIGEST_SIZE, 16);
			Hash::hash_once(AR, AR, Hash::DIGEST_SIZE+16);
		}
		io->flush();
		return memcmp(dgst, AR, Hash::DIGEST_SIZE)==0;
	}
};

}
#endif// FEQ_H__

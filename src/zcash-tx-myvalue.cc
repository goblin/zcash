#include <stdio.h>

#include <vector>
#include <algorithm>
#include <boost/iostreams/stream.hpp>

#include "config/bitcoin-config.h"
#include "chainparams.h"
#include "utilstrencodings.h"

#include "hexline_reader.h"

typedef std::vector<ZCNoteDecryption> decryptors_t;

class LaxDecryptor : ZCNoteDecryption {
public:
	LaxDecryptor(const ZCNoteDecryption &parent) : ZCNoteDecryption(parent) {}
	const libzcash::ViewingKey get_vk()
	{
		return sk_enc;
	}
};

static void try_decrypt_one(const CTransaction &tx,
		const JSDescription &js, 
		const ZCNoteDecryption::Ciphertext &ct, 
		const ZCNoteDecryption &decryptor)
{
	static const auto zcparams = ZCJoinSplit::Unopened();

	try {
		auto hsig = js.h_sig(*zcparams, tx.joinSplitPubKey);
		auto npt = libzcash::NotePlaintext::decrypt(decryptor, ct,
				js.ephemeralKey, hsig, 0);
		LaxDecryptor ld(decryptor);
		printf("val=%lf vk=%s txid=%s rho=%s memo=%s\n", 
				npt.value/100000000.0,
				ld.get_vk().ToString().c_str(),
				tx.GetHash().ToString().c_str(),
				npt.rho.ToString().c_str(),
				HexStr(npt.memo).c_str()
				);
	} catch(...) {
	}
}

static void try_decrypt(const CTransaction &tx, const decryptors_t &decs)
{
	for(auto js : tx.vjoinsplit) {
		for(auto ct : js.ciphertexts) {
			for(auto dec : decs) {
				try_decrypt_one(tx, js, ct, dec);
			}
		}
	}
}

int main(int argc, char **argv)
{
	SelectParams(CBaseChainParams::MAIN);
	decryptors_t decs;

	for(int i = 1; i < argc; ++i) {
		auto data = uint256S(argv[i]);
		auto vk = libzcash::ViewingKey(data);
		auto dec = ZCNoteDecryption(vk);
		decs.push_back(dec);
	}

	while(!std::cin.eof()) {
		CTransaction tx;
		hexline_reader hr;
		boost::iostreams::stream<hexline_reader> in(hr);
		try {
			tx.Unserialize(in, 0, 2);
			if(tx.nVersion == 2) {
				try_decrypt(tx, decs);
			}
		} catch(const std::exception &e) {
			return 0;
		}
	}

	return 0;
}

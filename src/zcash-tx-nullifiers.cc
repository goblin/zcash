#include <stdio.h>

#include <vector>
#include <algorithm>
#include <string>
#include <boost/iostreams/stream.hpp>

#include "config/bitcoin-config.h"
#include "chainparams.h"
#include "utilstrencodings.h"

#include "hexline_reader.h"

void print_tx_nullifiers(const CTransaction &tx)
{
	std::string out;
	for(auto i : tx.vjoinsplit) {
		for(auto j : i.nullifiers) {
			out += " " + j.ToString();
		}
	}
	printf("%s:%s\n", tx.GetHash().ToString().c_str(), out.c_str());
}

int main()
{
	SelectParams(CBaseChainParams::MAIN);

	while(!std::cin.eof()) {
		CTransaction tx;
		hexline_reader hr;
		boost::iostreams::stream<hexline_reader> in(hr);
		try {
			tx.Unserialize(in, 0, 2);
			if(tx.nVersion == 2) {
				print_tx_nullifiers(tx);
			}
		} catch(const std::exception &e) {
			return 0;
		}
	}

	return 0;
}

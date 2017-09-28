#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <algorithm>
#include <sstream>
#include <iterator>

#include "config/bitcoin-config.h"
#include "chainparams.h"
#include "base58.h"
#include "utilstrencodings.h"
#include "zcash/prf.h"

int main(int argc, char **argv)
{
	if(argc != 2) {
		fprintf(stderr, "need rho as an argument\n");
		return 1;
	}
	if(strlen(argv[1]) != 64) {
		fprintf(stderr, "rho should be exactly 64 characters\n");
		argv[1]++;
	}
	
	SelectParams(CBaseChainParams::MAIN);

	std::string a_sk_str;
	std::getline(std::cin, a_sk_str);

	auto a_sk = CZCSpendingKey(a_sk_str).Get();
	auto rho = uint256S(argv[1]);

	printf("%s\n", PRF_nf(a_sk, rho).ToString().c_str());

	return 0;
}

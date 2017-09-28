#ifndef HEXLINE_READER_H
#define HEXLINE_READER_H

#include <iostream>
#include <iosfwd> // streamsize
#include <boost/iostreams/concepts.hpp>

/**
  	reads a line of hex digits from stdin and outputs it as binary.
	ends the output stream upon encountering a newline.
	WARNING: make sure you pipe the transaction data, not paste it,
		because UNIX terminals can only handle 4096 characters in a line
  */
class hexline_reader : public boost::iostreams::source {
private:
	char rbuf[3];
public:
	std::streamsize read(char *s, std::streamsize n) {
		using namespace std;
		if(cin.eof())
			return -1;

		streamsize i;
		for(i = 0; i < n; ++i) {
			cin.get(rbuf[0]);
			if(rbuf[0] != '\n') {
				cin.get(rbuf[1]);
				if(rbuf[1] == '\n')
					throw std::runtime_error("newline on unexpected position");
			}

			if(cin.eof() || rbuf[0] == '\n') {
				if(i == 0)
					return -1;
				else
					return i;
			}
			long byte = strtol(rbuf, NULL, 16);
			s[i] = (char)byte;
		}

		return i;
	}

	hexline_reader() {
		rbuf[2] = 0;
		boost::iostreams::source();
	}
};

#endif // HEXLINE_READER_H

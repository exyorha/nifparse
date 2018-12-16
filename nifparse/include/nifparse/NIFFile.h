#ifndef NIFPARSE_NIFFILE_H
#define NIFPARSE_NIFFILE_H

#include <iostream>

namespace nifparse {
	class NIFFile {
	public:
		NIFFile();
		~NIFFile();

		NIFFile(const NIFFile &other) = delete;
		NIFFile &operator =(const NIFFile &other) = delete;

		void parse(std::iostream &ins);
	};
}

#endif

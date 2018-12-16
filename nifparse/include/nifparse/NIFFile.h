#ifndef NIFPARSE_NIFFILE_H
#define NIFPARSE_NIFFILE_H

#include <iostream>
#include <nifparse/Types.h>

namespace nifparse {
	class NIFFile {
	public:
		NIFFile();
		~NIFFile();

		NIFFile(const NIFFile &other) = delete;
		NIFFile &operator =(const NIFFile &other) = delete;

		void parse(std::iostream &ins);

		NIFArray &rootObjects();

	private:
		void linkBlock(NIFVariant &value);
		inline void doLinkBlock(std::monostate) { }
		inline void doLinkBlock(uint32_t) { }
		void doLinkBlock(NIFDictionary &val);
		void doLinkBlock(NIFArray &arr);
		inline void doLinkBlock(NIFEnum &) { }
		inline void doLinkBlock(NIFBitflags &) { }
		inline void doLinkBlock(std::vector<unsigned char> &) { }
		inline void doLinkBlock(std::string &) { }
		void doLinkBlock(NIFReference &);
		void doLinkBlock(NIFPointer &);
		inline void doLinkBlock(float) { }

		NIFVariant m_header;
		std::vector<std::shared_ptr<NIFVariant>> m_blocks;
		NIFVariant m_footer;
	};
}

#endif

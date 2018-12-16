#ifndef NIFPARSE_FILE_DATA_STREAM_H
#define NIFPARSE_FILE_DATA_STREAM_H

#include <nifparse/INIFDataStream.h>
#include <iostream>

namespace nifparse {
	class FileDataStream final : public INIFDataStream {
	public:
		FileDataStream(std::iostream &stream);
		~FileDataStream();

		virtual void readBytes(unsigned char *bytes, size_t size) override;
		virtual void writeBytes(const unsigned char *bytes, size_t size) override;

	private:
		std::iostream &m_stream;
	};
}

#endif

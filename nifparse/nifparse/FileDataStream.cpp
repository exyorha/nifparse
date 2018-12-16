#include <nifparse/FileDataStream.h>

namespace nifparse {

	FileDataStream::FileDataStream(std::iostream &stream) : m_stream(stream) {

	}

	FileDataStream::~FileDataStream() {

	}

	void FileDataStream::readBytes(unsigned char *bytes, size_t size) {
		m_stream.read(reinterpret_cast<char *>(bytes), size);
	}

	void FileDataStream::writeBytes(const unsigned char *bytes, size_t size) {
		m_stream.write(reinterpret_cast<const char *>(bytes), size);
	}
}

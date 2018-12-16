#include <nifparse/ConstantDataStream.h>

#include <stdexcept>

namespace nifparse {
	ConstantDataStream::ConstantDataStream(const unsigned char *data, size_t dataSize) : m_ptr(data), m_end(data + dataSize) {

	}

	ConstantDataStream::~ConstantDataStream() {

	}

	void ConstantDataStream::readBytes(unsigned char *bytes, size_t size) {
		if (m_ptr + size > m_end)
			throw std::logic_error("ConstantDataStream read is out of bounds");

		memcpy(bytes, m_ptr, size);

		m_ptr += size;
	}

	void ConstantDataStream::writeBytes(const unsigned char *bytes, size_t size) {
		(void)bytes;
		(void)size;
		
		throw std::logic_error("ConstantDataStream is not writable");
	}
}

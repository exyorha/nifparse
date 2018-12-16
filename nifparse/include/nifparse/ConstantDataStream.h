#ifndef NIFPARSE_CONSTANT_DATA_STREAM_H
#define NIFPARSE_CONSTANT_DATA_STREAM_H

#include <nifparse/INIFDataStream.h>

namespace nifparse {
	class ConstantDataStream final : public INIFDataStream {
	public:
		ConstantDataStream(const unsigned char *data, size_t dataSize);
		~ConstantDataStream();

		virtual void readBytes(unsigned char *bytes, size_t size) override;
		virtual void writeBytes(const unsigned char *bytes, size_t size) override;

	private:
		const unsigned char *m_ptr, *m_end;
	};
}

#endif

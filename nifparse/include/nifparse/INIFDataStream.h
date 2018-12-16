#ifndef NIFPARSE_I_NIF_DATA_STREAM_H
#define NIFPARSE_I_NIF_DATA_STREAM_H

#include <stdint.h>

namespace nifparse {
	class INIFDataStream {
	protected:
		inline INIFDataStream() {} 
		inline ~INIFDataStream() {}

		INIFDataStream(const INIFDataStream &other) = delete;
		INIFDataStream &operator =(const INIFDataStream &other) = delete;

	public:
		virtual void readBytes(unsigned char *bytes, size_t size) = 0;
		virtual void writeBytes(const unsigned char *bytes, size_t size) = 0;
	};
}

#endif

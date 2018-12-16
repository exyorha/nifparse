#ifndef NIFPARSE_SERIALIZER_CONTEXT_H
#define NIFPARSE_SERIALIZER_CONTEXT_H

#include <iostream>
#include <nifparse/Types.h>

namespace nifparse {
	class INIFDataStream;

	class SerializerContext {
	public:
		SerializerContext(NIFVariant &header, INIFDataStream &stream, bool useConstantLengths);
		~SerializerContext();

		SerializerContext(const SerializerContext &other) = delete;
		SerializerContext &operator =(const SerializerContext &other) = delete;

		inline INIFDataStream &stream() { return m_stream; }
		inline bool useConstantLengths() const { return m_useConstantLengths; }

		NIFVariant &header;

	private:
		INIFDataStream &m_stream;
		bool m_useConstantLengths;
	};
}

#endif

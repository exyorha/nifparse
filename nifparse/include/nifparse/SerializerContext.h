#ifndef NIFPARSE_SERIALIZER_CONTEXT_H
#define NIFPARSE_SERIALIZER_CONTEXT_H

#include <iostream>
#include <nifparse/Types.h>

namespace nifparse {
	class SerializerContext {
	public:
		SerializerContext(std::iostream &stream);
		~SerializerContext();

		SerializerContext(const SerializerContext &other) = delete;
		SerializerContext &operator =(const SerializerContext &other) = delete;

		inline std::iostream &stream() { return m_stream; }

		NIFVariant header;

	private:
		std::iostream &m_stream;
	};
}

#endif

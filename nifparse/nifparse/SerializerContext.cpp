#include <nifparse/SerializerContext.h>

namespace nifparse {
	SerializerContext::SerializerContext(NIFVariant &header, INIFDataStream &stream, bool useConstantLengths) : header(header), m_stream(stream), m_useConstantLengths(useConstantLengths) {

	}

	SerializerContext::~SerializerContext() {

	}
}

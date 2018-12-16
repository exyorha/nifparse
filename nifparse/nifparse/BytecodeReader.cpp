#include <nifparse/BytecodeReader.h>
#include <nifparse/bytecode.h>

namespace nifparse {
	BytecodeReader::BytecodeReader(size_t offset) : m_ptr(nifBytecode + offset) {

	}

	BytecodeReader::~BytecodeReader() {

	}

	uint8_t BytecodeReader::readByte() {
		return *m_ptr++;
	}

	uint32_t BytecodeReader::readVarInt() {
		uint32_t val = 0;
		uint8_t byte;

		do {
			byte = *m_ptr++;

			val = (val << 7) | (byte & 0x7F);
		} while (byte & 0x80);
		
		return val;
	}

	const char *BytecodeReader::readAsciiz() {
		auto stringPtr = reinterpret_cast<const char *>(m_ptr);
		
		m_ptr += strlen(stringPtr) + 1;

		return stringPtr;
	}

	uint16_t BytecodeReader::readU16() {
		union {
			unsigned char bytes[2];
			uint16_t value;
		} u;

		u.bytes[0] = *m_ptr++;
		u.bytes[1] = *m_ptr++;

		return u.value;
	}

	void BytecodeReader::branch(int displacement) {
		m_ptr += displacement;
	}

	size_t BytecodeReader::position() const {
		return m_ptr - nifBytecode;
	}
	
	const unsigned char *BytecodeReader::readBytes(size_t length) {
		auto result = m_ptr;

		m_ptr += length;

		return result;
	}
}

#ifndef NIFPARSE_BYTECODE_READER_H
#define NIFPARSE_BYTECODE_READER_H

#include <string.h>
#include <stdint.h>

namespace nifparse {
	class BytecodeReader {
	public:
		explicit BytecodeReader(size_t offset);
		~BytecodeReader();

		BytecodeReader(const BytecodeReader &other) = delete;
		BytecodeReader &operator =(const BytecodeReader &other) = delete;

		uint8_t readByte();
		uint32_t readVarInt();
		const char *readAsciiz();
		uint16_t readU16();
		const unsigned char *readBytes(size_t length);

		void branch(int displacement);

		size_t position() const;

	private:
		const uint8_t *m_ptr;
	};
}

#endif

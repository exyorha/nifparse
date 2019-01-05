#include <nifparse/Symbol.h>
#include <nifparse/SymbolTable.h>
#include <nifparse/BytecodeReader.h>
#include <nifparse/Types.h>

namespace nifparse {
	Symbol::Symbol(const char *string) : m_value(m_symbolTable.lookupSymbol(string)) {

	}

	const char *Symbol::toString() const {
		return m_symbolTable.symbolToString(m_value);
	}

	size_t Symbol::typeBytecodeStartOffset() const {
		return m_symbolTable.bytecodeStartOffset(*this);
	}

	bool Symbol::isTypeName() const {
		return m_symbolTable.isTypeName(*this);
	}

	Symbol Symbol::parentType() const {
		BytecodeReader reader(typeBytecodeStartOffset());

		if (static_cast<Opcode>(reader.readByte()) != Opcode::BEGIN) {
			throw std::logic_error("BEGIN expected");
		}

		if (reader.readVarInt() != m_value) {
			throw std::logic_error("Type ID expected");
		}

		if (static_cast<Opcode>(reader.readByte()) != Opcode::COMPOUND)
			return Symbol();

		if (static_cast<Opcode>(reader.readByte()) != Opcode::INHERIT)
			return Symbol();

		if (static_cast<Opcode>(reader.readByte()) != Opcode::NAMED_TYPE)
			return Symbol();

		return Symbol(reader.readVarInt());
	}

	SymbolTable Symbol::m_symbolTable;
}

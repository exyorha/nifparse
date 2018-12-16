#include <nifparse/Symbol.h>
#include <nifparse/SymbolTable.h>

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

	SymbolTable Symbol::m_symbolTable;
}

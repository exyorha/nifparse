#include <nifparse/PrettyPrinter.h>
#include <functional>
#include <string>
#include <algorithm>

namespace nifparse {
	PrettyPrinter::PrettyPrinter(std::ostream &stream) : m_stream(stream), m_state(State::StartOfLine), m_level(0) {

	}

	PrettyPrinter::~PrettyPrinter() = default;

	void PrettyPrinter::print(const NIFVariant &value) {
		std::visit([=](auto &&val) {
			doPrint(val);
		}, value);
	}

	void PrettyPrinter::doPrint(std::monostate) {
		printValue("NULL");
	}

	void PrettyPrinter::doPrint(uint32_t val) {
		printValue(std::to_string(val).c_str());
	}

	void PrettyPrinter::doPrint(const NIFDictionary &dictionary) {
		if (dictionary.isNiObject) {
			printValueNoNewLine("NIOBJECT:");
		}
		else {
			printValueNoNewLine("DICTIONARY:");
		}

		bool first = true;

		for (const auto &type : dictionary.typeChain) {
			if (first) {
				first = false;
			}
			else {
				printValueNoNewLine(" -> ");
			}
			printValueNoNewLine(type.toString());
		}

		printValue("");

		increaseLevel();

		for (const auto &pair : dictionary.data) {
			printKey(pair.first.toString());
			print(pair.second);
		}

		decreaseLevel();
	}

	void PrettyPrinter::doPrint(const NIFArray &ary) {
		printValue("ARRAY");

		increaseLevel();

		for (const auto &item : ary.data) {
			print(item);
		}

		decreaseLevel();
	}

	void PrettyPrinter::doPrint(const NIFEnum &val) {
		printValueNoNewLine("ENUM[");
		printValueNoNewLine(std::to_string(val.rawValue).c_str());
		printValueNoNewLine("]:");
		printValue(val.symbolicValue.toString());

	}

	void PrettyPrinter::doPrint(const NIFBitflags &val) {
		printValueNoNewLine("BITFLAGS[");
		printValueNoNewLine(std::to_string(val.rawValue).c_str());
		printValueNoNewLine("]: ");

		for (const auto &entry : val.symbolicValues) {
			printValueNoNewLine(entry.toString());
			printValueNoNewLine(" ");
		}

		printValue("");
	}

	void PrettyPrinter::doPrint(const std::vector<unsigned char> &byteArray) {
		printValue("BYTEARRAY");

		increaseLevel();

		for (size_t offset = 0; offset < byteArray.size(); offset += 16) {
			char buf[16];
			snprintf(buf, sizeof(buf), "%04zX ", offset);

			printValueNoNewLine(buf);

			auto chunk = std::min<size_t>(byteArray.size() - offset, 16);

			for (size_t byte = 0; byte < chunk; byte++) {
				snprintf(buf, sizeof(buf), "%02X ", byteArray[offset + byte]);
				printValueNoNewLine(buf);
			}

			for (size_t byte = chunk; byte < 16; byte++) {
				printValueNoNewLine("   ");
			}

			printValueNoNewLine(" | ");

			for (size_t byte = 0; byte < chunk; byte++) {
				auto ch = byteArray[offset + byte];
				snprintf(buf, sizeof(buf), "%c", (ch >= 0x20 && ch < 0x7F) ? ch : '.');
				printValueNoNewLine(buf);
			}

			printValue("");
		}

		decreaseLevel();
	}

	void PrettyPrinter::doPrint(const std::string &string) {
		printValueNoNewLine("\"");
		printValueNoNewLine(string.c_str());
		printValue("\"");
	}

	void PrettyPrinter::doPrint(const NIFReference &ref) {
		printValueNoNewLine("REF<");
		printValueNoNewLine(ref.type.toString());
		printValueNoNewLine("> ");
		printValue(std::to_string(ref.target).c_str());

		if (ref.ptr) {
			printReference(ref.ptr);
		}
	}

	void PrettyPrinter::doPrint(const NIFPointer &ref) {
		printValueNoNewLine("PTR<");
		printValueNoNewLine(ref.type.toString());
		printValueNoNewLine("> ");
		printValue(std::to_string(ref.target).c_str());

		auto ptr = ref.ptr.lock();
		if (ptr) {
			printReference(ptr);
		}
	}

	void PrettyPrinter::printReference(const std::shared_ptr<NIFVariant> &ref) {
		auto result = m_referencesPrinted.emplace(ref.get());

		if (result.second) {
			increaseLevel();

			print(*ref);

			decreaseLevel();
		}
	}

	void PrettyPrinter::doPrint(float val) {
		printValue(std::to_string(val).c_str());
	}

	void PrettyPrinter::startLine() {
		if (m_state == State::StartOfLine) {
			for (size_t level = 0; level < m_level; level++) {
				m_stream << "  ";
			}

			m_state = State::InLine;
		}
	}

	void PrettyPrinter::endLine() {
		if (m_state == State::InLine) {
			m_stream << "\n";
			m_state = State::StartOfLine;
		}
	}

	void PrettyPrinter::printKey(const char *key) {
		startLine();

		m_stream << key;
		m_stream << " = ";
	}

	void PrettyPrinter::printValue(const char *value) {
		startLine();

		m_stream << value;

		endLine();
	}

	void PrettyPrinter::printValueNoNewLine(const char *value) {
		startLine();

		m_stream << value;
	}

	void PrettyPrinter::increaseLevel() {
		m_level++;
	}

	void PrettyPrinter::decreaseLevel() {
		m_level--;
	}
}

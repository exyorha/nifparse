#ifndef NIFPARSE_PRETTY_PRINTER_H
#define NIFPARSE_PRETTY_PRINTER_H

#include <iostream>
#include <nifparse/Types.h>
#include <unordered_set>

namespace nifparse {
	class PrettyPrinter {
	public:
		PrettyPrinter(std::ostream &stream);
		~PrettyPrinter();

		PrettyPrinter(const PrettyPrinter &other) = delete;
		PrettyPrinter &operator =(const PrettyPrinter &other) = delete;

		void print(const NIFVariant &value);

	private:
		void doPrint(std::monostate);
		void doPrint(uint32_t val);
		void doPrint(const NIFDictionary &dictionary);
		void doPrint(const NIFArray &ary);
		void doPrint(const NIFEnum &val);
		void doPrint(const NIFBitflags &val);
		void doPrint(const std::vector<unsigned char> &byteArray);
		void doPrint(const std::string &string);
		void doPrint(const NIFReference &ref);
		void doPrint(const NIFPointer &ptr);
		void doPrint(float val);

		void printKey(const char *key);
		void printValue(const char *value);
		void printValueNoNewLine(const char *value);
		void increaseLevel();
		void decreaseLevel();
		void startLine();
		void endLine();

		void printReference(const std::shared_ptr<NIFVariant> &ref);

	private:
		enum class State {
			StartOfLine,
			InLine
		};

		std::ostream &m_stream;
		State m_state;
		size_t m_level;
		std::unordered_set<NIFVariant *> m_referencesPrinted;
	};
}

#endif

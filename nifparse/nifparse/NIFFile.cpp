#include <nifparse/NIFFile.h>
#include <nifparse/Serializer.h>
#include <nifparse/SerializerContext.h>
#include <nifparse/PrettyPrinter.h>
#include <nifparse/FileDataStream.h>

#include <functional>

namespace nifparse {
	NIFFile::NIFFile() {

	}

	NIFFile::~NIFFile() {

	}

	void NIFFile::parse(std::iostream &ins) {
		FileDataStream stream(ins);
		SerializerContext ctx(m_header, stream, false);

		Serializer::deserialize(ctx, Symbol("Header"), ctx.header);
		
		auto &header = std::get<NIFDictionary>(ctx.header);
		auto blockCount = header.getValue<uint32_t>(Symbol("Num Blocks"));

		m_blocks.reserve(blockCount);

		Symbol symBlockTypeIndex("Block Type Index");
		Symbol symSizedString("SizedString");
		Symbol symValue("Value");

		if (header.data.count(symBlockTypeIndex) == 0) {
			// Morrowind-era format

			for (size_t index = 0; index < blockCount; index++) {
				NIFVariant string;
				Serializer::deserialize(ctx, symSizedString, string);

				Symbol blockType(std::get<NIFDictionary>(string).getValue<std::string>(symValue).c_str());

				auto blockValue = std::make_shared<NIFVariant>();
				Serializer::deserialize(ctx, blockType, *blockValue);
				
				m_blocks.emplace_back(std::move(blockValue));
			}
		}
		else {
			auto &blockTypeArray = header.getValue<NIFArray>(Symbol("Block Type Index"));
			auto &blockTypes = header.getValue<NIFArray>(Symbol("Block Types"));
			auto &blockSizes = header.getValue<NIFArray>(Symbol("Block Size"));

			size_t position = static_cast<size_t>(ins.tellg());

			for (size_t index = 0; index < blockCount; index++) {
				auto blockTypeIndex = std::get<uint32_t>(blockTypeArray.data[index]);
				if (blockTypeIndex >= blockTypes.data.size())
					throw std::logic_error("block type index is out of range");

				Symbol blockType(std::get<NIFDictionary>(blockTypes.data[blockTypeIndex]).getValue<std::string>(Symbol("Value")).c_str());

				auto blockValue = std::make_shared<NIFVariant>();
				Serializer::deserialize(ctx, blockType, *blockValue);
				
				size_t endPosition = static_cast<size_t>(ins.tellg());

				size_t blockSize = endPosition - position;
				size_t expectedBlockSize = std::get<uint32_t>(blockSizes.data[index]);

				if (blockSize != expectedBlockSize) {
					throw std::logic_error("invalid block length");
				}

				position = endPosition;

				m_blocks.emplace_back(std::move(blockValue));
			}
		}

		for (const auto &block : m_blocks) {
			linkBlock(*block);
		}

		Serializer::deserialize(ctx, Symbol("Footer"), m_footer);
		
		linkBlock(m_footer);
	}

	void NIFFile::linkBlock(NIFVariant &value) {
		std::visit([=](auto &&val) {
			doLinkBlock(val);
		}, value);
	}

	void NIFFile::doLinkBlock(NIFDictionary &val) {
		for (auto &entry : val.data) {
			linkBlock(entry.second);
		}
	}

	void NIFFile::doLinkBlock(NIFArray &val) {
		for (auto &entry : val.data) {
			linkBlock(entry);
		}
	}

	void NIFFile::doLinkBlock(NIFReference &val) {
		if (val.target < 0) {
			val.ptr.reset();
		}
		else {
			val.ptr = m_blocks[val.target];
		}
	}

	void NIFFile::doLinkBlock(NIFPointer &val) {
		if (val.target < 0) {
			val.ptr.reset();
		}
		else {
			val.ptr = m_blocks[val.target];
		}
	}

	NIFDictionary &NIFFile::header() {
		return std::get<NIFDictionary>(m_header);
	}

	const NIFDictionary &NIFFile::header() const {
		return std::get<NIFDictionary>(m_header);
	}

	NIFArray &NIFFile::rootObjects() {
		return std::get<NIFDictionary>(m_footer).getValue<NIFArray>("Roots");
	}

	const NIFArray &NIFFile::rootObjects() const {
		return std::get<NIFDictionary>(m_footer).getValue<NIFArray>("Roots");
	}
}

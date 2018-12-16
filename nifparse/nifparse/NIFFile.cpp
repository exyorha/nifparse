#include <nifparse/NIFFile.h>
#include <nifparse/Serializer.h>
#include <nifparse/SerializerContext.h>
#include <nifparse/PrettyPrinter.h>

namespace nifparse {
	NIFFile::NIFFile() {

	}

	NIFFile::~NIFFile() {

	}

	void NIFFile::parse(std::iostream &ins) {
		SerializerContext ctx(ins);

		Serializer::deserialize(ctx, Symbol("Header"), ctx.header);

		PrettyPrinter printer(std::cout);
		printer.print(ctx.header);

		auto &header = std::get<NIFDictionary>(ctx.header);
		auto blockCount = header.getValue<uint32_t>(Symbol("Num Blocks"));

		std::vector<NIFVariant> blocks;
		blocks.reserve(blockCount);

		auto &blockTypeArray = header.getValue<NIFArray>(Symbol("Block Type Index"));
		auto &blockTypes = header.getValue<NIFArray>(Symbol("Block Types"));
		auto &blockSize = header.getValue<NIFArray>(Symbol("Block Size"));

		size_t position = static_cast<size_t>(ins.tellg());

		for (size_t index = 0; index < blockCount; index++) {
			auto blockTypeIndex = std::get<uint32_t>(blockTypeArray.data[index]);
			if (blockTypeIndex >= blockTypes.data.size())
				throw std::logic_error("block type index is out of range");

			Symbol blockType(std::get<NIFDictionary>(blockTypes.data[blockTypeIndex]).getValue<std::string>(Symbol("Value")).c_str());

			NIFVariant blockValue;
			Serializer::deserialize(ctx, blockType, blockValue);

			//printer.print(blockValue);

			size_t endPosition = static_cast<size_t>(ins.tellg());

			if (endPosition - position != std::get<uint32_t>(blockSize.data[index])) {
				printf("expected %u, got %d\n", std::get<uint32_t>(blockSize.data[index]), endPosition - position);
				throw std::logic_error("invalid block length");
			}

			position = endPosition;

			blocks.emplace_back(std::move(blockValue));
		}

		NIFVariant footer;

		Serializer::deserialize(ctx, Symbol("Footer"), footer);

		printer.print(footer);

		__debugbreak();
	}
}

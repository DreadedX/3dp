#include <sstream>
#include <fstream>

#include "plugin.h"
#include "extra/extra.h"

enum PRAGMA_COMMANDS {
	PRAGMA_VERSION,
	PRAGMA_INTERFACE_START,
	PRAGMA_INTERFACE_END,
	PRAGMA_IMPORT,
	PRAGMA_COUNT
};

const std::string STRING_PRAGMA          = "#";
const std::string STRING_VERSION         = STRING_PRAGMA + "version ";
const std::string STRING_INTERFACE_START = STRING_PRAGMA + "interface_start";
const std::string STRING_INTERFACE_END   = STRING_PRAGMA + "interface_end";
const std::string STRING_VERTEX          = STRING_PRAGMA + "vertex";
const std::string STRING_FRAGMENT        = STRING_PRAGMA + "fragment";

// Default version
std::string version = "330 core";
std::string interface;

std::string vertex;
std::string fragment;

PRAGMA_COMMANDS pragmaSwitch (std::string const& s) {
	if (s == "version") return PRAGMA_VERSION;
	if (s == "interface_start") return PRAGMA_INTERFACE_START;
	if (s == "interface_end") return PRAGMA_INTERFACE_END;
	if (s == "import") return PRAGMA_IMPORT;

	return PRAGMA_COUNT;
}

std::string firstPass(std::string source, std::string filePath) {

	for (std::size_t pragma = 0; pragma != std::string::npos; pragma = source.find("#", pragma+1)) {

		print::d("Preprocessor instruction found at: %i", pragma);

		std::stringstream ss(source.substr(pragma + 1));
		std::string s;

		ss >> s;

		print::d("Preprocessor instruction: %s", s.c_str());

		std::size_t end;
		std::string includeName;
		std::ifstream r;
		std::string includeSource;
		switch(pragmaSwitch(s)) {
			case PRAGMA_VERSION:
				end = source.find("\n", pragma);
				version = source.substr(pragma + STRING_VERSION.length(), end - pragma - STRING_VERSION.length());
				source.erase(pragma, end - pragma);
				pragma = 0;
				break;

			case PRAGMA_INTERFACE_START:
				end = source.find(STRING_INTERFACE_END, pragma);
				interface = source.substr(pragma + STRING_INTERFACE_START.length(), end - pragma - STRING_INTERFACE_START.length());
				source.erase(pragma, end - pragma + 1);
				pragma = 0;
				break;

			case PRAGMA_IMPORT:
				end = source.find("\n", pragma);

				ss >> includeName;
				includeName = filePath.substr(0, filePath.find_last_of("/")+1) + includeName;
				

				r = std::ifstream(includeName);

				if (!r) {

					print::e("Include file not found: %s", includeName.c_str());
					exit(-1);
				}

				includeSource = std::string((std::istreambuf_iterator<char>(r)), std::istreambuf_iterator<char>());

				source.replace(pragma, end - pragma + 1, includeSource);

				break;

			default:
				print::d("Ignoring: %s", s);
				break;
		}

	}
	return source;
}

std::string secondPass(std::string source) {
	
	// VERTEX
	std::size_t start;
	start = source.find(STRING_VERTEX) + STRING_VERTEX.length();

	std::size_t end;
	end = source.find(STRING_FRAGMENT);

	vertex = source.substr(start, end - start);
	
	// FRAGMENT
	start = source.find(STRING_FRAGMENT) + STRING_FRAGMENT.length();

	fragment = source.substr(start);

	return source;
}

std::string removeWhitespace(std::string source) {

	for (std::size_t whitespace = source.find("\n\n"); whitespace != std::string::npos; whitespace = source.find("\n\n")) {

		source.replace(whitespace, 2, "\n");
	}

	return source;
}

void load(std::string assetName, std::string filePath, Array<flux::FileWrite*> *files) {

	std::ifstream t(filePath);

	std::string source((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	source = firstPass(source, filePath);
	source = secondPass(source);

	// VERTEX
	std::string vertexString;

	vertexString += "#version " + version + "\n";
	if (interface != "") {

		vertexString += "out INTERFACE {" + interface + "} vs_out;" + "\n";
	}
	vertexString += vertex + "\n";

	vertexString = removeWhitespace(vertexString);
	print::d(vertexString.c_str());

	// FRAGMENT
	std::string fragmentString;

	fragmentString += "#version " + version + "\n";
	if (interface != "") {

		fragmentString += "in INTERFACE {" + interface + "} fs_in;" + "\n";
	}
	fragmentString += fragment + "\n";

	fragmentString = removeWhitespace(fragmentString);
	print::d(fragmentString.c_str());

	// Create final asset
	flux::FileWrite *file = new flux::FileWrite;
	files->add(file);

	file->name = assetName;
	file->dataSize = sizeof(size_t) + sizeof(size_t) + vertexString.length() + 1 + fragmentString.length() + 1;
	file->data = new byte[file->dataSize];

	uint offset = 0;
	for (uint i = 0; i < sizeof(size_t); ++i) {

		file->data[i+offset] = reinterpret_cast<size_t>(vertexString.length() + 1) >> (i*8);
	}
	offset += sizeof(size_t);
	for (uint i = 0; i < sizeof(size_t); ++i) {

		file->data[i+offset] = reinterpret_cast<size_t>(fragmentString.length() + 1) >> (i*8);
	}
	offset += sizeof(size_t);

	offset += vertexString.copy(reinterpret_cast<char*>(file->data + offset), vertexString.length(), 0);
	file->data[offset] = '\0';
	offset += 1;

	offset += fragmentString.copy(reinterpret_cast<char*>(file->data + offset), fragmentString.length(), 0);
	file->data[offset] = '\0';
	offset += 1;

	assert(offset == file->dataSize);
}

Plugin plugin("GLSL Plugin", "Takes in special glsl files and processes them", load);

extern "C" {

	Plugin getPlugin() {

		return plugin;
	}
}


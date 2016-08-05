#include <sstream>
#include <fstream>

#include "plugin.h"
#include "extra/extra.h"

enum PRAGMA_COMMANDS {
	PRAGMA_VERSION,
	PRAGMA_INTERFACE_START,
	PRAGMA_INTERFACE_END,
	PRAGMA_INCLUDE,
	PRAGMA_COUNT
};

const std::string STRING_PRAGMA          = "#pragma";
const std::string STRING_VERSION         = STRING_PRAGMA + " version ";
const std::string STRING_INTERFACE_START = STRING_PRAGMA + " interface_start";
const std::string STRING_INTERFACE_END   = STRING_PRAGMA + " interface_end";
const std::string STRING_VERTEX          = STRING_PRAGMA + " vertex";
const std::string STRING_FRAGMENT        = STRING_PRAGMA + " fragment";

// Default version
std::string version = "330 core";
std::string interface;

std::string vertex;
std::string fragment;

PRAGMA_COMMANDS pragmaSwitch (std::string const& s) {
	if (s == "version") return PRAGMA_VERSION;
	if (s == "interface_start") return PRAGMA_INTERFACE_START;
	if (s == "interface_end") return PRAGMA_INTERFACE_END;
	if (s == "include") return PRAGMA_INCLUDE;

	return PRAGMA_COUNT;
}

std::string firstPass(std::string source, std::string filePath) {

	for (std::size_t pragma = 0; pragma != std::string::npos; pragma = source.find("#pragma", pragma+1)) {

		print::d("Pragma found at: %i", pragma);

		std::stringstream ss(source.substr(pragma + 7));
		std::string s;

		ss >> s;

		print::d("Pragma command: %s", s.c_str());

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

			case PRAGMA_INCLUDE:
				end = source.find("\n", pragma);

				ss >> includeName;
				includeName = filePath.substr(0, filePath.find_last_of("/")+1) + includeName +".glsl";
				

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

		print::d("Removing whitespace");
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

	flux::FileWrite *vertexFile = new flux::FileWrite;
	files->add(vertexFile);
	vertexFile->name = assetName + "/vertex";

	vertexFile->dataSize = vertexString.length();
	vertexFile->data = new byte[vertexFile->dataSize];

	vertexString.copy(reinterpret_cast<char*>(vertexFile->data), vertexFile->dataSize, 0);

	// FRAGMENT
	std::string fragmentString;

	fragmentString += "#version " + version + "\n";
	if (interface != "") {

		fragmentString += "in INTERFACE {" + interface + "} fs_in;" + "\n";
	}
	fragmentString += fragment + "\n";

	fragmentString = removeWhitespace(fragmentString);
	print::d(fragmentString.c_str());

	flux::FileWrite *fragmentFile = new flux::FileWrite;
	files->add(fragmentFile);
	fragmentFile->name = assetName + "/fragment";

	fragmentFile->dataSize = fragmentString.length();
	fragmentFile->data = new byte[fragmentFile->dataSize];

	fragmentString.copy(reinterpret_cast<char*>(fragmentFile->data), fragmentFile->dataSize, 0);
}

Plugin plugin("GLSL Plugin", "Takes in special glsl files and processes them", load);

extern "C" {

	Plugin getPlugin() {

		return plugin;
	}
}


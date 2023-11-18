//@ts-check

const path = require("path");
const fs = require("fs");
const {Utils} = require("../lib/Utils");

const MAIN_FILENAME = "test_main.c";
const TEST_SUIT_PREFIX = "unit__";
const C_MAX_ID_LENGTH = 31;
const MAX_ID_LENGTH = C_MAX_ID_LENGTH - TEST_SUIT_PREFIX.length - 1;

(async function main() {
	const root = path.join(__dirname, "/../");
	const allFiles = Utils.readDirRecursive(__dirname);
	const testFiles = allFiles.filter(file => file.endsWith(".test.c"));

	const files = testFiles.map(file => ({
		path: file,
		relative: path.relative(root, file),
		base: path.basename(file),
		functions: /**@type {{name: string, description: string, line: number}[]}*/([]),
		priority: 0
	}));

	const testNames = new Set();

	for(const file of files) {
		const content = fs.readFileSync(file.path, "utf8");
		const regex = /DESCRIBE\s*\(([_a-zA-Z][_a-zA-Z0-9]*)\s*,\s*("(?:\\"|.)*?")\)/g;	// 32 - 1 - "unit__".length (6) = 25

		// Get priority of test file
		const [m, priority] = content.match(/^\s*#define\s+TEST_PRIORITY\s+([+-]?[0-9]+)/m) || [null, "0"];
		file.priority = parseInt(priority);

		// Get all test functions
		let match;
		while((match = regex.exec(content)) !== null) {
			const [m, name, description] = match;

			if(name.length > MAX_ID_LENGTH) throw new Error(`Test name "${name}" is too long! Max length is ${MAX_ID_LENGTH} characters.`);
			if(testNames.has(name)) throw new Error(`Test name "${name}" is already used!`);

			testNames.add(name);

			file.functions.push({
				name: TEST_SUIT_PREFIX + name,
				description: description,
				line: content.slice(0, match.index).split("\n").length
			});
		}
	}

	// Sort files by priority
	files.sort((a, b) => b.priority - a.priority);

	fs.writeFileSync(path.join(__dirname, MAIN_FILENAME), `/**
 * @file test/${MAIN_FILENAME}
 * @author test/register_tests.js
 * @date ${new Date().toISOString().slice(0, 10)}
 * @brief This file is generated automatically. Do not edit manually!
 */

#include "allocator/MemoryAllocator.h"
#include "unit.h"

${files.map(file => `// Functions from file "${file.relative}"\n${file.functions.map(func => `void ${func.name}();`).join("\n")}\n`).join("\n")}

int main(int argc, char** argv) {
	UNIT_BEGIN();

	UNIT_SILENT(argc > 1 && strcmp(argv[1], "silent") == 0);

	${files.map(file => `UNIT_REGISTER_FILE("${file.base}");\n\t${file.functions.map(func => `UNIT_REGISTER_SUIT(${func.name}, ${func.description}, "${file.relative}", "${func.line}");`).join("\n\t")}\n`).join("\n\t")}

	UNIT_END();

	Allocator_cleanup();

	return UNIT_RESULT();
}

// End of file ${MAIN_FILENAME}
`);
})();

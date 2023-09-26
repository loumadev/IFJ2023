//@ts-check

const path = require("path");
const fs = require("fs");
const {Utils} = require("../lib/Utils");

const MAIN_FILENAME = "test_main.c";

(async function main() {
	const root = path.join(__dirname, "/../");
	const allFiles = Utils.readDirRecursive(__dirname);
	const testFiles = allFiles.filter(file => file.endsWith(".test.c"));

	const files = testFiles.map(file => ({
		path: file,
		relative: path.relative(root, file),
		base: path.basename(file),
		functions: /**@type {{name: string, line: number}[]}*/([])
	}));

	for(const file of files) {
		const content = fs.readFileSync(file.path, "utf8");
		const regex = /void ([^_].*?)\(/g;

		let match;
		while((match = regex.exec(content)) !== null) {
			file.functions.push({
				name: match[1],
				line: content.slice(0, match.index).split("\n").length
			});
		}
	}

	fs.writeFileSync(path.join(__dirname, MAIN_FILENAME), `/**
 * @file test/${MAIN_FILENAME}
 * @author test/register_tests.js
 * @date ${new Date().toISOString().slice(0, 10)}
 * @brief This file is generated automatically. Do not edit!
 */

#include "allocator/MemoryAllocator.h"
#include "unit.h"

${files.map(file => `// Functions from file "${file.relative}"\n${file.functions.map(func => `void ${func.name}();`).join("\n")}\n`).join("\n")}

int main(int argc, char** argv) {
	UNIT_BEGIN();

	UNIT_SILENT(argc > 1 && strcmp(argv[1], "silent") == 0);

	${files.map(file => `UNIT_REGISTER_FILE("${file.base}");\n\t${file.functions.map(func => `UNIT_REGISTER_SUIT(${func.name}, "${file.relative}", "${func.line}");`).join("\n\t")}\n`).join("\n\t")}

	UNIT_END();

	Allocator_cleanup();

	return UNIT_RESULT();
}`);
})();

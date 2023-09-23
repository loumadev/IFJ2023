//@ts-check

const path = require("path");
const fs = require("fs");

const MAIN_FILENAME = "test_main.c";

(async function main() {
	const root = path.join(__dirname, "/../");
	const allFiles = readDirRecursive(__dirname);
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
	TEST_BEGIN();

	TEST_SILENT(argc > 1 && strcmp(argv[1], "silent") == 0);

	${files.map(file => `TEST_REGISTER_FILE("${file.base}");\n\t${file.functions.map(func => `TEST_REGISTER_SUIT(${func.name}, "${file.relative}", "${func.line}");`).join("\n\t")}\n`).join("\n\t")}

	TEST_END();

	Allocator_cleanup();

	return TEST_RESULT();
}`);
})();

/**
 * Recursively resolves all files in directory
 * @param {string} dirPath Starting directory
 * @param {number} [depth=Infinity] Max depth of recursion
 * @return {string[]} 
 */
function readDirRecursive(dirPath, depth = Infinity) {
	if(!fs.existsSync(dirPath)) return [];
	if(!fs.statSync(dirPath).isDirectory()) return [dirPath];

	/** @type {{path: string, depth: number}[]} */
	const queue = [{path: dirPath, depth: 0}];

	/** @type {string[]} */
	const arrayOfFiles = [];

	while(queue.length) {
		const dir = queue.shift();
		if(!dir || dir.depth > depth) continue;

		const files = fs.readdirSync(dir.path);

		for(const file of files) {
			const pathname = path.join(dir.path, "/", file);

			if(fs.statSync(pathname).isDirectory()) {
				queue.push({
					path: pathname,
					depth: dir.depth + 1
				});
			} else {
				arrayOfFiles.push(pathname);
			}
		}
	}

	return arrayOfFiles;
}
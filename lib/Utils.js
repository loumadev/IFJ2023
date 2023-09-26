const fs = require("fs");
const path = require("path");

class Utils {
	/**
	 * @static
	 * @param {any[]} args
	 * @memberof Utils
	 */
	static info(...args) {
		console.log(`[INFO]`, ...args);
	}

	/**
	 * @static
	 * @param {any[]} args
	 * @memberof Utils
	 */
	static warn(...args) {
		console.warn(`[WARN]`, ...args);
	}

	/**
	 * @static
	 * @param {any[]} args
	 * @memberof Utils
	 */
	static error(...args) {
		console.error(`[ERROR]`, ...args);
	}

	/**
	 * Recursively resolves all files in directory
	 * @param {string} dirPath Starting directory
	 * @param {number} [depth=Infinity] Max depth of recursion
	 * @return {string[]} 
	 */
	static readDirRecursive(dirPath, depth = Infinity) {
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

	/**
	 * Recursively creates directory if it doesn't exist
	 * @static
	 * @param {string} dirPath
	 * @memberof Utils
	 */
	static createDirectory(dirPath) {
		if(!fs.existsSync(dirPath)) {
			fs.mkdirSync(dirPath, {recursive: true});
		}
	}

	/**
	 * Adds all values from array to set
	 * @template T
	 * @static
	 * @param {Set<T>} set
	 * @param {T[]} values
	 * @memberof Utils
	 */
	static setAddAll(set, values) {
		for(const value of values) {
			set.add(value);
		}
	}
}

module.exports = {
	Utils
};
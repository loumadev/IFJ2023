//@ts-check

const fs = require("fs");
const path = require("path");
const {spawnSync} = require("child_process");
const {Utils} = require("../lib/Utils");

const ROOT = path.join(__dirname, "/..");
const SOURCES_DIR = path.join(ROOT, "/src");
const HEADERS_DIR = path.join(ROOT, "/include");
const MAIN = path.join(SOURCES_DIR, "/main.c");

const OUTPUT_DIR = path.join(__dirname, "/output");
const PROJECT_DIR = path.join(OUTPUT_DIR, "/project");

const ARCHIVE_PATH = path.join(OUTPUT_DIR, "/xlouma00.zip");

/**
 * @typedef {Object} ProjectFile
 * @prop {string} path 
 * @prop {string} relative 
 * @prop {string} name 
 * @prop {string} extension
 */

/**
 * @typedef {Object} FileModule
 * @prop {ProjectFile} file 
 * @prop {ProjectFile[]} dependencies
 * @prop {ProjectFile[]} dependents
 * @prop {boolean} isResolved
 */

/**
 * @typedef {Record<ProjectFile["path"], FileModule>} DependencyTree
 */

/**
 * @typedef {Object} Project
 * @prop {ProjectFile[]} files 
 * @prop {DependencyTree} dependencies 
 * @prop {string} root
 */

(async function main() {
	const start = Date.now();

	const FAILURE = `Failed to deploy project:`;


	Utils.info("Preparing...");


	// Create output directories
	prepareOutputDirectories();

	/** @type {Project} */
	const project = {
		files: /**@type {any}*/([]),
		dependencies: {},
		root: ROOT
	};


	Utils.info("Collecting project files...");


	// Collect all source files
	/** @type {ProjectFile[]} */
	const sources = Utils.readDirRecursive(SOURCES_DIR)
		.filter(e => e.endsWith(".c"))
		.map(e => absolutePathToProjectFile(project, e));

	// Collect all header files
	/** @type {ProjectFile[]} */
	const headers = Utils.readDirRecursive(HEADERS_DIR)
		.filter(e => e.endsWith(".h"))
		.map(e => absolutePathToProjectFile(project, e));

	project.files = [...sources, ...headers];


	Utils.info("Resolving dependencies...");
	{
		// Collect all source files
		for(const source of sources) {
			resolveDependencies(project, source);
		}

		// Validate usage of all files
		for(const file of project.files) {
			if(project.dependencies[file.path]) continue;

			Utils.warn(`File "${file.relative}" is not included in project! This file will be ignored!`);
		}
	}


	Utils.info("Checking accessability..."); // Validate reachability from main through includes
	const mainFile = absolutePathToProjectFile(project, MAIN);
	const main = resolveDependencies(project, mainFile);
	const reachable = new Set([main.file]);
	{
		const queue = [...main.dependencies];
		while(queue.length) {
			const current = queue.shift();
			if(!current) continue;

			reachable.add(current);

			const file = project.dependencies[current.path];
			if(!file) continue;

			const toCheck = [...file.dependencies, ...file.dependents]
				.filter(e => !reachable.has(e));

			Utils.setAddAll(reachable, toCheck);
			queue.push(...toCheck);
		}

		for(const file of project.files) {
			if(!reachable.has(file)) {
				Utils.warn(`File "${file.relative}" is not reachable from main! This file will be ignored!`);
			}
		}
	}


	Utils.info("Validating..."); // Look for duplicate names
	{
		/** @type {Map<ProjectFile["path"], ProjectFile>} */
		const names = new Map();
		for(const file of project.files) {
			const duplicate = names.get(file.path);
			if(duplicate) {
				Utils.error(FAILURE, `Duplicate file names "${duplicate.relative}" and "${file.relative}"!`);
				process.exit(1);
			}
			names.set(file.path, file);
		}
	}


	Utils.info("Processing project files...");
	{
		for(const file of [...reachable]) {
			// Copy the file
			const newPath = path.join(PROJECT_DIR, file.name);
			fs.copyFileSync(file.path, newPath);

			//Modify the include paths
			const newSource = fs.readFileSync(newPath, "utf8")
				.replace(/^[\t ]*#include\s*"(.*?)"/gm, (m, includeFile) => {
					const name = path.basename(includeFile);
					const file = getFileByName(project, name);

					return `#include "${file?.name || name}" // Included from "${file?.relative || "<failed to resolve>"}"`;
				});

			fs.writeFileSync(newPath, newSource);
		}
	}

	Utils.info("Generating Makefile...");
	const compilerSettings = {};
	{
		const makefile = fs.readFileSync(path.join(ROOT, "/Makefile"), "utf8");
		const names = ["COMPILER", "CFLAGS", "LIBS", "OUT"];

		makefile.replace(/^\s*(COMPILER|CFLAGS|LIBS|OUT)\s*=\s*(.+)$/gm, (m, name, value) => {
			if(!names.includes(name)) return "";
			compilerSettings[name] = value;
			return "";
		});

		// Validate variables
		for(const name of names) {
			if(compilerSettings[name]) continue;
			Utils.error(FAILURE, `Missing or invalid variable "${name}" in Makefile!`);
			process.exit(1);
		}

		fs.writeFileSync(path.join(PROJECT_DIR, "/Makefile"), `#
# File: Makefile
# Author: deploy/deploy.js
# Date: ${new Date().toISOString().slice(0, 10)}
# Brief: This file is generated automatically. Do not edit!
#

COMPILER = ${compilerSettings.COMPILER}
CFLAGS = ${compilerSettings.CFLAGS}
LIBS = ${compilerSettings.LIBS}
OUT = ${compilerSettings.OUT}

all: \$(OUT)

\$(OUT): \$(wildcard *.c) \$(wildcard *.h)
	\$(COMPILER) \$(CFLAGS) \$(LIBS) -o \$(OUT) *.c

# End of file Makefile
`);
	}


	Utils.info("Compiling...");
	{
		const proc = spawnSync("make", {
			cwd: PROJECT_DIR,
			shell: true,
			stdio: "pipe"
		});

		if(proc.status !== 0) {
			Utils.error(FAILURE, `Failed to compile the project!`);
			Utils.error(proc.stdout.toString());
			Utils.error(proc.stderr.toString());
			process.exit(1);
		}

		fs.unlinkSync(path.join(PROJECT_DIR, compilerSettings["OUT"]));
	}


	//TODO: Maybe test the project?


	Utils.info("Packing...");
	{
		const zip = spawnSync("zip", ["-r", ARCHIVE_PATH, "."], {
			cwd: PROJECT_DIR,
			shell: true,
			stdio: "pipe"
		});

		if(zip.status !== 0) {
			Utils.error(FAILURE, `Failed to create the project archive!`);
			Utils.error(zip.stdout.toString());
			Utils.error(zip.stderr.toString());
			process.exit(1);
		}
	}


	Utils.info(`Done in ${Date.now() - start}ms`);
})();


/**
 * @param {Project} project
 * @param {ProjectFile} file
 * @return {FileModule} 
 */
function resolveDependencies(project, file) {
	if(project.dependencies[file.path]) return project.dependencies[file.path];

	const source = fs.readFileSync(file.path, "utf8");

	// Collect all includes
	/** @type {ProjectFile[]} */
	const includeFiles = [...source.matchAll(/^[\t ]*#include\s*"(.*?)"/gm)]
		.map(([m, name]) => name.endsWith(".h") ? path.join(HEADERS_DIR, name) : path.join(SOURCES_DIR, name))
		.map(e => absolutePathToProjectFile(project, e));


	project.dependencies[file.path] = {
		file: file,
		dependencies: includeFiles,
		dependents: [],
		isResolved: false
	};

	for(const includeFile of includeFiles) {
		if(project.dependencies[includeFile.path]) {
			project.dependencies[includeFile.path].dependents.push(file);
			continue;
		}

		const include = resolveDependencies(project, includeFile);
		include.dependents.push(file);
	}

	project.dependencies[file.path].isResolved = true;

	return project.dependencies[file.path];
}

/**
 * @param {Project} project
 * @param {string} _path
 * @return {ProjectFile} 
 */
function absolutePathToProjectFile(project, _path) {
	const exists = project.files.find(e => e.path === _path);
	if(exists) return exists;

	return {
		path: _path,
		relative: path.relative(project.root, _path),
		name: path.basename(_path),
		extension: path.extname(_path)
	};
}

/**
 * @param {Project} project
 * @param {string} name
 * @return {ProjectFile | undefined} 
 */
function getFileByName(project, name) {
	return project.files.find(e => e.name === name);
}

function prepareOutputDirectories() {
	// Create diretories
	Utils.createDirectory(OUTPUT_DIR);
	Utils.createDirectory(PROJECT_DIR);

	// Remove all existing files
	const files = Utils.readDirRecursive(OUTPUT_DIR);
	for(const file of files) {
		fs.unlinkSync(file);
	}
}

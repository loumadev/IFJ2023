//@ts-check

const fs = require("fs");
const path = require("path");
const {spawnSync} = require("child_process");
const {Utils} = require("../lib/Utils");

const ROOT = path.join(__dirname, "/..");
const SOURCES_DIR = path.join(ROOT, "/src");
const HEADERS_DIR = path.join(ROOT, "/include");
const MAIN = path.join(SOURCES_DIR, "/main.c");

const STATIC_DIR = path.join(__dirname, "/static_files");
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

		fs.writeFileSync(path.join(PROJECT_DIR, "/Makefile"), `##
# File: Makefile
# Author: deploy/deploy.js
# Date: ${new Date().toISOString().slice(0, 10)}
# Brief: This file is generated automatically. Do not edit manually!
##

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


	Utils.info("Injecting static files...");
	{
		const staticFiles = Utils.readDirRecursive(STATIC_DIR);
		for(const file of staticFiles) {
			const newPath = path.join(PROJECT_DIR, path.relative(STATIC_DIR, file));
			Utils.createDirectory(path.dirname(newPath));
			fs.copyFileSync(file, newPath);
		}
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


	Utils.info("Cleaning up...");
	{
		cleanProjectDirectory();
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

	// Add file header
	{
		const content = addFileHeader(file.path, source);
		if(content) {
			Utils.warn(`Updating header of file "${file.relative}"...`);
			fs.writeFileSync(file.path, content);
		}
	}

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

		try {
			const include = resolveDependencies(project, includeFile);
			include.dependents.push(file);
		} catch(err) {
			Utils.error(`Failed to resolve dependency "${includeFile.relative}" of file "${file.relative}"!`);
			throw err;
		}
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
	Utils.createDirectory(STATIC_DIR);
	Utils.createDirectory(OUTPUT_DIR);
	Utils.createDirectory(PROJECT_DIR);

	// Remove all existing files
	cleanOutputDirectories();
}

function cleanOutputDirectories() {
	// Remove all existing files
	const files = Utils.readDirRecursive(OUTPUT_DIR);
	for(const file of files) {
		fs.unlinkSync(file);
	}
}

function cleanProjectDirectory() {
	// Remove all existing files
	const files = Utils.readDirRecursive(PROJECT_DIR);
	for(const file of files) {
		fs.unlinkSync(file);
	}
}


/**
 * @param {string} filePath
 * @param {string} content
 * @return {string | null}
 */
function addFileHeader(filePath, content) {
	const projetName = "IFJ23";

	const defaultAuthor = "Author Name";
	const defaultEmail = "<xlogin00@stud.fit.vutbr.cz>";
	const defaultBrief = `This file is part of the ${projetName} project.`;

	// const stats = fs.statSync(filePath);

	// @ts-ignore
	const fileIdentifier = filePath.replace(ROOT, "").replaceAll(path.sep, "/").slice(1);
	const existingHeader = (content.match(/^\/\*\*\r?\n \*[\S\s]*?\*\/\r?\n\r?\n?/m) || [])[0];
	const existingFooter = (content.match(/\s*\/\*\* End of file .* \*\*\/\s*$/m) || [])[0];

	const authorsMatches = existingHeader && [...existingHeader.matchAll(/@author\s+(.*?)\s+(<.*?>)/g)];
	const authors = authorsMatches && authorsMatches.map(([m, name, email]) => ({name, email})) || [];

	const brief = existingHeader && existingHeader.match(/@brief\s+(.*)/)?.[1];

	if(authors.length === 0) authors.push({name: defaultAuthor, email: defaultEmail});

	if(!existingHeader) {
		Utils.warn(`File "${fileIdentifier}" is missing header! Creating new one...`);
	}

	if(authors.some(e => !e.name || !e.email || e.name === defaultAuthor || e.email === defaultEmail)) {
		Utils.warn(`File "${fileIdentifier}" is missing author information!`);
	}

	const header = `/**
 * @file ${fileIdentifier}
${authors.map(e => ` * @author ${e.name || defaultAuthor} ${e.email || defaultEmail}`).join("\n")}
 * @brief ${brief || defaultBrief}
 * @copyright Copyright (c) ${new Date().getFullYear()}
 */`;
	// * @date ${Utils.formatDate(stats.birthtime)}

	const footer = `/** End of file ${fileIdentifier} **/`;

	const newContent = header + "\n\n" +
		content
			.replace(existingHeader || "", "")
			.replace(existingFooter || "", "") +
		"\n\n" + footer + "\n";

	return newContent === content ? null : newContent;
}

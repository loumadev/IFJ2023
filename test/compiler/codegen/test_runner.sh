#! /usr/bin/bash

set -e

INTERPRETER_PATH="./test/compiler/codegen/ic23int"
CODEGEN_PATH="./test/compiler/codegen"
COMPILER_PATH="./bin/main"

function prepare_environment() {
   echo "🌱 Preparing test environment"

  echo "Running \"make build\""
  # Build binary
  if make build >/dev/null ; then
     echo "🌱 Build successful"
  else
     echo "❌ Build failed"
  fi
}

function run_compilation() {
  echo "📄 Running compilation"

  mkdir -p "$CODEGEN_PATH"/compiled_codes

  for file in "$CODEGEN_PATH"/swift_samples/*; do
     echo "Compiling $file"

     if ! ($COMPILER_PATH < "$file") > "$CODEGEN_PATH/compiled_codes/$(basename "$file").ifjcode"; then
        echo "❌ Compilation failed at file: $file"
        exit 1
     fi
  done

  echo "📄 Compilation complete"
}

function run_interpreter() {
  echo "🚀 Running interpreter"

  for file in "$CODEGEN_PATH"/compiled_codes/*; do
    echo "Running program: $file"
    echo "Interpreter output ⬇️"

    if $INTERPRETER_PATH "$file"; then
        echo ""
        echo "✅ Interpreter finished successfully"
    else
        echo ""
        echo "❌ Interpreter failed"
        exit 1
    fi
  done

  echo "🚀 Code interpreted"
}

function cleanup() {
  echo "🧹 Cleaning up"

  echo "Running \"make clean\" "
  make clean >/dev/null

  echo "Removing compiled codes"
  rm -rf "$CODEGEN_PATH"/compiled_codes

  echo "🧹 Cleanup complete"
}

# Run tests
cleanup
prepare_environment
run_compilation
run_interpreter

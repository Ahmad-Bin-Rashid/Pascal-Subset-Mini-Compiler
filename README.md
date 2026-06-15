# Pascal Subset Mini Compiler

A multi-parser Mini Compiler for a subset of the Pascal programming language, developed in C++17. This project takes a Pascal source file (`.pas`) as input and sequentially performs lexical analysis, grammar table construction, syntactic analysis using three distinct parsing strategies, semantic analysis, and Abstract Syntax Tree (AST) generation. All compilation results, logs, and generated tables are written to an output directory.

---

## Technical Features

1. **Double-Buffered Lexical Analyzer (`lexer.cpp`)**
   - Employs a character scanner utilizing two alternating 4096-byte buffers to achieve $O(1)$ memory scale overhead during file scanning.
   - Accurately tokenizes Pascal keywords, identifiers, operators, and literals.
   - Tracks line and column numbers for precise error reporting.

2. **Dynamic Grammar Table Builder (`grammar.cpp`)**
   - Automatically computes `FIRST` and `FOLLOW` sets from context-free BNF rules.
   - Dynamically constructs `LL(1)` parsing tables.
   - Generates `SLR(1)` Action and Goto tables dynamically, logging the canonical collection of LR(0) items.

3. **Multi-Strategy Parsing Engines**
   - **Recursive Descent (RD) Parser (`parser_rd.cpp`)**: A top-down approach that integrates seamlessly with the AST Builder and Semantic Analyzer.
   - **Predictive LL(1) Parser (`parser_ll.cpp`)**: A non-recursive, table-driven top-down parser. It utilizes an explicit symbol stack and features FOLLOW-based panic-mode error synchronization.
   - **SLR(1) LR Parser (`parser_lr.cpp`)**: A bottom-up shift-reduce parser. It leverages state and symbol stacks, logs detailed execution traces, and recovers from errors using synchronization states.

4. **Symbol Table Manager (`symbol_table.cpp`)**
   - Manages nesting scopes using a linked parent chain.
   - Supports recursive declarations, function/procedure parameter counts, type validations, and constant assignments.

5. **Abstract Syntax Tree (AST) Generation (`ast.cpp`)**
   - Constructs a hierarchical tree representation of the source code structure during the Recursive Descent parsing phase.

6. **Robust Error Handling (`error_handler.h`)**
   - Logs lexical, syntactic, and semantic errors and warnings with exact line and column positions.

---

## Directory Layout

```text
pascal_parser/
├── Makefile                   # Automation of build, clean, and test runs
├── README.md                  # Project overview and run guide
├── mini_compiler              # Compiled executable binary (generated after running make)
├── src/                       # C++ source code for lexer, parsers, AST, symbol table, etc.
├── docs/                      # D1/D3 technical reports, diagrams, and Viva Q&A
├── test/                      # Pascal test suite (valid and invalid programs)
│   ├── valid_program1.pas     # Valid test case 1
│   ├── valid_program2.pas     # Valid test case 2
│   ├── valid_program3.pas     # Valid test case 3
│   ├── err_lexical.pas        # Test case with lexical errors
│   ├── err_syntax.pas         # Test case with syntactic errors
│   └── err_semantic.pas       # Test case with semantic errors
├── obj/                       # Compiled intermediate object files (generated)
└── output/                    # Compilation logs and AST results from each compiler run
```

---

## Prerequisites

- **C++17 Compiler**: Ensure you have a modern C++ compiler installed (e.g., `g++` or `clang++`).
- **Make**: Required for building the project using the provided `Makefile`.

---

## How to Build & Run

### 1. Build the Compiler

Compile the source code and generate the binary `mini_compiler` in the root directory by running:

```bash
make
```

### 2. Run the Compiler

To compile a Pascal source file, use the following syntax:

```bash
./mini_compiler <source_file.pas> [output_directory]
```

**Example:**

```bash
./mini_compiler test/valid_program1.pas
```

By default, the compiler will execute the lexical analyzer, generate the grammar analysis report, run all three parsers, validate semantics, build the AST, and save all outputs to the specified output directory (default: `output/`).

### 3. Run the Automated Test Suite

To compile and run the compiler against all six test cases inside the `test/` directory, saving their outputs to `output/`:

```bash
make test
```

### 4. Clean Build Artifacts

To remove all compiled binaries, intermediate `.o` object files, and generated output logs, run:

```bash
make clean
```

---

## Group Members

- **Ahmad Bin Rashid**
- **Abubakar Munir**
- **Muhammad Omer Gazali**

---

## Course Details

- **Subject:** Compiler Construction Lab
- **Semester:** 6th

---

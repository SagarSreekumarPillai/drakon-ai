Here’s your **single copy-paste ready** `instructions.md` content that you can drop into your Drakon AI root folder.
It is designed for **Cursor** to follow an **end-to-end check → code → verify → debug → fix flow** for every file until Drakon AI is fully built.

---

```markdown
# Drakon AI – End-to-End Build & Verification Instructions for Cursor

## Objective
Fully build **Drakon AI** as a modular, from-scratch AI engine with:
- **Custom neural backend** (no llama.cpp, no cloning of existing models)
- **Locally runnable, open-source only stack**
- **Modular architecture** for integration into other MVPs
- **No reliance on paid APIs or hosted services**

The system must be **functional, tested, and debugged** end-to-end.

---

## Overall Process Cursor Must Follow

**For EACH file in the codebase:**
1. **Check**
   - If the file exists.
   - If it exists, **read and analyze its current content**.
   - Determine if it is:
     - ✅ Complete and functional
     - ⚠️ Needs modification
     - ❌ Missing

2. **Plan**
   - Based on architecture and dependencies, plan the required code.
   - Follow the naming conventions and folder structure strictly.

3. **Code**
   - If missing: **generate the full file**.
   - If incomplete or incorrect: **rewrite fully with corrected, optimized code**.
   - Use **clear, well-commented code** with docstrings for every function/class.

4. **Verify**
   - Ensure the file compiles with no syntax errors.
   - Ensure all imports and dependencies resolve.
   - Ensure logic is correct based on the project goal.

5. **Debug**
   - If errors appear during compilation or testing, **identify the cause**.
   - **Fix immediately** and retest until passing.

6. **Repeat**
   - Move on to the next file until all files are fully functional and verified.

---

## Key Architecture & File Layout

The project is expected to follow a **clean modular C++ + Python hybrid architecture**:

```

/drakon-ai
│
├── include/
│   ├── model\_loader.h
│   ├── tensor.h
│   ├── activation\_functions.h
│   ├── layers.h
│   ├── optimizer.h
│   ├── dataset.h
│   ├── config.h
│   └── utils.h
│
├── src/
│   ├── main.cpp
│   ├── model\_loader.cpp
│   ├── tensor.cpp
│   ├── activation\_functions.cpp
│   ├── layers.cpp
│   ├── optimizer.cpp
│   ├── dataset.cpp
│   ├── config.cpp
│   └── utils.cpp
│
├── python/
│   ├── train.py
│   ├── evaluate.py
│   ├── preprocess.py
│   ├── model\_export.py
│   └── utils.py
│
├── tests/
│   ├── test\_tensor.cpp
│   ├── test\_layers.cpp
│   ├── test\_optimizer.cpp
│   ├── test\_dataset.cpp
│   └── test\_end\_to\_end.cpp
│
├── CMakeLists.txt
├── requirements.txt
└── instructions.md

```

---

## Technical Guidelines

### C++ Layer (Core Engine)
- Handle **tensor math**, **matrix ops**, **activations**, **forward/backpropagation**, **optimizers**, and **dataset loading**.
- Must be **optimized for CPU first** (multi-threading optional for v0.1).
- Clean separation between **header files** (`include/`) and **implementations** (`src/`).

### Python Layer (Training & Utilities)
- Wraps around the C++ core via **pybind11** bindings.
- Handles **data preprocessing, training loops, evaluation, and model export**.
- Allows easy experimentation for AI models without touching C++ code.

### Testing & Verification
- Every C++ module must have a **corresponding test file** in `/tests`.
- Python functions must have **unit tests**.
- End-to-end test must:
  - Load a dataset
  - Train a small model
  - Evaluate accuracy
  - Save and reload model
  - Verify predictions are consistent

---

## Rules Cursor Must Follow

- **Never skip verification** after generating or editing a file.
- **Always check dependencies** before declaring a file complete.
- **Write code in one go per file** — no partial snippets.
- **Always document functions and classes**.
- If a bug is found later in another file, **fix it at the source**.
- Maintain **consistent formatting** across files.

---

## Final Deliverable
At the end of the process:
1. The entire Drakon AI project **compiles and runs without errors**.
2. End-to-end training → evaluation → save/load cycle works flawlessly.
3. Code is **fully documented and modular**.
4. All tests **pass successfully**.

---

**Prompt for Cursor to Execute:**

```

Follow the step-by-step process in instructions.md from the root directory.
Check each file in the Drakon AI project, determine if it exists and is complete,
then code, verify, debug, and fix it until it passes compilation and functional tests.
Do this for every file in the architecture until the entire system is fully functional,
tested, and documented as described.


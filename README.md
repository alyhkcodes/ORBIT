# ORBIT - Object Recovery Based on Integrated Tracking

ORBIT matches lost and found reports using a C++ core engine. The current user interface is a terminal application; reports are stored locally in `data/reports.json`. The Node.js and MongoDB backend is deferred.

## Project Structure
- `backend/Engine/` - C++ matching engine and terminal application
- `backend/` - deferred Node.js/Express API and MongoDB schema

## Terminal application

Build and run from the repository root (CMake fetches `nlohmann/json`):

```powershell
cmake -S backend/Engine -B backend/Engine/build
cmake --build backend/Engine/build --config Release
```

Run `orbit-engine.exe` from the generated build directory. With a multi-configuration generator, it is under `backend/Engine/build/Release/`; with a single-configuration generator, it is under `backend/Engine/build/`.

The menu lets you add lost or found reports, list saved reports, and exit. After each report, ORBIT displays up to five ranked matches. Data is saved in `data/reports.json` relative to the current working directory. If the file is missing, it is created when the first report is saved. Invalid saved JSON is reported and left untouched.

## JSON-lines engine

The `orbit-engine-jsonl` executable reads one JSON object per stdin line and emits one JSON response per line. Each request contains `lostItems`, `foundItems`, and optional `topK` (default `5`, maximum `100`). Item fields follow `backend/schema.js`; lost items require `reporterContact`, while found items require `finderContact` and `currentLocation`.

The local pipeline prunes by normalized category and month, expands description terms using trie-based edit-distance lookup, scores candidates with BM25 plus color, brand, location, and time signals, and returns the highest-scoring pairs. Gemini scoring is not part of this executable yet. Invalid request lines produce a JSON `error` response, and the process continues with the next line.

Engine code is split by responsibility: `Item.*` defines the item model, `JsonItemParser.*` validates and builds items from JSON, `Trie.*` handles typo lookup, `InvertedIndex.*` handles token indexing and BM25, `MatchingEngine.*` performs pruning and ranking, and `main.cpp` owns the stdin/stdout loop.

Example request:

```json
{"lostItems":[],"foundItems":[],"topK":5}
```

## Team
- Husain Khan, Aly
- Gupta, Priyanshu
- Singh, Gaurav
- Kotnala, Aditya

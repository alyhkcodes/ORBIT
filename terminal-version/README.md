# ORBIT Terminal Lost-and-Found

A beginner-friendly C++17 terminal application for reporting lost/found items and ranking possible matches.

## Files

- `item.h` — item data structure.
- `storage.h`, `storage.cpp` — save/load items from `orbit_data.txt`.
- `scoring.h`, `scoring.cpp` — BM25 scoring and inverted index.
- `matching.h`, `matching.cpp` — builds candidate documents and ranks them with a `priority_queue`.
- `main.cpp` — menu and user interaction.
- `Makefile` — build shortcuts.

The `engine/` folder is not needed by this standalone terminal version and is not included.

## Compile and run

### Linux / macOS / WSL / MinGW terminal

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp storage.cpp scoring.cpp matching.cpp -o orbit
./orbit
```

On Windows PowerShell after compiling with MinGW:

```powershell
.\orbit.exe
```

If `make` is installed, you can also run:

```bash
make
./orbit
```

## How matching works

1. Report a lost item and a found item using the menu.
2. Choose **Find matches** and enter the ID of the lost item.
3. The program builds a text document for every active found item.
4. BM25 scores the found-item documents against the lost item's title, category, description, and location.
5. A C++ `priority_queue` returns the highest score first. Only candidates with a score above zero are displayed.
6. Items are saved in `orbit_data.txt` so they remain after closing the program.

## Notes

- Matching is text-based. Similar words, spelling differences, and synonyms are not automatically understood.
- Date/time is stored and displayed, but is not currently used to filter candidate matches.
- This is a terminal prototype; it does not include login, a web frontend, or MongoDB.

# Quamium – Code Review (2025-12-15) by GPT-5.2 Extra-High

## Summary
- Browser skeleton stands up, but parser/layout/network glue has a few correctness gaps that will surface quickly in normal browsing (stateful lexer, host:port parsing, stale canvas on resize), and synchronous network calls currently block/crash the UI.
This review is based on reading the repository sources (no execution). The project is a great early-stage “browser skeleton” (fetch → tokenize → lay out → paint). The main opportunities are: (1) correctness/robustness in URL parsing and HTTP handling, (2) keeping the Qt UI responsive by moving networking off the UI thread, and (3) simplifying ownership/state so resize/clear behavior is predictable.
## Executive Summary (What to Fix Next)

### P0 – Correctness / “Will break quickly”
- **`server.cpp` URL parsing state reset is broken**: `scheme, host, port, path = ""` only clears `path` (comma operator). This can cause `host` and `port` to *accumulate across navigations* (e.g., `port` becoming `4438080`). Replace with explicit `.clear()` calls (or reinitialize locals, then assign to members once).
- **`server.cpp` HTTP (non-TLS) shutdown code won’t compile**: `shutdown(...)` returns `void` but is used in a boolean `if` expression. Split into two statements: call shutdown, then inspect `ec`.
- **UI thread is blocked by networking**: `Quamium::onSearchButtonClicked()` fetches synchronously and doesn’t handle exceptions; slow or failing requests will freeze or crash the app. Move networking off-thread or use Qt’s async networking.
- **`webcanvas.cpp` can resurrect stale content after “clear”**: `WebCanvas::clear()` empties the display list but keeps the previous `Layout` (and its stored tokens). A resize can re-layout and redraw the old page. Clear/reset the document/layout state, or gate `resizeEvent` if nothing is loaded.

### P1 – Security / Robustness
- **TLS hostname verification is missing**: `verify_peer` checks the chain but does not ensure the cert matches the requested host. Add hostname verification (`ssl::host_name_verification(host)` / `SSL_set1_host`) so a valid-but-wrong certificate is rejected.
- **No timeouts / size limits / status handling**: A slow server can hang forever; large responses can blow memory; 301/302/404/500 are treated like renderable HTML. Add timeouts, `res.result()` handling, redirect following (bounded), and body size caps.

### P2 – Performance / Maintainability
- **Hot-path copies in layout pipeline**: `Layout::layout(std::vector<Content> tokens, ...)` copies tokens multiple times; `WebCanvas::setDisplayList` copies `Layout` again. Prefer `const&`/move + store only what’s needed for relayout.
- **Layout does per-word allocations and per-word `QFontMetrics` creation**: This becomes expensive as pages grow; cache metrics while the font stays constant and avoid allocating a `std::vector<std::string>` just to iterate words.

## File-by-File Review

### `server.h` / `server.cpp`
- **Separate responsibilities**: `Server` should be a network/URL client; it shouldn’t include UI (`webcanvas.h`) or contain a `WebCanvas` member. This tight coupling increases build times and makes testing difficult.
- **Make `httpGet`’s API honest**: `httpGet(std::string url)` ignores its parameter and relies on member state (`host/port/path/scheme`). Either:
  - parse inside `httpGet(url)` and keep it stateless, or
  - introduce a small `ParsedUrl { scheme, host, port, target }` struct and pass that through.
- **Fix URL parsing initialization**: use `scheme.clear(); host.clear(); port.clear(); path.clear();` (or locals) and handle:
  - `:port` in the displayed URL (include it in `url` if non-default),
  - fragments (`#...`) not being part of HTTP requests,
  - whitespace trimming and basic validation (reject empty host unless you intentionally default it).
- **Handle errors without crashing**: use `boost::system::error_code` overloads (or `try/catch`) and return an error type (or empty string + error message) rather than throwing into the UI.
- **HTTP response handling**: consider:
  - redirects (`Location:`) with a max depth,
  - gzip/deflate (optional; can defer),
  - content-type checks (render only `text/html` initially),
  - a body size limit via Beast’s parser (`body_limit`).
- **Logging**: prefer consistent logging (Qt logging categories, or a single `std::cerr` stream). Avoid string concatenation inside streams (`"x: " + input`) to reduce allocations.

### `quamium.h` / `quamium.cpp` / `main.cpp` / `quamium.ui`
- **UI responsiveness**: do not run network I/O inside a slot. Prefer:
  - `QNetworkAccessManager` + `QUrl` (simplest in Qt), or
  - `boost::asio`/Beast on a worker thread with a signal to deliver HTML back to the UI thread.
- **Error UX**: display errors in the status bar / a label (DNS failure, TLS failure, HTTP status, empty body).
- **Sizing**: the layout width currently uses a fixed `width` member; use the scroll-area viewport width (or the canvas width) for initial line breaking so the first render matches the actual window size.
- **Qt Designer connections**: `quamium.ui` contains a connection from `searchButton.click()` to `searchBar.copy()`, which is likely accidental. Remove or replace with an intentional action.
- **Translations**: `main.cpp` tries to load `:/i18n/Quamium_*`, but the project has no `.qrc` resources and the `.ts` file is empty. Either:
  - load `.qm` files from disk, or
  - add a resource file and embed translations, or
  - remove translation scaffolding until it’s used.

### `lexer.h` / `lexer.cpp`
- **Statefulness is already handled well**: `lex()` clears `out` and `buffer`, avoiding cross-navigation contamination.
- **Next incremental step**: split “tokenization” from “parsing”. Right now tags are raw strings; even a minimal tag parser (`name`, `is_end_tag`, `attributes`) will simplify layout and reduce string fiddling.
- **Robustness** (future): handle comments (`<!-- -->`), `<script>/<style>` raw text, and entity decoding (`&amp;`, `&lt;`, …). These can be added progressively as you follow Browser Engineering.

### `layout.h` / `layout.cpp`
- **Avoid copies**: iterate `for (const Content& tok : tokens)` and accept tokens by `const&` (or by value + `std::move` into the member) to prevent multiple large copies.
- **Tag handling**: the `unordered_map<std::string, int>` + `switch` works, but gets hard to extend. Consider:
  - an enum (`enum class Tag { BoldOn, BoldOff, ... }`) or
  - a small function `applyTag(tagName, fontState, cursorState)` with a style stack so nested tags restore correctly.
- **Font sizing**: `<h1>` etc multiply the current size; if headings nest or appear repeatedly without a clean reset, sizes can compound. Prefer fixed sizes per heading level (and restore prior style on end tags).
- **Whitespace/line breaks**: HTML collapses runs of whitespace; consider normalizing whitespace in the tokenizer/parser rather than splitting by `' '` and skipping empties. Add support for `<br>` and basic block spacing rules.
- **Undefined behavior risk**: `std::transform(..., ::tolower)` should cast to `unsigned char` inside a lambda to avoid UB for non-ASCII bytes.

### `webcanvas.h` / `webcanvas.cpp`
- **Document model**: keep a clear “document loaded?” state. Right now `Layout la` holds tokens and is used for relayout on resize; `clear()` should reset that state (tokens + content size) so resizing cannot recreate old content.
- **Relayout policy**: `resizeEvent` always reflows at `0.95 * width()`. Consider using the scroll-area viewport width and a stable margin model (avoid magic `0.95`), and throttle relayout during continuous resizing.
- **Paint performance** (later): for larger pages, store bounding boxes in `DisplayText` to avoid recomputing `QFontMetrics` in the paint loop, or use `QStaticText`.

### `structs/content.h` / `structs/DisplayText.h`
- **Header hygiene**: put `#pragma once` first; include the headers you actually rely on (`QFont`, `QColor`) rather than indirectly via `qtextformat.h`.
- **Initialize fields**: `DisplayText` has `color` but paint ignores it; either initialize it and start using it, or remove it until colors are supported.

### `utils/utils.h` / `utils/utils.cpp`
- **Minimize includes**: `utils.h` includes several unused headers; trimming speeds incremental builds.
- **Split function semantics**: current `split` returns empty segments for consecutive/trailing delimiters. That’s OK, but document the behavior or adjust if other callers expect “skip empties”.

## Build System & Repo Hygiene

### `CMakeLists.txt`
- **Portability**: remove the hard-coded `CMAKE_PREFIX_PATH` pointing to `/home/pquam/...`; rely on user/CI-provided `CMAKE_PREFIX_PATH` or use `CMakePresets.json`.
- **Modern CMake**: consider moving from global settings to target-based:
  - `target_compile_features(Quamium PRIVATE cxx_std_17)`
  - warnings (`-Wall -Wextra -Wpedantic` etc.) per compiler
  - `CMAKE_EXPORT_COMPILE_COMMANDS` for tooling
- **Resources**: `CMAKE_AUTORCC` is enabled but no `.qrc` exists. Either add resources (icons/translations) or disable it.

### Repo files
- **Don’t commit per-user IDE files**: add `CMakeLists.txt.user` to `.gitignore` (Qt Creator user-specific). `.vscode/` is already ignored, but if it’s committed, consider removing it from version control and providing a template instead.

## Suggested Roadmap (Incremental, High-Value)
1. **Stabilize networking**: fix URL parsing reset, add timeouts/status checks/body limits, add hostname verification, and stop blocking the UI thread.
2. **Introduce a small “Document” model**: store HTML, tokens, and display list in one place; make clear/load/relayout deterministic.
3. **Minimal HTML parser**: convert tokens to a small DOM/tree; treat `<p>`, headings, lists, `<br>`, and text nodes more systematically.
4. **Performance pass**: remove hot-path copies; cache metrics; avoid per-word allocations.
5. **Add tests**: unit-test URL parsing, lexer/tokenizer, and a couple of layout cases to prevent regressions as you expand functionality.
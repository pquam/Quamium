# Quamium — Comprehensive Code Review ChatGPT5.1-codex-max extra-high

Date: 2025-12-18  
Scope: current repository state (Qt Widgets app + simple HTML lexer/layout/renderer + Boost.Beast HTTP client)

This review focuses on: correctness, performance, responsiveness (GUI), maintainability, and code style. No code changes are made here—only recommendations.

---

## Executive Summary (Highest Impact First)

1. **Avoid blocking the UI thread**: `Server::httpGet()` is synchronous and is called directly from `Quamium::onSearchButtonClicked()`. Move networking (and ideally parsing/layout too) off the GUI thread, or switch to Qt’s async networking.
2. **Fix layout state/reset bugs**: `Layout::initialLayout()` doesn’t reset cursor/font state, so subsequent page loads can inherit previous layout state; `<ul>` handling mutates `HSTEP` and never resets reliably.
3. **Remove large unintended copies** across modules (lexer/layout/rendering). Several APIs return vectors by value or store large objects by value, causing unnecessary allocations and slowdowns as pages grow.
4. **Harden networking/error handling**: handle HTTP status codes, redirects, failures, and TLS hostname verification (not just CA verification).
5. **Standardize code style and header hygiene**: missing include guards, duplicated includes, heavy includes in headers, inconsistent indentation, and debug `std::cout` calls in core hot paths.

---

## Build System / Repo Hygiene (CMake, project files)

### CMake configuration
- **Don’t hard-code developer-local Qt paths** in `CMakeLists.txt`:
  - Current: `set(CMAKE_PREFIX_PATH "/home/pquam/Qt/6.10.0/gcc_64" ...)`.
  - Recommendation: remove the hard-coded path and rely on `-DCMAKE_PREFIX_PATH=...` (as your README already describes), or gate it behind an option/env var.
- Consider enabling **warnings and treating warnings as errors** in Debug/CI builds (per-compiler):
  - GCC/Clang: `-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion`
  - MSVC: `/W4` (and optionally `/WX`)
- Consider enabling **sanitizers** in a dedicated build preset (ASan/UBSan) for development.

### Tracked IDE/user files
- `CMakeLists.txt.user` is a Qt Creator user-specific file and usually should **not** be tracked in git. Recommendation: remove it from version control and add it to `.gitignore`.

### Structure suggestions (optional)
- If the project grows, consider splitting into directories:
  - `src/` (app + browser pipeline)
  - `include/` (public headers)
  - `tests/`
  - `resources/` (default pages, etc.)

---

## Architecture & Separation of Concerns

Right now the main pipeline is essentially:
`Quamium (UI) -> Server (network fetch) -> Lexer (tokenize) -> Layout (layout display list) -> WebCanvas (paint)`

Recommendations:
- **Separate “document model” from rendering**:
  - The `Layout` object currently owns tokens and also owns the output display list.
  - Consider a clearer model:
    - `Document` owns raw HTML, parsed tokens/nodes
    - `LayoutEngine` produces a `DisplayList`
    - `Renderer` draws the `DisplayList`
- **Make `Server` stateless (or clearly stateful)**:
  - `Server::httpGet(std::string url)` takes a URL but uses member `host/port/path` instead; this is surprising. Either:
    - pass in a parsed URL struct and use it, or
    - make `httpGet()` operate purely on members and remove the parameter.
- Introduce lightweight value types:
  - `Url { scheme, host, port, path }` (parsed once, reused)
  - `RequestResult { status_code, headers, body, error }`

---

## UI Thread Responsiveness (Qt-specific)

### Synchronous network calls will freeze the app
- `Quamium::onSearchButtonClicked()` performs a blocking DNS resolve, TCP connect, TLS handshake, HTTP request, and full response read on the **GUI thread**.
- Recommendation options:
  1. Use **Qt networking** (`QNetworkAccessManager`) for async I/O and simpler TLS handling.
  2. Keep Boost.Beast but run it in a **worker thread** (`QThread`, `QtConcurrent::run`, or `std::jthread`) and deliver results back via signals/slots.

### Progress / cancellation UX
- Add a minimal loading state:
  - disable search button while loading
  - show “Loading…” in status bar
  - allow cancel/stop (even if initially it only ignores late responses)

---

## Correctness & Functional Issues

### Layout state reset on new pages
- `Layout::layout(int page_width)` resets layout state via `layoutReset()`.
- `Layout::initialLayout(...)` currently does **not** reset state before laying out new tokens; it only copies tokens and calls `layoutHelper()`.
- Recommendation: ensure new page loads always start from a clean layout state (cursor positions, font, indentation, etc.).

### `<ul>` indentation mutates `HSTEP`
- In `Layout::tagHandler()`, `<ul>` and `</ul>` change `HSTEP` directly (`HSTEP += 13` / `HSTEP -= 13`).
- This is risky because `HSTEP` is also used as your left margin baseline; if tags mismatch or layout is reused, indentation can drift.
- Recommendation: keep `HSTEP` constant as a base margin, and track indentation separately (e.g., `indent_level` or a stack).

### Font sizing logic and nesting
- Headings modify `size` multiplicatively (`size = size * 1.5`, etc.) but then reset to `16` on closing tags.
- This will behave poorly with nested tags or multiple headings in a row.
- Recommendation: manage font state with a stack (push current style on open tag, pop on close).

### Possible bug in line metrics calculations
- In `Layout::addLineToList()`:
  - `max_desc` is computed using `std::max(max_ascent, line_metrics.descent())`, which looks like a typo (it compares against `max_ascent` rather than `max_desc`).
  - `max_desc` is then not used in baseline calculations.
- Recommendation: revisit baseline/line spacing calculations and ensure ascent/descent are handled correctly.

### URL parsing and composition
- `Server::parseInputToURL()` parses a `port` but the returned `url` omits it (`scheme + "://" + host + path`).
- Recommendation: include `:port` in the URL when non-default, or avoid building a string URL if the real source of truth is `scheme/host/port/path`.

### `.ui` connections that likely don’t work
- In `quamium.ui`, there is a connection using `<signal>click()</signal>` on the search button. `click()` is typically a **slot**, not a signal.
- Recommendation: fix/remove this in Qt Designer (should probably be `clicked()`), and avoid accidental runtime warnings.

### Shadowed variables (readability + potential bugs)
- In `Quamium::loadDefault()`, a local `std::string body` shadows the `Quamium::body` member. In `Quamium::onSearchButtonClicked()`, a local `tokens` shadows the member `tokens`.
- This isn’t necessarily incorrect today, but it makes future changes error-prone.
- Recommendation: avoid shadowing members; use distinct names or assign directly to the member when that’s the intent.

### Tag lowercasing correctness
- `Layout::tagHandler()` lowercases via `std::transform(..., ::tolower)`. `::tolower` is undefined for negative `char` values unless cast to `unsigned char`.
- Recommendation: use a safe cast (or a lambda) when lowercasing.

### Resize relayout / repaint flow
- `WebCanvas::resizeEvent()` recalculates layout and updates internal state, but doesn’t explicitly call `update()`.
- Recommendation: ensure a repaint is triggered after relayout (especially if you later add more complex invalidation/caching).

---

## Performance & Memory Optimizations

### Avoid copying large vectors and strings
Key copy hot-spots:
- `Lexer::lex(std::string body)` takes the HTML body by value (copies it).
  - Recommendation: accept `std::string_view` or `const std::string&`.
- `Lexer` stores `out` as a member and returns it by value. Returning a member generally forces a **copy** (not NRVO).
  - Recommendation: build the output as a local variable and return it (enables move/NRVO), or return via output parameter.
- `Layout::getDisplayList()` returns `std::vector<DisplayText>` by value.
  - Recommendation: return `const std::vector<DisplayText>&` (or `std::span<const DisplayText>`).
- `WebCanvas::start(const Layout& layout)` stores a full `Layout` **by value** and then copies out the display list again.
  - Recommendation: pass/store only the `DisplayList` needed for painting, or store a `std::shared_ptr<const Layout>` if you truly need the whole engine state.

### Reduce per-frame cost in painting
- `WebCanvas::paintEvent()` constructs `QFontMetrics` for every `DisplayText` each paint.
  - Recommendation: store ascent/descent/height alongside each `DisplayText`, or batch by font, or reuse cached metrics.
- Remove `std::cout` logging in hot paths:
  - `Layout::addToLine()` prints every word—this will dominate runtime on non-trivial pages.
- Consider reducing per-item payload:
  - `DisplayText` stores a full `QFont` per item; this can be memory-heavy for large pages. Over time you may want a font table (id -> font) or grouping by font.

### Hash key construction overhead
- Layout cache key uses `font.toString().toStdString()` per word.
  - Recommendation: compute a stable font key once when font changes (e.g., store current font key string), then combine with the word hash.

---

## Error Handling, Robustness, and Security

### Networking
- Handle and surface:
  - DNS failures, connect timeouts, handshake failures
  - non-200 HTTP responses (404/500)
  - redirects (301/302) with a max redirect count
  - content encoding and transfer encoding (chunked is handled by Beast, but compression isn’t)
- Consider TLS hostname verification:
  - CA validation alone doesn’t guarantee the cert matches the host unless hostname verification is configured.
  - If staying with OpenSSL/Beast, look into setting host verification (e.g., `SSL_set1_host` / verify callback).

### File loading
- `Quamium::loadDefault()` should handle file open errors visibly (status bar message, fallback page) and rely on RAII (`std::ifstream`) rather than manual `close()`.

### Input validation
- `parseInputToURL()` currently accepts a broad range of inputs; consider:
  - trimming whitespace
  - rejecting unsupported schemes
  - normalizing common user inputs (`example.com` -> `https://example.com/`)

---

## Code Style & Maintainability

### Header hygiene
- Add include guards or `#pragma once` consistently (currently missing in `server.h` and `lexer.h`).
- Avoid heavy includes in headers unless needed:
  - e.g., `<iostream>` in headers causes needless rebuilds and longer compile times.
- Prefer forward declarations where possible (especially in `quamium.h`).
- Specific quick wins:
  - `quamium.h` includes `webcanvas.h` twice.
  - `quamium.h` includes the generated `ui_quamium.h`; consider forward-declaring `Ui::Quamium` in the header and including `ui_quamium.h` only in `quamium.cpp` to reduce rebuild cost.

### Const-correctness and signatures
- Prefer `const` references for read-only inputs:
  - `setInput(const std::string&)`, `parseInputToURL(const std::string&)`, etc.
- Mark getters `const`:
  - `std::string getInput() const`
- Mark methods that don’t modify state as `const` where applicable.

### Naming and consistency
- Avoid single-letter member names like `l`, `la`, `w`, `h` for long-term readability.
- Consider a consistent naming convention:
  - Types: `PascalCase`
  - Functions/vars: `camelCase` or `snake_case` (pick one)
  - Members: suffix/prefix (e.g., `m_layout`, `m_tokens`) if you prefer Qt style.

### Logging
- Prefer Qt logging (`qDebug()`, `qWarning()`) over `std::cout`/`std::cerr` in a Qt GUI app.
- Gate verbose logs behind a debug flag to keep release builds clean.

### Formatting
- Consider adding a `.clang-format` and formatting the project to eliminate indentation inconsistencies (especially in `server.cpp`).

---

## Testing Recommendations (Low Effort, High Value)

Add focused unit tests for the pieces that are easiest to test deterministically:
- `Server::parseInputToURL()` (inputs like `example.com`, `https://a.com:8443/x`, empty string)
- `Lexer::lex()` (simple HTML fragments; ensures tag/text classification is correct)
- Layout behavior for wrapping and basic tags (small deterministic cases)

If you prefer staying within Qt tooling, `QtTest` works well; otherwise Catch2/doctest are lightweight.

---

## Suggested Next Steps (Prioritized)

1. Move networking off the GUI thread (async fetch + UI updates).
2. Fix layout reset/indentation state and remove debug prints in hot paths.
3. Refactor APIs to avoid large copies (`Lexer`, `Layout`, `WebCanvas`).
4. Improve HTTP/TLS robustness (status handling, redirects, hostname verification).
5. Add small unit tests to lock in behavior as you iterate.

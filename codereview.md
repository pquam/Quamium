# Quamium — Comprehensive Code Review (Recommendations)

Date: 2025-12-18  
Reviewed revision: `1c1691a`  
Scope: Qt Widgets UI + simple HTML lexer/layout/renderer + Boost.Beast HTTP(S) client  

This document contains recommendations only (no application code was modified as part of this review).

---

## Executive Summary (Highest Impact First)

### P0 — Fix first (correctness/security/UX)

1. **Rendering bug: initial display list is ignored**
   - `webcanvas.cpp:10-14`: `WebCanvas::start(const std::vector<DisplayText>& displayList, ...)` assigns `this->display_list = display_list;` (member-to-member) instead of using the `displayList` parameter.
   - Impact: `Quamium` calls `start()` after loading (`quamium.cpp:69`, `quamium.cpp:93`), so the page may render empty even when layout produced content.

2. **TLS is missing hostname verification (MITM risk)**
   - `server.cpp:145-181`: `verify_peer` is set and SNI is configured, but there’s no explicit hostname verification step (CA validation alone doesn’t prove the certificate matches `host`).
   - Recommendation: use Boost.Asio’s `ssl::host_name_verification(host)` verify callback (or OpenSSL `X509_VERIFY_PARAM_set1_host` / `SSL_set1_host`).

3. **UI freezes during network fetch and parse/layout**
   - `quamium.cpp:76-95`: synchronous `Server::httpGet()` (DNS + connect + TLS + read) runs on the Qt GUI thread.
   - Recommendation: move network fetch (and ideally tokenization/layout) off-thread, or switch to Qt’s `QNetworkAccessManager` for async networking.

### P1 — Correctness/behavior gaps

4. **URL parsing bug when input omits scheme but includes port**
   - `server.cpp:71-75`: when there is no `://`, `port` is preset to `"443"` before parsing.
   - Example: input `example.com:8443/` will append digits to the existing default, producing `port == "4438443"`.
   - Recommendation: don’t prefill `port`; parse first, then apply defaults.

5. **Layout height tracking is incorrect and can use stale state**
   - `layout.cpp:261-287`: `content_height` uses `h` (height of the *last* measured word) at `layout.cpp:280`.
   - If `addLineToList()` is called when `line` is empty (possible via tag handling), `h` may be stale, and `content_height` becomes inconsistent.
   - Recommendation: compute line height from `max_ascent + max_desc` (or `QFontMetrics::height()` max across the line), and guard against empty `line`.

6. **Scroll area sizing is not updated on relayout**
   - `webcanvas.cpp:16-19`: `setDisplayList()` ignores `contentSize` (no `setMinimumSize()`), so scrollbars/content extents can get out of sync after relayout.

### P2 — Maintainability/perf improvements

7. **CMake portability issues**
   - `CMakeLists.txt:13`: hard-coded `CMAKE_PREFIX_PATH` points at a developer-local Qt path.
   - `CMakeLists.txt:16`: `cmake_policy(SET CMP0167 NEW)` can break configuration on CMake versions that don’t know CMP0167 (while `cmake_minimum_required` is 3.16).

---

## Build System / Repo Hygiene

### CMake

- **Remove developer-local Qt paths from source control** (`CMakeLists.txt:13`).
  - Prefer documenting `-DCMAKE_PREFIX_PATH=...` (already in `README.md`) or provide a cache option.

- **Guard new CMake policies or bump minimum version** (`CMakeLists.txt:1`, `CMakeLists.txt:16`).
  - If keeping the 3.16 minimum, wrap it:
    - `if(POLICY CMP0167)
         cmake_policy(SET CMP0167 NEW)
       endif()`
  - Or, bump `cmake_minimum_required(...)` to the first version that supports CMP0167.

- **Add warnings/sanitizers presets** (high value in a browser-like project):
  - GCC/Clang: `-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion`
  - Debug preset: ASan/UBSan

### Git / ignored files

- `.cache/` is untracked (seen in `git status`) and is typically IDE/build byproduct; consider adding it to `.gitignore`.
- `CMakeLists.txt.user` is typically QtCreator-user-specific; consider ignoring it rather than tracking.

### Qt resources

- `main.cpp` loads translations from `:/i18n/...`, but there is no `.qrc` file in the project sources.
  - Recommendation: add a `.qrc` and embed translation `.qm` files (or load `.qm` from disk beside the binary).

- Default pages are read from a relative filesystem path (`quamium.cpp:46-47`).
  - Recommendation: embed `defaultpages/*` in Qt resources so the app runs reliably after install.

---

## UI / Qt Integration

### Don’t do blocking work on the GUI thread

- `quamium.cpp:76-95` currently blocks the UI for the entire fetch+parse+layout pipeline.
- Recommendations:
  - **Preferred (Qt-native)**: use `QNetworkAccessManager`, connect to `finished(QNetworkReply*)`, and update the UI when the reply completes.
  - **If keeping Boost.Beast**: run `Server::httpGet()` in a worker (`QThread`, `QtConcurrent::run`, or `std::jthread`) and marshal results back with signals.

### `.ui` and runtime layout changes

- `quamium.ui` defines a `gridLayoutWidget` with a `QScrollArea` (`webCanvas`) already placed.
- `Quamium::Quamium()` then creates a new `QVBoxLayout` and moves widgets into it (`quamium.cpp:17-23`).
  - Recommendation: choose one source of truth:
    - either fully define layout in `.ui`, or
    - keep `.ui` minimal and build the layout in code.
  - Mixing the two often leads to warnings (reparenting widgets, geometry fighting layouts).

### Suspicious `.ui` connections

- `quamium.ui` contains:
  - `searchBar.returnPressed()` → `searchButton.click()` (reasonable), and
  - `searchButton.click()` → `searchBar.copy()` (likely accidental), and
  - uses `click()` as a “signal” (it is normally a slot).
- Recommendation: clean this up in Qt Designer and rely on the explicit C++ connect (`quamium.cpp:38`).

---

## Networking (`Server`)

### Security: verify the hostname

- Current TLS setup (`server.cpp:142-162`) does CA verification and SNI.
- Recommendation: add explicit hostname verification:
  - Asio: `stream.set_verify_callback(ssl::host_name_verification(host));`
  - Or OpenSSL host checks via `X509_VERIFY_PARAM_set1_host` / `SSL_set1_host`.

### Robustness: timeouts, status codes, redirects, and errors

- Add timeouts on resolve/connect/handshake/read (Beast supports `expires_after`).
- Check `res.result()` and handle non-2xx responses.
- Implement basic redirect handling (301/302/307/308) with a max redirect limit.
- Consider body size limits (avoid reading multi-GB responses into memory).
- Surface errors to the UI (status bar) instead of `std::cout`.

### URL parsing correctness

- `parseInputToURL()`:
  - Should not pre-fill `port` before parsing (`server.cpp:71-75`).
  - Builds `url` without `:port` (`server.cpp:115`), which is misleading if a non-default port was parsed.
  - Performs redundant self-assignments (`server.cpp:117-120`) because `scheme/host/port/path` are already members.
- Recommendation:
  - Prefer a real URL parser:
    - Qt: `QUrl` (already a dependency)
    - Boost: `boost::urls` (if available in your Boost version)
  - Model URL as a small struct, not parallel strings.

---

## Lexing / Parsing (`Lexer`)

### API shape and copies

- `Lexer::lex(std::string &body, std::vector<Content> &out)` takes a non-const `body` even though it doesn’t mutate it (`lexer.cpp:18-26`).
- It also returns `std::vector<Content>` by value (`lexer.cpp:4`, `lexer.cpp:30`), which encourages accidental extra copies (e.g., `tokens = l.lex(body, tokens)` in `quamium.cpp:60`, `quamium.cpp:84`).
- Recommendation:
  - Make `body` `const std::string&`.
  - Make `lex(...)` return `void` (or return a fresh vector and don’t take `out`).

### HTML correctness limitations (fine for early stages, but be explicit)

- This lexer treats everything between `<` and `>` as a tag, and everything else as text.
- Improvements to consider:
  - Trim tag names and handle self-closing tags like `<br/>` (currently becomes `"br/"` and won’t match your tag table).
  - Handle comments (`<!-- -->`), doctype, and `<script>/<style>` content.
  - Decode common HTML entities (`&amp;`, `&lt;`, etc.).

---

## Layout (`Layout`)

### State reset and invariants

- `Layout::initialLayout(...)` doesn’t call `layoutReset()` (`layout.cpp:9-16`), so if `Layout` is reused across documents, state can leak (cursor position, font state, indentation).
- `HSTEP` is mutated for `<ul>` (`layout.cpp:172-178`) but is also used as the base left margin.
  - Recommendation: keep a constant base margin and track indentation separately (e.g., `indent_level`).

### Tag handling and nested styles

- Current tag system is a string→int map with a big switch (`layout.h:81-108`, `layout.cpp:83-185`).
- This makes nesting hard to get right (e.g., `<b><i>text</i></b>`).
- Recommendation:
  - Use a **style stack**: push current style on open tags, pop on close tags.
  - Replace int codes with an enum (`enum class TagAction`) or separate handler functions.

### Correctness: line metrics and content height

- `addLineToList()` computes `max_ascent`/`max_desc` (`layout.cpp:261-287`) but uses `h` for content height (`layout.cpp:280`).
- If `line` is empty, the function still updates cursor/height based on stale `h`.
- Recommendations:
  - Early return if `line.empty()`.
  - Set `content_height` based on the actual line metrics (`baseline + max_ascent + max_desc`, etc.).

### Performance hot spots

- `Layout::wordHandler()` builds cache keys via `font.toString().toStdString()` per word (`layout.cpp:210`, `layout.cpp:294`). This is expensive.
- The cache key is a `size_t` hash; collisions are possible (rare, but catastrophic for layout correctness).
- Recommendations:
  - Use a structured key: `(word, fontKey)` rather than `hash(string)`.
  - Cache a font key string once per font change, not per word.
  - Avoid repeated QString/std::string conversions in the hot path.

### Character casing correctness

- `std::transform(..., ::tolower)` (`layout.cpp:78`) can be undefined for negative `char` values.
- Recommendation: cast to `unsigned char` in the tolower call.

---

## Rendering (`WebCanvas`)

### Correctness bugs

- `WebCanvas::start()` ignores its `displayList` parameter (`webcanvas.cpp:10-14`). Fixing that is likely required for any initial render.

### Scroll extents and relayout

- `setDisplayList(..., QSize contentSize)` doesn’t use `contentSize` (`webcanvas.cpp:16-19`).
- Recommendation: update the widget’s minimum size (or size hint) whenever content size changes so scroll bars track document size.

### Event handling

- `wheelEvent()` overrides the handler but just calls `QWidget::wheelEvent(ev)` (`webcanvas.cpp:58-61`). Depending on Qt’s default handling, this can prevent the scroll area from scrolling.
- Recommendation: consider removing the override or explicitly `ev->ignore()` to let the parent `QScrollArea` handle it.

### Paint performance

- `paintEvent()` constructs `QFontMetrics` per `DisplayText` each paint (`webcanvas.cpp:45-54`).
- Recommendations:
  - Cache per-font metrics or store ascent/descent in the display list.
  - Fill only the paint region (`ev->rect()`) instead of `rect()` for large documents.
  - Long-term: draw per-line or use `QStaticText` to reduce layout overhead.

---

## Data Structures / Headers / Code Style

### Header hygiene

- Several headers include heavy/unused headers (e.g., `<iostream>` in headers) which increases compile times.
- Prefer:
  - include what you use,
  - forward declare where possible,
  - keep `#pragma once` first in the file.

### Const correctness and moves

- Prefer `const std::string&` for read-only inputs (`Server::getInput`, `parseInputToURL`, `Lexer::lex`, etc.).
- If taking by value, move into members (`setInput(std::string input) { this->input = std::move(input); }`).

### Naming

- Consider more descriptive member names than `l`, `la`, `w`, `h` (readability will matter quickly as the engine grows).

---

## Testing & Tooling (High Value as Complexity Increases)

- Add small deterministic tests for:
  - URL parsing edge cases (`example.com`, `example.com:8443/path`, `http://...`, empty input)
  - lexer output for simple HTML fragments
  - layout wrapping and basic tag effects
- Suggested tooling:
  - `clang-format` for consistent formatting
  - `clang-tidy` for correctness/perf checks
  - ASan/UBSan builds for memory/UB issues

---

## Suggested Next Steps (Pragmatic Roadmap)

1. Fix `WebCanvas::start` parameter bug and ensure scroll extents update on relayout.
2. Move network fetch off the UI thread (Qt async networking is the simplest path).
3. Harden TLS (hostname verification + timeouts + error reporting).
4. Replace ad-hoc URL parsing with `QUrl` (or Boost.URL), and add tests for it.
5. Stabilize layout invariants (proper content height, guard empty lines, style stack).
6. Add a minimal test suite and formatting/linting to keep velocity high.

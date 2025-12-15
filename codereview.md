# Code Review

## Summary
- Browser skeleton stands up, but parser/layout/network glue has a few correctness gaps that will surface quickly in normal browsing (stateful lexer, host:port parsing, stale canvas on resize), and synchronous network calls currently block/crash the UI.

## Findings


- High – `quamium.cpp:45-58` and `server.cpp:130-202`: The search handler performs the network fetch synchronously on the UI thread and propagates any Beast/OpenSSL exceptions. A slow or failing server will freeze the window or crash the app. Wrap the fetch in async/off-thread work (or at least guard with try/catch and surface errors to the UI) to keep the UI responsive.
- Medium – `webcanvas.cpp:59-68`: `clear()` drops the display list but leaves `Layout la` intact. Any resize between searches rebuilds and re-renders the previous page because `resizeEvent` re-lays out whatever tokens `la` still holds. Minimum size also remains at the old content dimensions, so scrollbars linger after clearing. Reset the layout state and size when clearing or gate `resizeEvent` when no document is loaded.
- Low – `layout.cpp:67-149`: Tag map includes `<small>/<big>` but the size adjustments are commented out, so those tags silently do nothing. Header sizing multiplies the current size instead of starting from a baseline; repeated headings compound the font size until reset to 16px on close. Consider normalizing font sizing per heading level for consistent typography.

## Additional Findings / Nitpicks
- Medium – `server.cpp:61`: Accessing `input[4]` after finding `"://"` assumes the string is long enough; short inputs like `a://` or malformed schemes read past the end. When no scheme is supplied, prior `scheme`/`port` values are reused, so a single HTTPS visit makes all later scheme-less inputs default to HTTPS:443.
- Medium – `server.cpp:130-202`: `httpGet` treats any HTTP status as success and ignores redirects, timeouts, or body size limits. A 404/500 is rendered as if it were content, and a slow server can hang indefinitely. Add timeouts, check `res.result()`, and consider size caps.
- Low – `CMakeLists.txt:15`: Hard-coded `CMAKE_PREFIX_PATH` to a user-specific Qt install hurts portability and CI; better to document it and let users/CI set the path.
- Low – `quamium.h:4-5`: Duplicate include of `webcanvas.h`; `server.h` also includes `webcanvas.h` but does not use it, lengthening rebuilds unnecessarily.
- Low – `layout.cpp:38-180` + `webcanvas.cpp:10-15`: `Layout::layout` stores tokens by value on the class, then WebCanvas copies the whole `Layout` again when setting the display list. This double-copying grows with document size. Passing tokens by `const&` and keeping layout state (or the display list) move-only would avoid extra allocations.
- Low – `webcanvas.cpp:59-63`: The resize handler always re-lays out the current document at `0.95 * width` with implicit double-to-int truncation. Repeated resizes can jitter line breaks and content width, and the heuristic fails when no document is loaded. Consider skipping when empty and using the actual viewport width.
- Low – `DisplayText.h:5-13`: `color` is never initialized and never set before painting; relying on default-constructed `QColor` is fine but makes the field misleading. Either initialize explicitly or remove until colors are supported.
- Low – `utils/utils.cpp:6-16`: `split` pushes an empty element for trailing delimiters; layout skips empty `QString`s so it’s benign, but it does extra work and could emit spurious nodes if reused elsewhere.

## Notes
- Tests are absent; exercising Lexer and URL parsing with a few inputs (`<p>`, `<body>`, `host:port/path`, empty input) would catch the statefulness and parsing regressions quickly.
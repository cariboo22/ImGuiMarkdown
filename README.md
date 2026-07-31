# ImGuiMarkdown
ImGuiMarkdown is a Markdown renderer for [Dear ImGui](https://github.com/ocornut/imgui) using [MD4C](https://github.com/mity/md4c) parser. The goal of this project is to have the most native Markdown renderer in Dear ImGui. Tables are ImGui tables, so it is possible to resize them on the fly. Everything is word wrap, even inside tables. Parsing and rendering are separate so the text file is not parsed every frame which improves performance.

## Requirements

* C++17 or above.
* [MD4C](https://github.com/mity/md4c)

## Supported Markdown Features

ImGuiMarkdown (will) supports the following functionalities of [MD4C](https://github.com/mity/md4c) / [CommonMark Spec](https://spec.commonmark.org/):
  - [x] Quotes
  - [x] Lists (ordered and unordered)
  - [x] Titles
  - [x] Code (not colorized)
  - [x] Italic and bold text
  - [x] Horizontal rule
  - [x] Tables (no alignment support)
  - [ ] Tables (with alignment support)
  - [ ] Images
  - [ ] Links
  - [ ] Underline
  - [ ] Strikethrough
  - [ ] Footnotes
  - [ ] Superscript/Subscript
  - [ ] Highlight
  - [ ] LaTeX math
  - [ ] HTML
  - [ ] Code colorized

## Integration
To integrate ImGuiMarkdown into your project, do the following:
* Include MarkdownRenderer.cpp, MarkdownRenderer.h, Md4cCallbacks.cpp and Md4cCallbacks.h in your project.
* Instantiate as much MarkdownRenderer object as needed.
* Use MarkdownConfig struct to configure the properties of the MarkdownRenderer object. The properties are static, so they are the same for all MarkdownRenderer object.
* Call the MarkdownRenderer's Parse member function whenever the text to parse changes and the MarkdownRenderer's Render member function every frame.

A complete integration example is available in the [example](example/) folder.

## Examples
See [example](example/) folder for a complete example.

See [CommonMark](https://commonmark.org/) if you want to learn more about markdown and its [spec](https://spec.commonmark.org/).

## Credits
This software has been written from scratch by [Eliott Muller](https://github.com/cariboo22). 

It has been inspired by [imgui_markdown](https://github.com/enkisoftware/imgui_markdown) and [imgui_md](https://github.com/learn-more/imgui_md).

For the [MD4C](https://github.com/mity/md4c) library, credit goes to [Martin Mitáš](https://github.com/mity).

## License

This work is licensed under the terms of the MIT license.
For a copy, see <https://opensource.org/licenses/MIT>.

[MD4C](https://github.com/mity/md4c) is also released under the [MIT](https://opensource.org/licenses/MIT) license.

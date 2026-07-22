# Markdown Renderer Test

This document exercises most common Markdown features.

---

# Headings

# H1

## H2

### H3

#### H4

##### H5

###### H6

---

# Paragraphs

This is the first paragraph.

This is the second paragraph.

This paragraph has a
soft line break.

This paragraph has two spaces at the end.
This line should be a hard line break.

---

# Emphasis

*Italic*

_Italic_

**Bold**

__Bold__

***Bold + Italic***

~~Strikethrough~~

---

# Inline Code

Use `printf()` to print text.

Escape a backtick: `` ` ``

---

# Code Blocks

Indented code:

    function hello()
    {
    	return "Hello";
    }

Fenced code:

```text
Hello
World
```

Syntax highlighting:

```python
def hello():
    print("Hello")
```

```javascript
const x = 42;
console.log(x);
```

---

# Blockquotes

> One level.

>> Two levels.

> Multiple
> lines.

> First paragraph.
>
> Second paragraph.

---

# Lists

## Unordered

- Item 1
- Item 2
    - Nested
    - Nested
- Item 3

* Alternative bullet

+ Another bullet

---

## Ordered

1. One
2. Two
3. Three


1) One
    1) Nested
    2) Nested
2) Two

---

## Task Lists (GFM)

- [ ] Todo
- [x] Done
- [ ] Another item

---

# Links

Inline:

[OpenAI](https://openai.com)

Autolink:

<https://example.com>

Email:

<test@example.com>

Reference link:

[Google][google]

[google]: https://google.com

---

# Images

![Alt text](https://via.placeholder.com/150)

Reference image:

![Placeholder][img]

[img]: https://via.placeholder.com/100

---

# Horizontal Rules

---

***

___

---

# Tables (GFM)

| Name | Age | City |
|------|----:|:----:|
| Alice | 25 | NY |
| Bob | 30 | LA |
| Carol | 28 | SF |

---

# Escaping

\*Not italic\*

\# Not a heading

\`Not code\`

\\ Backslash

---

# HTML

<div>

This is inside a div.

</div>

<p>This is an HTML paragraph.</p>

<span style="color:red;">Red text (if HTML/styles allowed)</span>

---

# Mixed HTML + Markdown

<div>

**Bold**

*Italic*

- List item

</div>

---

# Nested Lists

- Item
  - Nested
    - Deep
      - Deeper

1. One
   - Mixed
     1. Nested ordered

---

# Definition List (only some renderers)

Term 1
: Definition 1

Term 2
: Definition 2

---

# Footnotes (GFM)

Footnote reference.[^1]

Another reference.[^long]

[^1]: Small footnote.

[^long]: This is a longer footnote
    that spans multiple lines.

---

# Emoji

:smile:

� � �

---

# Superscript (extension)

2^10^

---

# Subscript (extension)

H~2~O

---

# Highlight (extension)

==Highlighted text==

---

# Math (extension)

Inline:

$E = mc^2$

Block:

$$
\int_0^1 x^2 dx
$$

---

# Spoiler (extension)

>!Spoiler text!<

---

# Raw Characters

< > & " '

---

# Unicode

你好

こんにちは

안녕하세요

� � ❤️

---

# Long Paragraph

Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.

---

# Line Break Test

One
Two

One
Two

---

# HTML Comments

<!-- This comment should not appear -->

Visible text.

---

# Reference Definitions

Here's [one][a] and [another][b].

[a]: https://example.com/a
[b]: https://example.com/b

---

# The End

If everything above renders correctly, your renderer has very good Markdown support.

# Markdown Kitchen Sink {#md-sink}
[TOC]

This document demonstrates just some of the formatting available with markdown syntax. It is helpful to look at this file in a text editor to better understand the syntax.

---
# Headers {#headers}

    # H1 Header {#h1-label}
    ## H2 Header {#h2-label}
    ### H3 Header {#h3-label}
    #### H4 Header {#h4-label}

# H1 Header
## H2 Header
### H3 Header
#### H4 Header

---

# Text {#text}

    *Italic text.*

*Italic text.*

    **Bold text.**

**Bold text.**

    [Hyperlink text](http://www.aseifl.com/).

[Hyperlink text](http://www.aseifl.com/).

Verbatim text.

    Verbatim text (indent 4 spaces):
    ~~~{.cpp}
    string s = "C/C++ code fragment";
    ~~~

    ~~~{.py}
    from time import localtime
    print("Python time is: ", localtime())
    ~~~
    end verbatim

---

# Code Snippets {#code}
## C/C++ {#cpp}

    ~~~{.cpp}
    string s = "C/C++ code fragment";
    ~~~

~~~{.cpp}
string s = "C/C++ code fragment";
~~~

## Python {#python}

    ~~~{.py}
    from time import localtime
    print("Python time is: ", localtime())
    ~~~

~~~{.py}
from time import localtime
print("Python time is: ", localtime())
~~~

---

# Lists {#lists}
Unorderd lists (stars):

    * item 1
    * item 2
      * item 2.1


* item 1
* item 2
  * item 2.1


You can use dashes too:

    - item 1
    - item 2
      - item 2.1


- item 1
- item 2
  - item 2.1


Numbered lists:

    -# item 1
    -# item 2
      -# item 2.1


-# item 1
-# item 2
  -# item 2.1


---

# Images {#images}

    ![Logo](logo_med.png)

![Logo](logo_med.png)

---

# Blockquotes {#blockquotes}

    > "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum." - Lorem Ipsum

> "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum." - Lorem Ipsum

---

# Tables {#tables}

    Col1 Header | Col2 Header
    ----------- | -----------
    Row1, Col1 | Row1, Col2
    Row2, Col1 | Row2, Col2

Col1 Header | Col2 Header
----------- | -----------
Row1, Col1 | Row1, Col2
Row2, Col1 | Row2, Col2

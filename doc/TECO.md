# TECO-64 Reference Manual

## Introduction

TECO is a text editor for editing ASCII files. It can be executed
either interactively, using a simple command-line interface, or with
a display mode that includes the ability to define separate windows
on a terminal screen for both an edit buffer and TECO commands.

TECO is also a programming or scripting language that allows users to
create *macros* for processing text or files, using a terse syntax
designed to minimize keystrokes, that makes it possible to:

- Read, write, and find files.
- Insert and delete characters, strings, and numbers.
- Search for text, optionally using pattern matching constructs
(similar to regular expressions).
- Search for and replace text.
- Control execution with subroutines, looping constructs, and
unconditional (*goto*) and conditional (*if-then-else*) branches.
- Calculate numeric values using expressions that can make use of a
wide variety of arithmetic operators.
- Print messages on the user's terminal containing both strings and
numbers.
- Function as both a floor wax *AND* a dessert topping (well, why not?)

As a consequence, the ability to use and combine these and other
features provides TECO macros the versatility to be used for tasks
ranging from simple to complex. These macros may either be saved
internally in TECO, or stored in files that can be subsequently
executed as stand-alone programs.

However, it is also possible with just a few commands to get real
work done, and a novice TECO user can begin creating and editing
text files after a brief instruction. More powerful features can
be learned at leisure.

This manual presents TECO in two stages. First, it provides basic
information and introduces a simple set of commands. Then subsequent
sections describe the full TECO command set, including a more complete
description of the commands initially presented. These sections also
introduce the concept of TECO as a programming language and explain
how basic editing commands may be combined into programs sophisticated
enough to handle even complicated editing tasks.

- [Getting Started](starting.md)

- [Document Conventions](conventions.md)

- [Command Basics](basics.md)

- [Immediate Action Commands](action.md)

- [Novice Commands](novice.md)

## Command Reference

- [File Commands](file.md)

- [Arithmetic and Expressions](oper.md)

- [Numeric Variables](variables.md)

- [Mode Control Flags](flags.md)

- [Type-out Commands](typeout.md)

- [Page Commands](page.md)

- [Dot Commands](dot.md)

- [Insertion Commands](insert.md)

- [Deletion Commands](delete.md)

- [Search Commands](search.md)

- [Looping Commands](loops.md)

- [If/Then/Else Commands](ifthen.md)

- [GOTO Commands](goto.md)

- [Q-Register Commands](qregister.md)

- [Environment Commands](env.md)

- [Display Mode Commands](display.md)

- [Key Mapping Commands](keymap.md)

- [Miscellaneous Commands](misc.md)

## Appendices

- [Error Messages](errors.md)

- [Glossary of Terms](glossary.md)

- [Index of Commands](index.md)

These provide supplementary information to assist in using certain TECO commands.

- [Colon-Modified Commands](colon.md)

- [At Sign-Modified Commands](atsign.md)

These describe TECO-64 features that may be of interest to veterans of TECO.

- [New Features](new.md)

- [Missing Features](missing.md)

- [Internals Notes](internals.md)

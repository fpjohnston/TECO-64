## TECO-64 Release Checklist

### Overview

This document describes how to prepare a new TECO version for general release.
It is intended for the benefit of developers, and is not necessary for end users.

Note the sections for linting, testing, and building may need to be enhanced for
64-bit builds eventually.

### Create Release Build

- Confirm that all code changes have been checked in, using *git status*.
- Build release with *make release version=(major,minor,patch)*.
- Confirm that *include/version.h* is the only staged or unstaged file, and
check it in with *git add* and *git *commit*.

### Run Smoke Tests

- *make distclean smoke*

### Create Common Builds and Documentation

- *make paging=std teco*
- *make display=off teco*
- *make teco*
- *make doc*

### Install and Post Release

- *sudo make install*
- Add new tag with *git tag -a x.y.z -m "TECO-64 version x.y.z"*.
- Confirm new tag with *git log*.
- *git push*
- Draft a new release on *github*.

## TECO-64 Release Checklist

### Overview

This document describes how to prepare a new TECO version for general release.
It is intended for the benefit of developers, and is not necessary for end users.

Note the sections for linting, testing, and building may need to be enhanced for
64-bit builds eventually.
These sections may also need to be enhanced if it is determined that
various build options are dependent on one another.

### Check In Code

- Update *include/version.h* with new version number.
- Confirm that all code changes have been checked in with *git status*.
- Confirm that all check-ins are correct with *git log*.
- Ensure that header files are current with *make headers*.

### Lint Code (if PC-Lint available)

- *make distclean lint*
- *make distclean display=1 lint*
- *make distclean paging=std lint*

### Run Smoke Tests

- *make distclean display=1 memcheck=1 test=1 teco*
- *./tests/smoke_test.pl tests*

### Build Code and Documentation

- *make distclean teco doc*
- *make distclean display=1 teco doc*
- *make distclean paging=std teco doc*

### Label Release

- Delete any untracked files with *make distclean*.
- Add new tag with *git tag -a -m*.
- Confirm that all check-ins are correct with *git log*.

### Post Release

- Clean up directory tree with *make distclean*.
- Post new version to *github* with *git push*.
- Draft a new release on *github*.

## TECO-64 Release Checklist

### Overview

This document describes how to prepare a new TECO version for general release.
It is intended for the benefit of developers, and is not necessary for end users.

Note the sections for linting, testing, and building may need to be enhanced for
64-bit builds eventually.
These sections may also need to be enhanced if it is determined that
various build options are dependent on one another.

### Check In Code

- Confirm that all code changes have been checked in with *git status*.
- Confirm that all check-ins are correct with *git log*.

### Lint Code (if PC-Lint available)

- *make distclean paging=std lint*
- *make distclean display=off lint*
- *make distclean lint*

### Run Smoke Tests

- *make distclean memcheck=1 test=1 teco*
- *make smoke*

### Build Code and Documentation

- *make distclean paging=std teco doc*
- *make distclean display=off teco doc*
- *make release=(major,minor,patch) teco doc*

### Label Release

- Delete any untracked files with *make distclean*.
- Add new tag with *git tag -a -m*.
- Confirm that all check-ins are correct with *git log*.

### Post Release

- Post new version to *github* with *git push*.
- Draft a new release on *github*.

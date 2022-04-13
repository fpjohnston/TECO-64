## TECO-64 Release Checklist

### Overview

This document describes how to prepare a new TECO version for general release.
It is intended for the benefit of developers, and is not necessary for end users.

Note the sections for linting, testing, and building may need to be enhanced for
64-bit builds eventually.

### Check In Code

- Confirm that all code changes have been checked in with *git status*.
- Confirm that all check-ins are correct with *git log*.

### Verify Common Builds

- *make distclean paging=std  lint doc teco*
- *make distclean display=off lint doc teco*
- *make distclean             lint doc teco*

### Run Smoke Tests

- *make distclean test=1 memcheck=1 teco smoke*

### Build Release

- *make distclean release=(major,minor,patch) teco*
- Delete any untracked files with *make distclean*.
- Add new tag with *git tag -a -m*.
- Confirm that all check-ins are correct with *git log*.

### Post Release

- Post new version to *github* with *git push*.
- Draft a new release on *github*.

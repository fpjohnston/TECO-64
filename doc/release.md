## TECO-64 Release Checklist

### Overview

This document describes how to prepare a new TECO version for general release.
It is intended for the benefit of developers, and is not necessary for end users.

Note the sections for linting, testing, and building may need to be enhanced for
64-bit builds eventually.

### Check In Code

- *make distclean release=(major,minor,patch) teco*
- Confirm that all code changes have been checked in with *git status*.
- Add new tag with *git tag -a x.y.z -m "TECO-64 version x.y.z*" and confirm with *git log*.

### Run Smoke Tests

- *make distclean smoke*

### Verify Common Builds

- *make distclean paging=std display=off teco lint doc*
- *make distclean teco lint doc*

### Post Release

- Post new version to *github* with *git push*.
- Draft a new release on *github*.

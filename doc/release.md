## TECO-64 Release Checklist

### Overview

This document describes the steps for preparing a new version for general release.
It is intended for the benefit of developers, and is not necessary for end users.

### Software Requirements

- *GNU make*
- *gcc*
- *gdb* (optional, for debugging)
- *gprof* (optional, for profiling)
- *git* (optional, for version control)
- *ncurses* (optional, for display mode)
- *perl* (optional, for processing pattern files)
- *perlcritic* (optional, for checking Perl scripts)
- PC-Lint (optional, for linting source files)
- *Doxygen* (optional, for documentation)
- *xalan* (optional, for documentation)

### Create New Version

Confirm that all code changes have been checked in.

`git status`

Execute one of the following.

`make release version=patch`

`make release version=minor`

`make release version=major`

Take note of the version number, specified in the form x.y.z
(for example 200.37.0).
This will be used in the steps below.

### Check In New Version.

`git add include/version.h`

`git commit --message="Updated version number to x.y.z."`

### Test Build Variants

`make paging=std teco`

`make display=off teco`

### Run Smoke Tests

`make smoke`

This will build a debug version and then do the following.

- Confirm usage of all defined error codes.
- Test all command-line options.
- Test all TECO commands.

If *perl* and *perlcritic* are available, it will also validate all Perl scripts
in the *etc/* directory.

If PC-Lint is available, all source code will be linted.

If any of these steps result in an error, the build will be aborted.

### Create Documentation

`make doc`

This step may be skipped if Doxygen is not installed.

### Create Final Build and Install

`make distclean teco`

`sudo make install`

### Add and Verify New Tag

`git tag -a x.y.z -m "TECO-64 version x.y.z"`

`git log`

### Post New Release

`git push`

Draft new release on GitHub.


### EN - Open Wildcard File Specification

EN*foo\**$
- This command presets the "wildcard" lookup file specification
(shown in the example here as *foo\**).
It is only a preset; it does not open, close, or try to find any file.
The "wildcard" lookup is the only file specification command that 
can contain any wildcard notations.

@EN/*foo\**/
- Equivalent to EN*foo\**$.

EN$
- Once the wild card lookup file specification has been preset,
executing this command will find the next file that matches the preset
wild card lookup filespec and will load the filespec buffer with
that file’s name. The G* command can be used to retrieve the fully
expanded file specification. When no more occurences of the wildcard
filespec exist, the ?FNF error is returned.

@EN//
- Equivalent to EN$.

:EN$
- Executes the EN$ command, but also returns non-zero (-1) if another match
of the wildcard file specification could be found, and zero if no more
matches exist.

:@EN//
- Equivalent to :EN$.

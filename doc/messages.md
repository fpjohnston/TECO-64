## TECO-64 - Additional Messages

<style>
    span {
        white-space: nowrap;
    }
</style>

The following lists TECO messages other than command errors.
Most of these can only occur at the time TECO is initialized.
Messages labeled as &apos;Error&apos; will result in an immediate exit from TECO.
Messages labeled as &apos;Warning&apos; indicate that something unexpected
happened, but that the editing session will continue.
Messages labeled as &apos;Info&apos; provide information regarding normal
operation.


| Message(s) | Type | Description |
| ---------- | ---- | ----------- |
| &apos;*pooh*&apos; and &apos;*pooh*&apos; are the same file | Warning | The input and output files had the same name and path. TECO will start the editing session as though only the first file was named. |
| &apos;*pooh*&apos; and &apos;*piglet*&apos; are the same file | Warning | The input and output files had the same device IDs and inode numbers. TECO will start the editing session as though only the first file was named. |
| <span>Argument required for option *--tigger*</span> | Error | The named option is missing a required argument. |
| Can&apos;t find file &apos;*pooh*&apos; | Error | The named file does not exists and either the -c or --nocreate option were used. This message will also occur if both an input and an output file were named, but the input file does not exist. |
| Can&apos;t find file &apos;*pooh*&apos;<br><span>Ignoring TECO&apos;s memory</span></br> | Warning | TECO was unable to find the file previously edited. The editing session will start as though no file was named. |
| <span>Conflicting option: *--tigger*</span> | Error | The --make and --mung options may only be specified once, and are incompatible with one another, as well as the --execute option. |
| Creating new file &apos;*pooh*&apos; | Info | The file *pooh* will be opened for output. Any existing file will be overwritten. |
| Editing file &apos;*pooh*&apos; | Info | The file *pooh* will be opened for input and output. |
| How can I inspect nothing? | Error | The -R or --read-only option was used without a file name. This may mean that there was no file explicitly named on the command-line option, or no previously edited file could be found. |
| How can I make nothing? | Error | The --make option requires a file name. |
| How can I mung nothing? | Error | The --mung option requires a file name. |
| Invalid argument &apos;*owl*&apos; for -A option<br><span>Invalid argument &apos;*owl*&apos; for --arguments option</span></br> | Error | The specified option requires either a single numeric value or a pair of numeric values separated by a comma. |
| Invalid argument &apos;*owl*&apos; for -S option<br><span>Invalid argument &apos;*owl*&apos; for --scroll option</span></br> | Error | The specified option requires a positive numeric value. |
| <span>Invalid option: *--tigger*</span> | Error | The specified option is not valid for TECO-64. |
| Not war? | Info | The file name named with the --make option was &apos;love&apos;. This is just an informational message in keeping with classic TECO, and does not affect the editing session. |
| Reading file &apos;*pooh*&apos; | Info | The file *pooh* will be opened for input only. |
| Reading file &apos;*pooh*&apos;<br><span>Creating new file &apos;*piglet*&apos;</span></br> | Info | The file *pooh* will be opened for input, and the file *piglet* will be opened for output. |
| Reading file &apos;*pooh*&apos;<br><span>Superseding existing file &apos;*piglet*&apos;</span></br> | Info | The file *pooh* will be opened for input, and the file *piglet* will be opened for output, overwriting an existing file of that name. |
| <span>Search failure in iteration</span> | Warning | A search command has failed inside an iteration, causing the iteration is terminated. A semi-colon (;) command immediately following the search command can be used to suppress this message. |
| <span>Superseding existing file &apos;*piglet*&apos;</span> | Info | The file *piglet* will be opened for output, overwriting an existing file of that name. This message can also occur any time an EW command is used. Note that an EK command can be used to undo the effect of the EW command, and recover the original file. |
| Too many files | Error | More than two files were named on the command line, or more than one file was named with either the --make or --mung options. |
| Too many options | Error | TECO has encountered an internal limit on the number of options it can process. |
| <span>Try 'teco --help' for more information</span> | Info | This is printed following one of the errors described in this table. |
| <span>Useless argument for option: *--tigger*</span> | Error | The specified option does not allow an argument. |

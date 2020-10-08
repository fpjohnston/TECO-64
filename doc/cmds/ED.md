### ED - Edit Level Flag

The edit level flag is a bit-encoded word that controls TECO’s
behavior in various respects. Any combination of the individual
bits may be set as the user sees fit. The bits have the following
functions:

| Bit | Function |
| --- | -------- |
| ED&1 | Allow caret (^) in search strings. If this bit is clear, a caret (^) in a search string modifies the immediately following character to become a control character. When this bit is set, a caret in a search string is simply the literal character caret. If you are editing a file that contains many caret characters, you will want to set this bit. (For control of upper/lower case matching in search strings, see the ^X flag.) |
| ED&2 | Allow all Y and _ commands. If this bit is set, the Y and _ commands work unconditionally as described earlier in the manual. If clear, the behavior of the Y and _ commands are modified as follows: If an output file is open and text exists in the text buffer, the Y or _ command will produce an error message and the command will be aborted leaving the text buffer unchanged. Note that if no output file is open the Y and _ commands act normally. Furthermore, if the text buffer is empty the Y command can be used to bring in a page of text whether or not an output file is open (HKY will always work). The _ command will succeed in bringing one page of text into an empty text buffer but will fail to bring in successive pages if an output file is open. |
| ED&4 | Unused in TECO-64. |
| ED&8 | Unused in TECO-64. |
| ED&16 | Allow failing searches to preserve dot. If this bit is set, then whenever a search fails, the original location of the text buffer pointer will be preserved. If this bit is clear, then failing searches (other than bounded searches) leave the text buffer pointer at pointer position 0 after they fail. |
| ED&32 | Enable immediate ESCape-sequence commands. If this bit is set, TECO will recognize an ESCape sequence key pressed immediately after the prompting asterisk as an immediate command. See TBD for a description of immediate ESCape-sequence commands. If this bit is clear (the default case), TECO will treat an ESCape coming in immediately after the asterisk prompt as a <DELIM> That is, TECO will hear a discrete <ESC> character: an ESCape sequence will therefore be treated not as a unified command, but as a sequence of characters. |
| ED&64 | Only move dot by one on multiple occurrence searches. If this bit is clear, TECO treats nStext$ exactly as n\<1Stext\$\>. That is, skip over the whole matched search string when proceeding to the nth search match. For example, if the text buffer contains only A’s, the command 5SAA$ will complete with dot equal to 10. If this bit is set, TECO increments dot by one each search match. In the above example, dot would become 5. |
| ED&128 | Unused in TECO-64. |

The initial value of ED&1 is system dependent. The initial value of the other bits in the ED
flag is 0.

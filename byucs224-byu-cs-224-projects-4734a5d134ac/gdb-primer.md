# Debugging with GDB

The command line does provide a debugger for making sense of a misbehaving program. It is called `gdb`. It is invoked as `gdb program` where `program` is the program to be debugged.
Here is an example: 

```
$ gcc -Wall -g myxxd.c -o myxxd
egm@tajmahal:~/tmp$ gdb myxxd
GNU gdb (Ubuntu 8.1-0ubuntu3.2) 8.1.0.20180409-git
Copyright (C) 2018 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from myxxd...done.
(gdb) 
```

You can think of this debugger as a wrapper program that can run the program you are debugging inside of it, but with complete control over everything.  
You can set breakpoints, execute single instructions, run the program, examine memory/registers, stop the program, etc. 
This can be very useful for debugging and/or figuring out what a program is doing. 

## GDB Commands
Most gdb commands have a longer version and a shorter abbreviation, which can be helpful when a command is used repeatedly. 

### Exiting GDB
* To exit gdb: `quit` or `q`

### Breakpoints
* Setting a breakpoint: `break myxxd.c:37` or `b myxxd.c:37`.  Any line number, label (like a function name (i.e. main)), or memory address can be specified. 
* View current breakpoints: `info breakpoints` 
* Deleting a breakpoint: `delete 1` or `d 1`, where 1 is the number of the breakpoint to be deleted (shown as `Num` in the output of `info breakpoints`)

### Controlling the program
* Running the program: `run` or `r`
* Running the program with input `run < input.txt` or `r < input.txt`
* Execute a single source-code instruction (stepping **into** functions): `step` or `s`
* Execute a single source-code instruction (stepping **over** functions): `next` or `n`
* Execute a single assembly instruction (stepping **into** functions): `stepi` or `si`
* Execute a single assembly instruction (stepping **over** functions): `nexti` or `ni`
* Finish executing a function (stopping after return statement): `finish`
* Continue execution until next breakpoint: `continue` or `c`
* Kill/stop the program: `kill`

### Viewing the processor state
* Viewing the source code: `layout src`
* Viewing the assembly code: `layout asm`
* Viewing the registers: `layout regs`
* Viewing both code and registers `layout split`
* Refresh the layout (when corrupted): `refresh`
* Printing local variables: `print i` or `p i`, where i is the variable to print out
    * Example: displaying contents of register %rsp: `p $rsp`. Displaying that in hex `p/x`
* Print contents of memory: `x/nfu address`, where n is the number of units to print, f is the format character, and u is unit (b/h/w/g for 1/2/4/8 bytes), and address is a memory address. 
Some possible format characters:
    * a - pointer
    * c - character
    * d - signed decimal
    * f - floating point number
    * s - C string
    * t - binary number
    * u - unsigned decimal
    * x - hexadecimal
* Viewing the memory location that register %rax is pointing to: `x $rax`.  Display that in decimal `x/d $rax`

## Additional resources:
Obviously, this is a very condensed list of gdb commands, but it should be enough to help you succeed on the labs for this class. Additional resources can be found: 

  * Help: `help` (gives subtopics for example `help breakpoints`)
  * Googling something like `gdb tutorial` or `gdb cheatsheet` should pull up lots of useful information. 

Here is an example session.

```
$ gdb myxxd
GNU gdb (Ubuntu 8.1-0ubuntu3.2) 8.1.0.20180409-git
Copyright (C) 2018 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from myxxd...done.
(gdb) break myxxd.c:36 
Breakpoint 1 at 0x8c8: file myxxd.c, line 36.
(gdb) run < myxxd.c
Starting program: /users/faculty/egm/tmp/myxxd < myxxd.c

Breakpoint 1, printDataAsChars (data=0x7fffffffe2c0 "#include <stdio.\b\344\377\377\377\177", size=16) at myxxd.c:36
36		while(i < size) {
(gdb) print i
$1 = 0
(gdb) print size
$2 = 16
(gdb) next
37			c = data[i];
(gdb) next
38			if(c < ' ' || c > '~') {
(gdb) next
41			printf("%c", c);
(gdb) print data[i]
$3 = 35 '#'
(gdb) next
42			++i;
(gdb) next
36		while(i < size) {
(gdb) continue
Continuing.
00000000: 2369 6e63 6c75 6465 203c 7374 6469 6f2e  #include <stdio.

Breakpoint 1, printDataAsChars (data=0x7fffffffe2c0 "h>\n#include <std\b\344\377\377\377\177", size=16) at myxxd.c:36
36		while(i < size) {
(gdb) delete 1
(gdb) continue
Continuing.
...lots of output
(gdb) quit
$
```

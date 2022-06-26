# Purpose

This is a command-line utility for solving, generating and testing Nonograms.


## Installation

Installation depends on [Binodeps](https://github.com/simpsonst/binodeps) and [Nonogram library](https://github.com/simpsonst/nonolib).

To install in `/usr/local`:

```
make
sudo make install
```

Override the default location by setting `PREFIX`.
This can be set on the command line:

```
make PREFIX=$HOME/.local install
```

&hellip;or in a local file `config.mk`:

```
PREFIX=$(HOME)/.local
```

&hellip;or in `nonogram-env.mk`, which is sought on GNU Make's search path, set by `-I`:

```
make -I$HOME/.config/local install
```

You probably want to set some more optimal compilation options, and find the Nonogram library, e.g., by placing them in `config.mk`:

```
CFLAGS += -O2 -g
PREFIX=$(HOME)/.local
CPPFLAGS += -I$(PREFIX)/include
LDFLAGS += -L$(PREFIX)/lib
```

Installation places installs the following files under `$(PREFIX)`:

```
bin/nonogram
```

## Formats

Puzzles the [format defined by the Nonogram Library](https://github.com/simpsonst/nonolib#puzzle-format).
Solutions are simple text files consisting of lines of `#` for foreground colour and `-` for background colour.

## Use

The `nonogram` command processes switches and filenames in order.
Some switches modify a context:

- `-is` &ndash; Read from standard input (the default).
- `-i file` &ndash; Read from `file`.
- `-os` &ndash; Write to standard output (the default).
- `-o file` &ndash; Write to `file`.  Multiple solutions are separated by blank lines.
- `-on file-%d.txt` &ndash; Write each solution to `file-1.txt`, `file-2.txt`, etc.
- `+o` &ndash; Disable solution/puzzle output.
- `-v` &ndash; Clear screen and display the grid as the solution is solved.  The standard error output is used.
- `+v` &ndash; Don't display the grid during solving.
- `-Afast` &ndash; Use the fast algorithm.
- `-Acomplete` &ndash; Use the slow exhaustive algorithm.
- `-Ahybrid` &ndash; Use the fast algorithm first, then the slow exhaustive one.
- `-Afcomp` &ndash; Use the fast comprehensive algorithm.
- `-Affcomp` &ndash; Use the fast algorithm, then the fast comprehensive algorithm.
- `-Aolsak` &ndash; Use the Olšáks' algorithm.
- `-Afastolsak` &ndash; Use the fast algorithm, then the Olšáks'.
- `-Afastolsakcomplete` &ndash; Use the fast algorithm, then the Olšáks', then the slow exhaustive.
- `-Afastodd` &ndash; Probably the same as `-Afastolsak`, but `odd` was intended to diverge. 
- `-Afastoddcomplete` &ndash; Probably the same as `-Afastolsakcomplete`.
- `-CO`, `-CE` &ndash; Display a count of solutions when complete, on standard output or standard error output respectively.
- `+CO`, `+CE` &ndash; Disable count of solutions, on standard output or standard error output respectively.
- `-s num` &ndash; Stop after `n` solutions.  Use `-n 2` to detect bad puzzles that have more than one solutions.
- `-log file.log` &ndash; Append logging information to `file.log`.
- `-nlog file.log` &ndash; Overwrite `file.log` with logging information.
- `+log`, `+nlog` &ndash; Cancel logging (default).
- `-ll num` &ndash; Set log detail to `num` (default 0).
- `-Dkey` &ndash; Delete the metadata specified by `key`.
- `-Rkey value` &ndash; Set or replace the metadata specified by `key`.
- `-Xkey` &ndash; Print the metadata specified by `key`.
- `-W` &ndash; Print the width.
- `-H` &ndash; Print the height.

Other switches act on the current context before further argument processing:

- `-x` &ndash; Load a puzzle from the source, and solve it, writing solutions to the destination.  This switch is assumed at the end if the context has changed since the last action switch.
- `-p` &ndash; Print the current context.
- `-c` &ndash; Load a puzzle, and display the sum of row clues minus the sum of column clues.
- `-g` &ndash; Load a grid from the source, stopping at end-of-file, or on first line with a different length, and write out a puzzle.

## Examples

```
nonogram -on flower%d.txt -v -i flower.non -x
```

Solve `flower.non`, printing results in `flower1.txt`, `flower2.txt`, &hellip;, and display work.


```
nonogram -x < duck.non
```

Solve `duck.non`, print to `stdout`.


```
nonogram +o -v -x < duck.non
```

Show `duck.non` being solved.
Don't produce solution.


```
cat duck.non flower.non | nonogram +o -v -x -x
```

Solve two puzzles from `stdin`.
Display working but give no other output.

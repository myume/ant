# ant

code annotation tool (i wanted cat as the acronym but it wouldn't work for
[obvious reasons](https://en.wikipedia.org/wiki/Cat_(Unix)))

<img width="2832" height="1800" alt="image" src="https://github.com/user-attachments/assets/68ff55b1-e2a0-495f-b97c-8103bde5e4fd" />

## Usage

There is a cli tool, library, and vscode plugin.

The vscode plugin requires the cli tool to be installed on your machine.

```console
Usage: ant [command] [options]

Commands:
  init             - initialize the annotations directory
  list             - list annotations for file
  add              - add an annotation
  remove           - remove an annotation

Options:
  --help           - show this menu
  --version        - the ant version
  --json           - output json
  -s               - the source code directory
  -o               - path to output/annotations directory
```

## Benchmarks

These are the benchmarks on my machine. The performance is rather asymmetric,
with writes being far more performant than reads. This is because the
implementation is pretty much using an append only log to store the annotations.

It seems that it is performant enough for the general case. 7-8ms per read seems
to be a reasonable time to read 10000 annotations, especially if you're using
the tool manually. Annotations are bound above by the lines of code in a file,
usually this will be under 10000.

```console
$ build/bench/ant-benchmark 10000 
Successfully initialized ant in .ant_bench/.ant
Running benchmarks...
312500 writes/s 0.0032 ms/write
127.763 reads/s 7.827 ms/read
```

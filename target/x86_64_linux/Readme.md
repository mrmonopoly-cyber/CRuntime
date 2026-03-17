# Target X86_64_LINUX

> [!IMPORTANT]
> This section is in work in progress, and it's mostly used for testing

This is the section related to the deployment for x86_64_linux. The best way to understand it 
is to look at the sources. They are tiny and not too hard with the only exception of
the context switch witch does use a bit of inline assembly.

# Build

To build a deployment release for this target you just need to execute the following commands:

```sh

gcc nob.c -o nob
./nob


```

Output in `out` dir:

```sh

# tree -L 1 out
out
├── CR.a
├── CR.o
├── CR.so
├── Include
└── lib

3 directories, 3 files


```

# Examples

There are a few examples that you can use to better understand the library. 
To check them look into the `Examples` directory.
For all of them the following process is true.

To build the example you just need to compile the build script once with the following
command:

```sh

gcc nob.c -o nob


```

**nob** has an integrated, minimal, CLI which can be seen with the following command:


```sh

./nob -h


```

To build the project just run **nob**:

```sh

./nob


```

To run the executable just run **nob**:

```sh

./main


```

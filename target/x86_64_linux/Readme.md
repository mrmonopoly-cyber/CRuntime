# Target X86_64_LINUX

> [!IMPORTANT]
> This section is in work in progress, and it's mostly used for testing

This is the section related to the deployment for x86_64_linux. The best way to understand it 
is to look at the sources. They are tiny and not too hard with the only exception of
the context switch witch does use a bit of inline assembly.

# Build

To build the deployment you just need to compile the build script once with the following
command:

```sh

gcc nob.c -o nob


```

now **nob** has an integrated, minimal, CLI which can be seen with the following command:


```sh

./nob -h


```

To build the project just run **nob**:
```sh

./nob


```

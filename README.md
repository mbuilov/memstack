# memstack
memory stack allocation library

In C, it's often hard to track memory allocations and avoid memory leaks.
To simplify this task, it may be useful to allocate memory via special object and, when appropriate, free all allocations by one call.

`struct memstack` - an example of such special object, available via [memstack/memstack.h](/memstack/memstack.h)

## Contents

- [Api overview](#api-overview)
- [Debug api](#debug-api)
- [Installing](#installing)

### Api overview

1. [memstack_init](#initialize-memstack-structure)
2. [memstack_destroy](#destroy-memstack-structure)
3. [memstack_push](#get-memory-from-memstack)
4. [memstack_pop](#give-memory-back-to-memstack)
5. [memstack_repush_last](#reallocate-last-allocation)
6. [memstack_cleanup](#pop-all-memory-allocations)
7. [memstack_get_bottom](#get-memstack-bottom-position)
8. [memstack_reset](#reset-memstack-to-saved-state)
9. [memstack_get_last_mem](#get-pointer-to-last-pushed-memory)

### Debug api

1. [memstack_check](#check-red-zones-of-memstack-allocations)
2. [memstack_print](#print-current-memstack-allocations)
3. [memstack_enable_log](#enable-logging-of-memstack-allocations)

---------------------------------------------------

#### Initialize memstack structure
```C
void memstack_init(struct memstack *st);
```
Parameters:
- `st`  - memstack structure to initialize

*Example:*
```C
struct memstack st;
memstack_init(&st);
```
It's also possible to initialize memstack statically:
```C
struct memstack st = MEMSTACK_STATIC_INITIALIZER;
```

#### Destroy memstack structure
```C
void memstack_destroy(struct memstack *st);
```
Parameters:
- `st`  - memstack structure to destroy

All memory allocated by memstack is deallocated

#### Get memory from memstack
```C
memstack_memory_t *memstack_push(struct memstack *st, size_t size);
```
Parameters:
- `st`   - memstack structure
- `size` - number of bytes to allocate, must be non-zero

**Returns:** pointer to abstract structure `memstack_memory_t` or `NULL` if allocation failed

Returned pointer will be suitably aligned for any structure, so may be casted to any poiner type

_Note_: `memstack_push()` may not call system `malloc()` if there is enough free space in last allocated internal memstack memory block

*Example:*
```C
extern struct memstack *st;
struct my_struct *m = (struct my_struct*)memstack_push(st, sizeof(*m));
if (!m)
    fail;
```

#### Give memory back to memstack
```C
void memstack_pop(struct memstack *st, memstack_memory_t *mem);
```
Parameters:
- `st`  - memstack structure
- `mem` - pointer to abstract structure `memstack_memory_t` - the same pointer that was returned by one of previous `memstack_push()` calls

It is possible to pop multiple sequential allocations by one call - just pop the first allocation of the sequence

_Note_: `memstack_pop()` may not call system `free()` for the last popped internal memstack memory block

*Example:*
```C
extern struct memstack *st;
extern struct my_struct *m;
memstack_pop(st, (memstack_memory_t*)m);
```

#### Reallocate last allocation
```C
memstack_memory_t *memstack_repush_last(struct memstack *st, memstack_memory_t *mem, size_t new_size);
```
Parameters:
- `st`  - memstack structure
- `mem` - pointer to abstract structure `memstack_memory_t` that was returned by last `memstack_push()` call or `NULL`
- `new_size` - new allocation size, in bytes, must be non-zero

If `mem` is `NULL`, then acts like `memstack_push()`

**Returns:** pointer to abstract structure `memstack_memory_t` or `NULL` if failed to expand existing allocation or create new allocation

Returned pointer will be suitably aligned for any structure, so may be casted to any poiner type

*Example:*
```C
extern struct memstack *st;
extern void *m;
m = memstack_repush_last(st, (memstack_memory_t*)m, 100);
if (!m)
    fail;
```

#### Pop all memory allocations
```C
void memstack_cleanup(struct memstack *st);
```
Parameters:
- `st`  - memstack structure

_Note_: memstack remembers maximum total size of allocations and will try to allocate one big memory block on first `memstack_push()` call

#### Get memstack bottom position
```C
memstack_bottom_t *memstack_get_bottom(struct memstack *st);
```
Parameters:
- `st`  - memstack structure

**Returns:** pointer to abstract structure `memstack_bottom_t` for passing it to `memstack_reset()`

Returned pointer must not be checked, it may have any value, even `NULL`

This function is used to remember memstack state before doing next allocations and then to reset memstack to saved state after these allocations

_Note_: saved state is associated with last memstack allocation that may be made before `memstack_get_bottom()` call and is invalidated by `memstack_pop()/memstack_repush_last()` called for that allocation

#### Reset memstack to saved state
```C
void memstack_reset(struct memstack *st, memstack_bottom_t *pos);
```
Parameters:
- `st`  - memstack structure
- `pos` - memstack state previously obtained via `memstack_get_bottom()` or `NULL`

All memory allocated in memstack after `pos` was taken is popped

If `pos` is `NULL`, then acts like `memstack_cleanup()`

*Example:*
```C
extern struct memstack *st;
memstack_bottom_t *pos = memstack_get_bottom(st);
memstack_memory_t *m1 = memstack_push(st, 100);
memstack_memory_t *m2 = memstack_push(st, 200);
...
memstack_reset(st, pos);
```

#### Get pointer to last pushed memory
```C
memstack_memory_t *memstack_get_last_mem(struct memstack *st, size_t size);
```
Parameters:
- `st`   - memstack structure
- `size` - last allocation size, in bytes, must be non-zero

**Returns:** pointer to abstract structure `memstack_memory_t` that was returned by last `memstack_push()/memstack_repush_last()` call

_Note_: `size` must be exactly the same one that was passed to last `memstack_push()/memstack_repush_last()` call

*Example:*
```C
extern struct memstack *st;
memstack_memory_t *m1 = memstack_push(st, 100);
...
memstack_memory_t *m2 = memstack_get_last_mem(st, 100);
assert(m1 == m2);
```

=======================================================

#### Check red zones of memstack allocations
```C
void memstack_check(struct memstack *st);
```
Parameters:
- `st`  - memstack structure

_Note_: red zone size - build-time configurable via `DMEMSTACK_TEST_BYTES_COUNT` macro

#### Print current memstack allocations
```C
void memstack_print(struct memstack *st);
```
Parameters:
- `st`  - memstack structure

_Note_: allocations are written to `stderr`

#### Enable logging of memstack allocations
```C
void memstack_enable_log(struct memstack *st, int enable);
```
Parameters:
- `st`     - memstack structure
- `enable` - non-zero to enable logging, `0` - to disable

_Note_: allocations are written to `stderr`

---------------------------------------------------

### Installing

1. Get clean-build build system:

    [`git clone https://github.com/mbuilov/clean-build`](https://github.com/mbuilov/clean-build)

2. For windows, get [Gnu Make](https://www.gnu.org/software/make) executable:

    [`git clone https://github.com/mbuilov/gnumake-windows`](https://github.com/mbuilov/gnumake-windows)

3. Build library

    3.1 On Linux (_example_):
    ```sh
    $ make MTOP=/home/user/clean-build OS=LINUX CPU=x86_64 TARGET=MEMSTACK
    ```

    3.2 On Windows (_example_):
    ```cmd
    C:\tools\gnumake-4.2.1.exe MTOP=C:\tools\clean-build OS=WINXX CPU=x86_64 TARGET=MEMSTACK OSVARIANT=WIN7 VS="C:\Program Files (x86)\Microsoft Visual Studio 14.0" WDK="C:\Program Files (x86)\Windows Kits\10" WDK_TARGET="10.0.14393.0"
    ```

    _**Tips**_:
    - define `NO_STATIC=1` to not build static library archive
    - define `NO_SHARED=1` to not build shared library (dll)
    - to view other possible values of `OS`, `CPU` or `TARGET` variables, do not define them
    - define variable `V=1` for verbose build, to print executed commands

    If make target is not specified, default target _`all`_ (compile the library) will be built

    By default, all variants of library are built:
    * for static library  - `MEMSTACK_LIB_VARIANTS="R P D S"`
    * for dynamic library - `MEMSTACK_DLL_VARIANTS="R S"`

    _**Notes**_:
    - if some variant is unsupported under target platform, variant is filtered-out from list of variants
    - if variants list is empty, default variant `R` is built

    Variants of static library for **LINUX**:
    * _`R`_ - default, position-dependent code for linking executables
    * _`P`_ - position-independent code for linking executables (`-fpie` compiler option)
    * _`D`_ - position-independent code for linking shared objects (`-fpic` compiler option)

    Variants of static library for **WINDOWS**:
    * _`R`_ - default, dynamically linked multi-threaded C runtime library (`/MD` compiler option)
    * _`S`_ - statically linked multi-threaded C runtime library (`/MT` compiler option)

    Variants of dynamic library for **LINUX**:
    * _`R`_ - default, position-independent code (`-fpic` compiler option)

    Variants of dynamic library for **WINDOWS**:
    * _`R`_ - default, dynamically linked multi-threaded C runtime library (`/MD` compiler option)
    * _`S`_ - statically linked multi-threaded C runtime library (`/MT` compiler option)

    _**Tip**_: there are predefined targets:
    * _`tests`_     - to build library and tests
    * _`check`_     - to build library and tests, then run tests
    * _`clean`_     - to delete built artifacts, except created directories
    * _`distclean`_ - to delete all artifacts, including created directories

4. Install library and interface headers

    _Note_: make command should be the same as for building, except the target should be _`install`_ or _`uninstall`_

    4.1 On Linux (_example_):

    possibly as root, do
    ```sh
    $ make MTOP=/home/user/clean-build OS=LINUX CPU=x86_64 TARGET=MEMSTACK install
    ```

    4.2 On Windows (_example_):
    ```cmd
    C:\tools\gnumake-4.2.1.exe MTOP=C:\tools\clean-build OS=WINXX CPU=x86_64 TARGET=MEMSTACK OSVARIANT=WIN7 VS="C:\Program Files (x86)\Microsoft Visual Studio 14.0" WDK="C:\Program Files (x86)\Windows Kits\10" WDK_TARGET="10.0.14393.0" PREFIX=C:\dst install
    ```

    _Note_: Headers are installed in `$(PREFIX)/include`, libraries - in `$(LIBDIR)`

    _**Tips**_:
    - define variable `PREFIX` to override default install location - `/usr/local` (for UNIX) or `dist` (for WINDOWS)
    - define variable `LIBDIR` to override default libraries install location - `$(PREFIX)/lib`
    - define variable `DESTDIR` to add prefix to `$(PREFIX)` - to make path to temporary install location
    - define `NO_INSTALL_HEADERS=1` to not install development library interface header files
    - define `NO_INSTALL_LA=1` to not install development libtool library files (for UNIX)
    - define `NO_INSTALL_PC=1` to not install development pkg-config  library files (for UNIX)
    - define `NO_INSTALL_IMPS=1` to not install development dll import libraries (for WINDOWS)
    - define `NO_DEV=1` to not install all above development files (headers, .la, .pc, import libraries)

    _**Tip**_: there is one more predefined target:
    * _`uninstall`_ - to delete installed files. Note: some installed directories may not be deleted.

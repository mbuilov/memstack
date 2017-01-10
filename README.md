# memstack
simple memory stack allocation library

In C, it's often hard to track memory allocations and avoid memory leaks.
To simplify this task, it may be useful to allocate memory via special object and, when appropriate, free all allocations by one call.

struct memstack - an example of such special object.


Api.


1) void memstack_init(struct memstack *st);

Init memstack structure.

Parameters:
st  - memstack structure to initialize

Example:

struct memstack st;
memstack_init(&st);

It's also possible to initialize memstack statically:

struct memstack st = MEMSTACK_STATIC_INITIALIZER;


2) void memstack_destroy(struct memstack *st);

Destroy memstack structure, free all allocated memory.

Parameters:
st  - memstack structure to destroy


3) struct memstack_memory *memstack_push(struct memstack *st, size_t size);

Get memory from memstack.

Parameters:
st   - memstack structure
size - number of bytes to allocate, must be non-zero

It's nothig bad will happen if size is zero, except assert in DEBUG, but don't pass zero size value.

Returns: pointer to abstract structure memstack_memory or NULL if allocation failed.

Returned pointer will be suitably aligned for any structure, so may be casted to any poiner type.

Note: memstack_push() may not call system malloc() if there is enough free space in last allocated internal memstack memory block.

Example:

extern struct memstack *st;
struct my_struct *m = (struct my_struct*)memstack_push(st, sizeof(*m));
if (!m)
	fail;


4) void memstack_pop(struct memstack *st, struct memstack_memory *mem);

Give memory back to memstack.

Parameters:
st  - memstack structure
mem - pointer to abstract structure memstack_memory - the same pointer that was returned by one of previous memstack_push() calls

It is possible to pop multiple sequential allocations by one call - just pop the first allocation of the sequence.

Note: memstack_pop() may not call system free() for the last popped internal memstack memory block.

Example:

extern struct memstack *st;
extern struct my_struct *m;
memstack_pop(st, (struct memstack_memory*)m);


5) struct memstack_memory *memstack_repush_last(struct memstack *st, struct memstack_memory *mem, size_t new_size);

Reallocate last allocation.

Parameters:
st  - memstack structure
mem - pointer to abstract structure memstack_memory that was returned by last memstack_push() call
new_size - new allocation size, in bytes.

If mem is NULL, then acts like memstack_push().
It's nothig bad will happen if new_size is zero, except assert in DEBUG, but don't pass zero new_size value.

Returns: pointer to abstract structure memstack_memory or NULL if failed to expand allocation.

Returned pointer will be suitably aligned for any structure, so may be casted to any poiner type.

Example:

extern struct memstack *st;
extern void *m;
m = memstack_repush_last(st, (struct memstack_memory*)m, 100);
if (!m)
	fail;


6) void memstack_cleanup(struct memstack *st);

Pop all memory allocations.

Parameters:
st  - memstack structure

memstack remembers maximum total size of allocations and will try to allocate one big memory block on first memstack_push() call. 


7) struct memstack_bottom *memstack_get_bottom(struct memstack *st);

Get memstack bottom position.

Parameters:
st  - memstack structure

Returns: pointer to abstract structure memstack_bottom to pass it to memstack_reset().

Returned pointer must not be checked, it may have any value, even NULL.

This function is used to remember memstack state before doing next allocations and then to reset memstack to saved state after these allocations.

Note: saved state is associated with last memstack allocation that may be made before memstack_get_bottom() call
  and is invalidated by memstack_pop()/memstack_repush_last() called for that allocation.


8) void memstack_reset(struct memstack *st, struct memstack_bottom *pos);

Reset memstack to saved state.

Parameters:
st  - memstack structure
pos - memstack state obtained via memstack_get_bottom()

All memory allocated after pos was taken is popped.

Example:

extern struct memstack *st;
struct memstack_bottom *pos = memstack_get_bottom(st);
struct memstack_memory *m1 = memstack_push(st, 100);
struct memstack_memory *m2 = memstack_push(st, 200);
....
memstack_reset(st, pos);


9) struct memstack_memory *memstack_get_last_mem(struct memstack *st, size_t size);

Get pointer to last pushed memory.

Parameters:
st   - memstack structure
size - last allocation size, in bytes

Note: size must be exactly the same one that was passed to memstack_push()/memstack_repush_last().

Example:

extern struct memstack *st;
struct memstack_memory *m1 = memstack_push(st, 100);
....
struct memstack_memory *m2 = memstack_get_last_mem(st, 100);
assert(m1 == m2);

# memstack
memory stack allocation library

In C, it's often hard to track memory allocations and avoid memory leaks.
To simplify this task, it may be useful to allocate memory via special object and, when appropriate, free all allocations by one call.

struct memstack - an example of such special object.


## Api overview

1. [memstack_init](#init-memstack-structure)
2. [memstack_destroy](#destroy-memstack-structure)
3. [memstack_push](#get-memory-from-memstack)
4. [memstack_pop](#give-memory-back-to-memstack)
5. [memstack_repush_last](#reallocate-last-allocation)
6. [memstack_cleanup](#pop-all-memory-allocations)
7. [memstack_get_bottom](#get-memstack-bottom-position)
8. [memstack_reset](#reset-memstack-to-saved-state)
9. [memstack_get_last_mem](#get-pointer-to-last-pushed-memory)


## Debug api

1. [memstack_check](#check-red-zones-of-memstack-allocations)
2. [memstack_print](#print-current-memstack-allocations)
3. [memstack_enable_log](#enable-logging-of-memstack-allocations)


#### Init memstack structure
```
void memstack_init(struct memstack *st);
```
Parameters:
- ```st```  - memstack structure to initialize

<i>Example:</i>
```
struct memstack st;
memstack_init(&st);
```
It's also possible to initialize memstack statically:
```
struct memstack st = MEMSTACK_STATIC_INITIALIZER;
```

#### Destroy memstack structure
```
void memstack_destroy(struct memstack *st);
```
Parameters:
- ```st```  - memstack structure to destroy

All memory allocated by memstack is deallocated.

#### Get memory from memstack
```
memstack_memory_t *memstack_push(struct memstack *st, size_t size);
```
Parameters:
- ```st```   - memstack structure
- ```size``` - number of bytes to allocate, must be non-zero

<b>Returns:</b> pointer to abstract structure ```memstack_memory_t``` or ```NULL``` if allocation failed.

Returned pointer will be suitably aligned for any structure, so may be casted to any poiner type.

Note: ```memstack_push()``` may not call system ```malloc()``` if there is enough free space in last allocated internal memstack memory block.

<i>Example:</i>
```
extern struct memstack *st;
struct my_struct *m = (struct my_struct*)memstack_push(st, sizeof(*m));
if (!m)
    fail;
```

#### Give memory back to memstack
```
void memstack_pop(struct memstack *st, memstack_memory_t *mem);
```
Parameters:
- ```st```  - memstack structure
- ```mem``` - pointer to abstract structure ```memstack_memory_t``` - the same pointer that was returned by one of previous ```memstack_push()``` calls

It is possible to pop multiple sequential allocations by one call - just pop the first allocation of the sequence.

Note: ```memstack_pop()``` may not call system ```free()``` for the last popped internal memstack memory block.

<i>Example:</i>
```
extern struct memstack *st;
extern struct my_struct *m;
memstack_pop(st, (memstack_memory_t*)m);
```

#### Reallocate last allocation
```
memstack_memory_t *memstack_repush_last(struct memstack *st, memstack_memory_t *mem, size_t new_size);
```
Parameters:
- ```st```  - memstack structure
- ```mem``` - pointer to abstract structure ```memstack_memory_t``` that was returned by last ```memstack_push()``` call or ```NULL```
- ```new_size``` - new allocation size, in bytes, must be non-zero.

If ```mem``` is ```NULL```, then acts like ```memstack_push()```.

<b>Returns:</b> pointer to abstract structure ```memstack_memory_t``` or ```NULL``` if failed to expand existing allocation or create new allocation.

Returned pointer will be suitably aligned for any structure, so may be casted to any poiner type.

<i>Example:</i>
```
extern struct memstack *st;
extern void *m;
m = memstack_repush_last(st, (memstack_memory_t*)m, 100);
if (!m)
    fail;
```

#### Pop all memory allocations
```
void memstack_cleanup(struct memstack *st);
```
Parameters:
- ```st```  - memstack structure

Note: memstack remembers maximum total size of allocations and will try to allocate one big memory block on first ```memstack_push()``` call.

#### Get memstack bottom position
```
memstack_bottom_t *memstack_get_bottom(struct memstack *st);
```
Parameters:
- ```st```  - memstack structure

<b>Returns:</b> pointer to abstract structure ```memstack_bottom_t``` for passing it to ```memstack_reset()```.

Returned pointer must not be checked, it may have any value, even ```NULL```.

This function is used to remember memstack state before doing next allocations and then to reset memstack to saved state after these allocations.

Note: saved state is associated with last memstack allocation that may be made before ```memstack_get_bottom()``` call and is invalidated by ```memstack_pop()/memstack_repush_last()``` called for that allocation.

#### Reset memstack to saved state
```
void memstack_reset(struct memstack *st, memstack_bottom_t *pos);
```
Parameters:
- ```st```  - memstack structure
- ```pos``` - memstack state previously obtained via ```memstack_get_bottom()``` or ```NULL```

All memory allocated in memstack after ```pos``` was taken is popped.

If ```pos``` is ```NULL```, then acts like ```memstack_cleanup()```.

<i>Example:</i>
```
extern struct memstack *st;
memstack_bottom_t *pos = memstack_get_bottom(st);
memstack_memory_t *m1 = memstack_push(st, 100);
memstack_memory_t *m2 = memstack_push(st, 200);
....
memstack_reset(st, pos);
```

#### Get pointer to last pushed memory
```
memstack_memory_t *memstack_get_last_mem(struct memstack *st, size_t size);
```
Parameters:
- ```st```   - memstack structure
- ```size``` - last allocation size, in bytes, must be non-zero

<b>Returns:</b> pointer to abstract structure ```memstack_memory_t``` that was returned by last ```memstack_push()/memstack_repush_last()``` call.

Note: ```size``` must be exactly the same one that was passed to last ```memstack_push()/memstack_repush_last()``` call.

<i>Example:</i>
```
extern struct memstack *st;
memstack_memory_t *m1 = memstack_push(st, 100);
....
memstack_memory_t *m2 = memstack_get_last_mem(st, 100);
assert(m1 == m2);
```

#### Check red zones of memstack allocations
```
void memstack_check(struct memstack *st);
```
Parameters:
- ```st```  - memstack structure

Note: red zone size - build-time configurable via ```DMEMSTACK_TEST_BYTES_COUNT``` macro

#### Print current memstack allocations
```
void memstack_print(struct memstack *st);
```
Parameters:
- ```st```  - memstack structure

Note: allocations are written to ```stderr```

#### Enable logging of memstack allocations
```
void memstack_enable_log(struct memstack *st, int enable);
```
Parameters:
- ```st```     - memstack structure
- ```enable``` - non-zero to enable logging, 0 - to disable

Note: allocations are written to ```stderr```

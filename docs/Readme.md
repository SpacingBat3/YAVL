YAVL: Yet Another Vector Library
================================

This is a a (yet another?) library that implements data structure
for dynamically sized arrays in C, similar to C++ `vector<T>` or
Rust's `Vec<T>` types.

And yeah, project name is subject to change :/.

Features
--------

### 1. Type-independent data structure

Library does not work on data, but rather on the memory – this means
you might be able to pack any type into it, and in some cases also
mix types as well, as long as value types match appropriate offsets
for values.
   
### 2. Stack-like data management

Library offers convenient operations like `push` or `pop` – with
additional resizing of the array if it becomes a neccesity.

### 3. CMake build infrastructure integration

The project is designed to be easily integrated into another projects
– it offers defining a personal symbol namespace (prefix) to avoid type
collision with another library (or to have more fitting namespace for your
use case scenario) and also properly exposes CMake metadata to use and depend
on it as on any other library you can in CMake.

### 4. Common Package Spec support

Outside of CMake, a deployed builds offer a [Common Package Specification][cps]
metadata for machines with relatively new CMake version installed, so you can 
integrate it with any tooling to manage your dependencies: CPS offers a more 
feature-rich package description than `pkg-config` while not requiring any tooling 
to be `cmake` at some part or another. CPS is still relatively fresh, so adoption 
across tooling might be limited.

### 5. Pointer validation mechanisms & safety policy

Library tries to well-define how to handle your data at the memory level – most
functions have their expected pointer handling documented and are well-defined
for correctness. There's also an additional method `chkptr` to peek if external
pointer that points into memory within `vec_t` is within-bounds of given vector,
which is useful as an additional anti-measure against out-of-bounds memory access.

In the future, pointer API might be a separate thing, and most operations will be
slower copy-alike data return.

> [!NOTE]
> There's still no guarantees for every use case to be memory-safe: currently
> library is implemented in way you need to allocate memory properly by yourself,
> this means you need to initially assign it to `VEC_T_ALLOCATOR` macro's value
> (or, currently, zero-fill the memory space).

### 6. Implicit memory immutability in types and arguments

Library well defines what it will done with the input arguments and whether they
will be modified or not.

### 7. In-code Doxygen compatible documentation

The library API is fully documented via Doxygen, meaning it is presentable well
on various C/C++ LSP providers that follow Doxygen standard and supports
detalied auto-generated API documentation generation to HTML via Doxygen,
as part of CMake build/install pipeline.

### 8. (WIP) Error handling

Currently, error handling is implemented in similar manner as in C standard
library – however in the future, I want to introduce similar error handling
to Rust's `Result<...>` containers in order to allow for more safe result
handling of data – so accessing invalid states will result in crashes, like
`unwrap` in Rust does, with possibility to handle the wrong state in different
manner. This will require a separate library that defines proper API and methods
and runtime, and will provide either macro with generic type, or struct
container with type-independent `void *` dynamic pointer type.

### 9. (TODO) Rust integration

I consider toying with the library further to offer Rust bindings for it and
to try implement a wrapper that will make it fit the Rust ecosystem.

### 10. (TODO) Dynamic buffers for `FILE` streams.

I plan to introduce a platform-agnostic API for implementing various parsers
in more sensible way than what C offers by-default – is should offer very
user-friendly `readline`-kind of parsing files and anything that could
benefit from dynamically-sized buffers. For security applications, I might
need to introduce "hard limits" if neccesary but that most likely will be
an optional API arg outside of base `vec` API.

API
---

Currently, API is properly documentated via doc comments in header files.
This documentation uses `doxygen` and offers offline HTML docs that
describe the library in-detail.

An online-published variant of the documentation is being worked on
and will be available on GitHub pages.

As various library builds might use different prefixes for symbols, be aware
API will be documented in distribution-specific way (i.e. default prefix
for symbols will be used, following the CI/CD pipeline).

History
-------

I've implemented this library for a quite a long time now, as I wanted
to have some lightweight solution to include dynamically-sized arrays
into my code, to ease development of multiple academic or personal
software projects. It quickly occured to me I use it quite frequently
and sometimes place it in the projects, just to have an access to dynamic
arrays or just because I need to implement some kind of parser.

As it is one of the most battle-tested projects, I've decided to make
it public as one of my experimental projects – it is still very much
incomplete and needs to be polished, albeit being rather stable solution
that just does what it should do. I still see in it more potential, to
implement an API specifically to ease development of parsing various file
formats (be it CSV or obscure academic input data file).

License
-------

See [COPYING](../COPYING) for licensing information about this project.

Take note that another releases of this project, including academic ones,
might be a subject to exclusive licensing terms and conditions, as this
project might be used in various other software I am involved with, and
I might be in need to grant various entities additional priviledges
(regardless if they violate *the GPL3 ideology* or not).

> [!NOTE]
> I might also switch from `GPL3-or-later` to something more permissive if I'll
> find value in that. `GPL3` is more-or-less a license for the particular development
> and public builds of this library. Contributors are also assumed to give their
> ownership rights to the code to sole author of this library or use more permissive
> licensing on request, else their contribution might not get acknowledged at all
> (i.e. I won't be even reading your source code and I might close MR/PR without any
> feedback).

[CPS]: https://cps-org.github.io

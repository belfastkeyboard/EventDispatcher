# Arena
Basic arena implementation

- Static arena, size is fixed, requesting memory beyond limit returns NULL
- Dynamic arena, shrinks and grows as memory is allocated/freed, doubles/halves size

Abstract API, cannot access arena's internal state, must rely on functions.

TODO:
- add support for global arena
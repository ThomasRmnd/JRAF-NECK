# JUNO Reactor Analysis Framework - NTuples Event Cut Kit (JRAF-NECK)

## Organization

### `src`

### `jobs`

Jobs need to be launch in the `jobs` directory in order to work.

### `scripts`

#### `timestamp_sorter`

It appears that some `EDWIN` files do not have their `TTree` sorted by timestamp. To improve the search for correlations between the various reconstruction user files, two `C++` scripts have been created to:
- Check whether a file is sorted (`timestamp_sorted_checker.cpp`)
- Sort an unsorted file (`chain_reader.cpp`)
In addition, `bash` scripts have been created to facilitate the parallelization of these `C++` scripts. 
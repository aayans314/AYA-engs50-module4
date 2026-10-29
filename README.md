# TSE Architecture Chronological

## Week 1 -> Crawler:

Takes in a seed url and will peform bfs on the internal urls on its html until a given depth.

### Pipeline:

- Builds a hash table of webpages by their url lookups to prevent double counting, and pushes webpages to a queue after checking
- Extracts the pages from the queue and puts their content to the specified directory, named chronologically with numbers
- Within that loop, while their depth is within limits, makes the page's children urls into webpages and adds to the queue

## Week 2 -> Indexer:

Performs frequency analysis on words in pages stored in a directory.
Forms an index which is a hash table keyed by word: containing a (wordentry -> which is a queue of (doc -> containing id and word frequency))

### Pipeline:

- Normalizes each word to be lowercased, alphabetical and containing at least 3 characters.
- Looks up if the index contains the word
  1. if not, `addword()` initializes its slot and empty wordentry
  2. it then matches its wordentry's docs with given id of the page containing the word,
    - if doesn't find it, it initializes one doc with given id and count 0
  3. finally for all cases adds 1 to the doc containing the word
- Runs the loop over all webpages loaded from specified directory creating table sequentially
- Saves it hierarchically (`indexio`: hash -> queue) in the specified indexnm

---




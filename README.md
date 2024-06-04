# Search-Engine

## Overview

This project is a simple full-text search engine to search through files. Project includes program that builds a search index and searcher program to do full-text search over created index.

## Components

- **Indexer Application**: Prepares the search index by processing files in a given directory
- **Search Application**: Performs the search using the prepared index

## Indexer Application

Creates search index that stores data on hard drive as BTree.

### Example usage
```bash
indexer /path/to/files /path/where/to/store/database
```

## Search Application

Allow user to make search requests throw console input until "EXIT" word entered. On each request it outputs to console most relevant files with line numbers where words specified in request where met

### Example usage
```bash
searcher nubmer_of_relevant_files_to_show
```

## Searcher Request Syntax


- Supports parentheses and the logical operators AND and OR (case-sensitive). Queries are logical expressions with words separated by spaces.

### Valid Requests

- `"for"`
- `"vector OR list"`

- `"vector AND list"`
- `"(for)"`

- `"(vector OR list)"`
- `"(vector AND list)"`

- `"(while OR for) AND vector"`
- `"for AND and"`

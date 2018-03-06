## Boolean Search with Positional Index
In this project, we implement a simple boolean searcher using a positional
inverted index that were built on Reuters-21578 dataset. This is done in two
steps:

1. Building the positional inverted index. This step requires Reuters-21578
dataset and a list of stopwords.
2. Searching using the already built positional inverted index. This step
doesn't require any data except for the dictionary and index files created using
the indexer. Three search types are supported:
  1. **Conjunctive query**: This is the most basic boolean search query. A
  matching document must contain all the given words in any order.
  An example query is as follows:
  ```
  1 price AND bbc AND crude
  ```
  Here, 1 denotes the type of query. The rest of the query follows after a
  single space.
  2. **Phrase query**: This query requires the given words to occur
  consecutively in the given document. An example query is as follows:
  ```
  2 immediate price increase
  ```
  3. **Proximity query**: This query requires the given words to have at most
  some number of words in between. For example
  ```
  3 citibank /3 reuter /10 money
  ```
  matches documents where reuter words occurs at most 3 words after citibank,
  and money occurs at most 10 words after reuter. Ordering in proximity queries
  are important.

### Requirements
1. g++-5 and above with full C++14 support
2. cmake 3.2.2 and above
3. dirent.h header to retrieve file information. This normally comes installed
C POSIX library. However, if you are on Windows and compiling with MSVC,
you need to obtain this header manually.
4. Porter Stemmer in file src/porter\_stemmer.cpp. For reference page, go to
[https://tartarus.org/martin/PorterStemmer/](https://tartarus.org/martin/PorterStemmer/).

### Build
To build the project, run the following commands in the project root directory

```
chmod +x build.sh
./build.sh
```

After building the project, you should see two executable: indexer and searcher.

### Documentation
You can view the documentation in the source files. Optionally, you can build
the project documentation using doxygen and view it in your browser. To build the
documentation, doxygen must be installed on your system. After installing doxygen,
type the following commands in the project root directory to build the documentation:

```
mkdir -p doc
doxygen Doxyfile
```

To view the documentation, open ```doc/html/index.html``` file with your
browser.

### Running
The build process creates two executables:

#### indexer
This is the executable to index the Reuters dataset and create the dictionary
and index files.

indexer executable expects the following directory layout:

```
indexer
stopwords.txt
Dataset
├── reut2-000.sgm
├── reut2-001.sgm
├── reut2-002.sgm
...
```

To obtain the Reutrs-21578 dataset, please go to
[reuters21578](http://www.daviddlewis.com/resources/testcollections/reuters21578/),
download the gzipped tar archive and copy it into the project root directory.
Then, by running extract\_dataset.sh script, you can create the necessary
dataset layout.

Afterwards to build the index, simply run the indexer using
```
./indexer
```

indexer creates a dictionary file called dict.txt and an index file called
index.txt . To learn about the structure of these files, refer to file\_manager
documentation.

#### searcher
Searcher is the executable that performs boolean search using the index built
by the indexer. There are two ways to run searcher:

1. You can run searcher in interactive mode by simply executing it:
```
./searcher
```
To see the help message, simply press Enter. Invalid queries are reported back
to the user.

2. You can pipe a series of commands to searcher and redirect their output to
files. By default, searcher outputs the matching document IDs to STDOUT and
logging and feedback messages to STDERR. Hence, with a cmd.txt looking like this:

```
1 crude AND oil
2 crude oil
3 crude /5 oil
```

you can use the following command to get the matching document IDs to out and
logging to log:

```
cat cmd.txt | ./searcher > out 2> log
```

Output of each query is separated by a blank line. If you don't care about the
log text, you can always pipe it to /dev/null via

```
cat cmd.txt | ./searcher > out 2> /dev/null
```

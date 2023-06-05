### Program description

This program will implement Lempel-Ziv Compression algorithm, LZ78 specifically. This is a dictionary-based algorithm, where the key is a prefix, also known as a word, and the value is a code that we can utilize for fast look-ups, during compression. The decompression is very similar, except that the key and value for the decompressing dictionary is swapped. Users can compress and decompress their data with this program.

### Quick Start

	download all the souce code
	make 
	./encode -i uncompressed_file -o compressed_file
	./decode -i compressed_file -o  uncompressed_file

### Usage Guidance
	decode:
	USAGE
	 ./decode [-vh] [-i input] [-o output]

	OPTIONS
	 -v Display decompression statistics
	 -i input Specify input to decompress (stdin by default)
	 -o output Specify output of decompressed input (stdout by default)
	 -h Display program help and usage

	encode 
	 USAGE
	 ./encode [-vh] [-i input] [-o output]

	OPTIONS
	 -v Display compression statistics
	 -i input Specify input to compress (stdin by default)
	 -o output Specify output of compressed input (stdout by default)
	 -h Display program help and usage

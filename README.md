The program has two executes to demonstrate the LZ78 Compression algorithm.  
  
Type "make all" in the directory of the project to run the Makefile and create the programs.  
  
encode Executable:  
This program will take in a file as an input with stdin and compress the contents and will output the compressed contents to stdout.  
Type "./encode " once compiled and add the following arguments to run the program.  
-i infile: Sets the file to be compressed, where infile is the desired file.  
-o outfile: Outputs the compressed file, where outfile is the desired file.  
-v: Prints out the compression statistics.  
Example: "./encode -i decodedFile -o encodedFile -v" will take decodedFile as the input and compress the contents, then will output the contents to the file encodedFile. The statistics for this compression will be printed to the terminal.  
  
decode Executable:  
This program will take in a file as an input with stdin and decompress the contents and will output the decompressed contents to stdout.  
Type "./decode " once compiled and add the following arguments to run the program.  
-i infile: Sets the file to be decompressed, where infile is the desired file.  
-o outifle: Outputs the decompressed file, where outfile is the desired file.  
-v: Prints out the compression statistics.  
Example: "./decode -i encodedFile -o decodedFile -v" will take encodedFile as the input and decompress the contents, then will output the contents to the file decodedFile. The statistics for this decompression will be printed to the terminal.  
    
Credit to TA Eugene Chou for references used to implement functions for io.c.  
Credit to Professor Max Dunne for pseudocode used to implement the LZ78 compression algorithms.  

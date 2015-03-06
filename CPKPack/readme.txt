CPKPack is a command line tool developed by Parax that can unpack and repack Dragon Ball Xenoverse .cpk files. It can handle encrypted CPKs as well as decompress compressed files at unpack time. Note that this was made specifically for Dragon Ball Xenoverse and support for CRIWARE CPKs from other games is not guaranteed (in fact, they more likely than not won't work).

=====
Usage
=====

CPKPack [mode] [options]

There are two modes: Extract and Repack.

-------
Extract
-------

The input CPK will be read and its contents will be dumped out to a folder. CPKPack will detect and decrypt encrypted CPKs. Compressed files can optionally be decompressed at unpack time.

Usage: CPKPack -x [INPUT.cpk]

Options:
-d: Disable decompression; this is useful if you intend to repack compressed files
-o [OUTPUT DIRECTORY]: Set output directory. By default CPKPack will extract to a folder with the same name as the CPK. This option allows you to change that if you want.

------
Repack
------

The input folder will be parsed and all files and folders contained in it will be packed into a new CPK. Although there is no recompression support, pre-compressed files will be detected and marked as compressed. If you want to repack compressed files, it's a good idea to unpack the CPK compressed and uncompressed; then you can modify only specific files and repack with everything else left compressed.

Usage: CPKPack -r [INPUT DIRECTORY] [OUTPUT.cpk]
# LibXenoverse
Dragon Ball Xenoverse Modding Tools and Library.

Simple library to handle Dragon Ball Xenoverse's file formats. Supports both little endian (PC) and big endian (Xbox 360) files.

You can also integrate LibXenoverse into your own applications if you wish to add support for these files on a custom game engine, previewer, plugin, etc. Simply build LibXenoverse.lib and link it.

#### Builds
Win32 builds of the tools are provided on the /bin/ directory.

#### [Tools](https://github.com/DarioSamo/LibXenoverse/wiki#tools)
#### [Building (Only required if you want to contribute code)](https://github.com/DarioSamo/LibXenoverse/wiki#building)
#### [Tutorials](https://github.com/DarioSamo/LibXenoverse/wiki#tutorials)

#### Modding the game
It's recommended to use the data folder inside of data2.cpk to override the files from data.cpk. It's much smaller and faster to repack than the main big data.cpk. Use the CPKPack tool to extract and repack Dragon Ball Xenoverse's CPK files.

WARNING: Repacking data2.cpk with the uncompressed files seem to just crash the game at the moment for some reason. Try extracting the files first with extract-data2.bat (leaves the files compressed) and repack those instead.

#### Use XenoViewer
-add esk file for add skeleton
-add emd for add meshes. it's will automatically use emm (material) and emd (textures)
-add ean files  to use animations.

-drag and drop emd on skeleton to apply skinning.
-double-click on an animations to view the result

in view part:
-use left click to turn camera.
-use right click to move camera.
-use mouse wheel to zoom/unzoom.

Note : view part could didn't work with more than one monitor. 

####License
(todo)

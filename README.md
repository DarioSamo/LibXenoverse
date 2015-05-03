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

#### Using XenoViewer
* Opening an ESK file will add an skeleton to the current view.
* Opening EMD files will add meshes to the current view, as well as load its materials and textures automatically from the EMM/EMB files from the same directory.
* Opening EAN files will add animations to the current list.
* You can drag and drop a mesh from the current list on top of a skeleton to apply skinning.
* To play animations, simply double click on one in the animation list.

##### Viewer Controls:
* Left click to spin the camera around the focus point.
* Right click to pan the camera's focus point.
* Mouse wheel to make the camera zoom in or out.

Note : view part could didn't work with more than one monitor. 

####License
(todo)

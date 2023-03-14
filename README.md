# Hera
Copyright Final Fall Games

## Setup
### Setup Unreal Engine Source
[Unreal's setup README](https://github.com/EpicGames/UnrealEngine/blob/release/README.md#getting-up-and-running)

### Launching the UProject from VSCode
- Install VSCode
- Open the `Hera/Hera.code-workspace`
- Select the 'Run and Debug' option in the sidebar
- In the Run and Debug target dropdown select 'Launch HeraEditor (Development) (Hera)'

## Unreal Notes
- Target.cs: They're target rules files -- they tell the Unreal Build System how to build the project.
- Build.cs: They're module build rules files -- they tell the Unreal Build System how to build the module for the target that it's currently building.
- Modules: To make a module (like HeraCore) you need a ModuleName.Build.cs, a ModuleName.h header file and add the module to a Target.cs. Every Unreal project needs exactly 1 module designated as the primary game module. 



## VSCode - Unreal Snippets
### Common
- umh: Boilerplate for header files. 
- ull: Unreal log entry.
- uca: Boilerplate for an Actor header file.
- upc: Component property.
- upe: Editable propery.
- ufc: Constructor.

### Setup Related
- uuproj: Boilerplate text for a uproject file. Needs the "Name" field to be filled in and correct engine version.
- umt: Boilerplate for Target.cs files. Need to specify source modules to be built for the target.
- umb: Boilerplate for Build.cs files. Need to fill in the list of modules that this module depends on. 
- umcp: Boilerplate specifically for a cpp file for the primary module in the project.
- ulh: Boilerplate for a module's log header file.
- ulc: Boilerplate for a module's log cpp file.


### Project Commands
The scripts can be found in `Hera/scripts`.
#### Bat Files
- build: builds the editor for Win64 Development
- editor: launches the editor 
- build && editor: builds the editor and if successful launches it

#### Raw Commands 
this functionality is in the build.bat and editor.bat scripts
- Build Editor: 
"C:\Program Files\Unreal Engine\UE_5.1.0\Engine\Build\BatchFiles\Build.bat" HeraEditor Win64 Development "C:\Users\name\Documents\FinalFall\Hera\Hera.uproject" -waitmutex -NoHotReload

- Build game executable: 
"C:\Program Files\Unreal Engine\UE_5.1.0\Engine\Build\BatchFiles\Build.bat" Hera Win64 Development "C:\Users\name\Documents\FinalFall\Hera\Hera.uproject" -waitmutex -NoHotReload

- Launch Editor: 
"C:\Program Files\Unreal Engine\UE_5.1.0\Engine\Binaries\Win64\UnrealEditor.exe" "C:\Users\name\Documents\FinalFall\Hera\Hera.uproject" -log

- Launch Editor as game: 
"C:\Program Files\Unreal Engine\UE_5.1.0\Engine\Binaries\Win64\UnrealEditor.exe" "C:\Users\name\Documents\FinalFall\Hera\Hera.uproject" -game -log -windowed -resx=1280 -resy=720

- Cook Assets: 
"C:\Program Files\Unreal Engine\UE_5.1.0\Engine\Binaries\Win64\UnrealEditor-cmd.exe" "C:\Users\name\Documents\FinalFall\Hera\Hera.uproject" -run=cook -targetPlatform=Windows

- Launch executable after being Built and Cooked:
"C:\Users\name\Documents\FinalFall\Hera\Binaries\Win64\Hera.exe" -log -windowed -resx=1280 -resy=720
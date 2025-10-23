# README

This is the template project folder for your lab. Feel free to modify the files
in here, as long as they remain in this folder. This is where TAs will look for
when running your project and/or inspecting your code for software quality.

## Creating a New Project (Keil)

1. Open up Keil.
2. Click on the 'Project' tab on the top toolbar.
3. Click on New uVision Project.
4. Create a new .uvproj file of the desired name (typically Lab1, Lab5, etc) in the SW folder.
5. Search for the 'TM4C123GH6PM' device in the 'Select Device for Target 'Target 1' window.
6. Press OK and then press OK again in the 'Manage Run-Time Environment' window. 

## Managing Project Files

1. Right click on the 'Target 1' item in the Project sidebar window.
2. Click on 'Manage Project Items' in the submenu.
3. In the 'Files' column, click on the 'Add Files' button.
4. Add the implementation files (.C and .s files) required for the project
   dependencies into the project. This includes your main file. In particular,
   make sure you include the startup.s file, since this includes things like
   interrupt definitions.

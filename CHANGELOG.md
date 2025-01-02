# Changelog
All notable changes to this project will be documented in this file.

## [0.0.0]
- The initial version development with a stable version of the MapEditor.

## [0.1.0]

### Game
- First steps in the game application.
- Displayed the map using modern OpenGL.
- Displayed the player on the map.
- Added the window resize feature.
- Added an option to display the FPS.

### Map Editor
- Added the map resize feature
- Added a tool to configure the cannot step-on tile.

## [0.2.0]

### Game

#### New features
- Added GLTextBox support to display messages in the game.
- Added a message pipeline in the game.
- Added GLPopupWindow support to display inventory, character infos etc.
- Added the Main menu when in game map mode.
- Added the inventory window mode when in game map mode. You can equip, move and drop items.
- Added the character window mode when in game map mode.
- Added a mechanism to make sure we cannot open the chest multiple times.
- Added joystick support for the different windows.

#### Bug fixes
- Fix joystick player movement issue and start working on the InputDevicesState class.

### Item Editor
- Create the Item Editor to manage items, stats items, weapons and armors.

### Map Editor (Version 1.3.0)

#### New features
- Added the X and Y coordinates of the selected tile.
- Added a flag to indicate if an object texture needs to appear in front or behind the player.
- Added tile's trigger and action feature.
- Added icons in the toolbar.
- Added the Edit, View and Clear Blocked Border Mode features.

#### Bug fixes
- Fix an issue where the initShader method of the GLTextService and GLTileService was not returning any value on success.

## [0.3.0]

### Game

#### New features
- Add gold stats in the Character window

### Map Editor (Version 1.3.1)

#### New features
- Added a setting form to manage item stores.
- Added a setting form to manage monster stores.
- Added the Monster zones component in the main form.

### Monster Editor
- Create the Monster Editor to manage monsters with their stats.

## [0.3.1]

### Map Editor (Version 1.3.2)

#### New features
- Added the assign/clear monster zones to the map feature.

#### Bug fixes
- When selecting multiple tiles (e.g. with a texture assigned), the resulting selection could be incorrect if you stop on the edge of a tile.

### Monster Editor (Version 1.0.0)

#### New features
- Implemented the CLI program options to be able to load a monster store directly from the CLI.
- Draw the tiles zones in the Add & Edit Texture form.

### Item Editor (Version 1.0.0)

#### New features
- Implemented the CLI program options to be able to load a monster store directly from the CLI.
- Draw the tiles zones in the Add & Edit Texture form.

## [0.3.2]

### Map Editor (Version 1.3.3)

#### New features
- Updated the upper toolbar to make the selected action visually marked as selected.
- It is now possible to move the differents toolbar to the left and the right of the screen.
You can also close them and made them floatable. The menu View > Toolbar has been added
to toggle toolbars visibility.
- Added a debugging info toolbar that is hidden by default. You can open it through the
View > Toolbar menu.
- The map section of the editor is supporting dynamic size.
- Added the zoom feature to the map.
- Removed the NPC component that was unused for now.
- Relocated MainForm components as real QWidget independant components.
- Implemented the Double click and Del key press in the Texture List Component.
- Implemented the Double click and Del key press in the Tile Properties Component for the Event list.
- Enhanced the current texture edit form by using the common ui texture edit form controller.

#### Bug fixes
- Solved Issue # 10: When you unselect a tile the tiles details stays in the Tile Properties Component.

## [0.3.3]

### Map Editor (Version 1.3.4)

#### Change
- Move the editor configuration file into the folder "Jed# Software/The Warrior - Map Editor"
instead of "The Warrior - Map Editor"

#### New features
- Persist the position status of toolbars.
- Persist the visibility status of toolbars.
- Persist the visiblity status of the display grid menu item.
- Add a shortcut to move the map -> Alt + Click.
- Enable the zoom feature with the mouse wheel.
- Implement the undo/redo actions.
- Add a Picker tool to get a texture an object from a tile.
- Add support to change multi-selection tile properties.
- Add bottom tabs to change the map view (ex: CanStep, MonsterZone, BlockedBorder etc).
- Persist the "Use only one monster zone for all the map" field in the GameMap class.

#### Bug fixes
- Solved Issue # 6: Application crash when you multiselect a zone that ends outside of the GLComponent.

### Item Editor (Version 1.0.1)

#### Change
- Move the editor configuration file into the folder "Jed# Software/The Warrior - Item Editor"
instead of "The Warrior - Item Editor"

### Monster Editor (Version 1.0.1)

#### Change
- Move the editor configuration file into the folder "Jed# Software/The Warrior - Monster Editor"
instead of "The Warrior - Monster Editor"

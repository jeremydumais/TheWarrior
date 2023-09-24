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
- Added the Monster zones component in the main form

### Monster Editor
- Create the Monster Editor to manage monsters with their stats

## [0.3.1]

### Map Editor (Version 1.3.2)

#### New features
- Added the one monster zone for all the map feature.


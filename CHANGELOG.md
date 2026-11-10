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
- Solved Issue #10: When you unselect a tile the tiles details stays in the Tile Properties Component.

## [0.3.3]

### Map Editor (Version 1.3.4)

#### Change
- Move the editor configuration file into the folder "Jed# Software/The Warrior - Map Editor"
instead of "The Warrior - Map Editor"
- Only the tiles that are visible on the screen are now displayes. This result
in lower CPU usage.

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
- Add a MonsterZone selection component in the Tile Property panel.^
- Disable the MonsterZone combobox in tile props component when "Use only one monster zone for all the map" is selected.
- Implement the copy/paste of tiles feature.

#### Bug fixes
- Solved Issue #6: Application crash when you multiselect a zone that ends outside of the GLComponent.
- Solved Issue #14: When creating the first monster zone, it is assigned automatically to the selected tiles.
- Solved Issue #15: Undo history is not update when we change tile value via the Tile Property panel.
- Solved Issue #13: Application crash when you apply texture on map then same on another file.
- Solved Issue #9: Unable to select a tile when the map is moved in the lower right corner.


### Item Editor (Version 1.0.1)

#### Change
- Move the editor configuration file into the folder "Jed# Software/The Warrior - Item Editor"
instead of "The Warrior - Item Editor"

### Monster Editor (Version 1.0.1)

#### Change
- Move the editor configuration file into the folder "Jed# Software/The Warrior - Monster Editor"
instead of "The Warrior - Monster Editor"

## [0.3.4]

### Game

#### New features
- Add the battle system and monster encounter. Fight and Run action items are
now supported.
- Add the Damage system (Player and Monsters).

#### Optimization
- Now only the tiles visible on screen are processed and displayed with OpenGL.

### Monster Editor (Version 1.0.2)

#### New features
- Add the Experience range fields for monster.

#### Change
- The monster health is now a range instead of single value.

### Map Editor (Version 1.3.5)

#### Change
- Ensure that at least one Monster encounter has a ratio of Normal.

## [0.4.0]

### Game

#### New features
- Add a Exit Game choice in the popup menu of the game.
- Add a Save choice in the popup menu of the game.
- Create the main menu screen and implement the New game (choose player name)
and Quit actions.

## [0.5.0]

### Game

#### New features
- Added save and load game functionality.

#### Change
- Reworked the visuals of the New Game and Load Game menus.

#### Bug fixes
- Fixed an issue where main menu text elements were incorrectly positioned after returning from the game.
- Fixed text misalignment in the game menu when resizing the window while the menu was visible.
- Fixed the scale property not being applied correctly in the Message Pipeline.

## [0.6.0]

### Game

#### New features
- Added NPCs with configurable facing, collision, and random wandering within assigned zones. NPCs face the player during conversations and resume their behavior afterward.
- Added paginated NPC dialogue with a typewriter effect and sound, branching choices, and optional cancellation. Player movement is blocked during dialogue.
- Added conversation scenarios for NPCs and tile triggers, supporting item and gold rewards, inn stays, merchant shops, scripted battles, and story progression.
- Added merchant buying and selling with confirmation dialogs, configurable inventories, and item prices.
- Added paid inn stays that restore health, with sleep effects and audio.
- Added persistent completed story IDs, story-dependent conversations, and conditional NPC visibility that can update as the story progresses. Scripted battle victories can advance conversations and story events.
- Added boss types, configurable battle music, boss victory effects, and restrictions on fleeing boss battles.
- Added vitality item use during battles. Healing and consumption occur on the player's turn; canceling item selection does not spend a turn.
- Added map-specific background music and battle backgrounds. Music continues between maps using the same track and resumes after battles.
- Added sound effects for combat, level-ups, inventory actions, dialogue, and merchant transactions.
- Added gold rewards in treasure chests.
- Added a configurable movement cooldown between random encounters through `resources/gameplay.json` (five completed tile moves by default).
- Added an overlay and delay during map transitions.

#### Content and presentation
- Expanded and refreshed Krikru Village, King Aldric's castle and outer bailey, the home house, shops, and church.
- Added the three-level North West Cave with treasures and monster zones, and continued development of the Outworld map.
- Updated the main menu background, tilesets, and monster artwork.
- Adjusted player experience progression and combat calculations so weaker attackers can occasionally hit for minimal damage.
- New games begin in the home house with the player facing down. The startup window uses the primary display's dimensions.

#### Bug fixes
- Fixed running while holding Shift and directional input handling for joysticks.
- Return to the main menu after battle defeat music and confirmation, and prevent input from carrying over into subsequent screens.
- Fixed visible tile seams, UI texture sampling, and on-screen keyboard label alignment.
- Fixed player sprite initialization to respect the stored facing direction in new and loaded games.

### Map Editor (Version 1.4.0)

#### New features
- Added NPC creation, editing, deletion, texture and spawn selection, facing, behavior, wandering zones, and story-based visibility settings, persisted in map files.
- Added an NPC list, selection outlines, map visibility toggles, and tools to apply or clear wandering zones.
- Added conversation scenario editing with dialogue, choices, actions, and node transitions.
- Added merchant inventory editing with item selection and pricing. Inventories referenced by conversation scenarios cannot be renamed or deleted.
- Added per-map background music and battle background configuration.
- Added a tile option allowing interaction through objects such as counters.
- Added multi-tile texture and object drag-and-drop with placement previews and undo support.
- Added an optional grid in the texture selection component.

#### Change
- Reorganized the View menu and added NPC wandering zone and monster zone toolbars.
- Updated the editor icon.

#### Bug fixes
- Preserved NPC wandering zones when editing NPC properties.
- Added safeguards against resizing away NPC spawn tiles, blocking occupied NPC tiles, or deleting textures used by NPCs.
- Clear NPC data when opening another map and indicate NPCs without dialogue during editing.

### Item Editor (Version 1.0.2)

#### New features
- Added default buy and sell prices for items, weapons, armor, and stat items.
- Added item grids with icons in the main window.

#### Change
- Changed stat item duration configuration from seconds to turns.
- Updated the editor icon.

### Monster Editor (Version 1.0.3)

#### New features
- Added monster type and battle music configuration for regular and boss encounters.

#### Change
- Updated the editor icon.

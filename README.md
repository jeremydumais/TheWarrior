<img src="https://github.com/jeremydumais/TheWarrior/blob/medias/TheWarriorLogoSmall.png?raw=true"
     align="left" width="72" style="margin-right: 1rem;" />

# **TheWarrior**
*A retro-inspired RPG built from scratch in modern C++*

[![Build](https://github.com/jeremydumais/TheWarrior/actions/workflows/ci.yml/badge.svg)](https://github.com/jeremydumais/TheWarrior/actions/workflows/cmake.yml)
![Latest version](https://img.shields.io/badge/version-0.6.0-brightgreen)
![Status](https://img.shields.io/badge/status-active_development-blue)

---

## 🎮 Overview

**TheWarrior** is a handcrafted **RPG game developed in C++ using SDL2**, inspired by classic RPG mechanics while leveraging modern architecture, tools, and workflows.

The project focuses on:
- Clean, modular C++20 design
- Custom editors (Map, Items, Monsters)
- Turn-based combat
- Persistent game states
- Tooling-first development (editors before content)

> 🚧 **The game is actively developed** — features are added sprint by sprint.

---

## 🚀 Current Version

**v0.6.0**

- Core engine foundation in place
- Functional editors (Map, Item, Monster)
- Early combat system
- UI & menu system actively evolving
- Early NPC system

---

## 🧭 Next Sprint — NPC Development

The next development sprint focuses on adding more features on the **Non-Playable Characters (NPCs)**.

<p align="center">
  <img src="https://github.com/jeremydumais/TheWarrior/blob/medias/NextSprintNPC.png?raw=true" alt="Next Sprint - NPC Development" />
</p>

### Planned NPC Features
- Dialogue system
- Merchant interactions
- Animations & visual feedback
- Integration with maps and quests

---

## ✅ Development Progress

| Area | Feature | Status |
|-----|--------|--------|
| **Core Gameplay** | Game Engine | ✅ Done |
| | Combat System | 🟡 In progress |
| | NPC | 🟡 In progress |
| | Trading | ⏳ Not started |
| | Story & Quests | ⏳ Not started |
| **Game Presentation** | Audio & Music | 🟡 In progress |
| | Menus & Navigation | 🟡 In progress |
| **Tools** | Item Editor | 🟡 In progress |
| | Map Editor | 🟡 In progress |
| | Monster Editor | 🟡 In progress |

Legend:
- ✅ Done
- 🟡 In progress
- ⏳ Planned

---

## 📸 Screenshots

### 🌍 Game World — Sample Map
![Sample Map](https://raw.githubusercontent.com/jeremydumais/TheWarrior/medias/SampleMap1.png)

### 🎒 Inventory System
![Inventory](https://raw.githubusercontent.com/jeremydumais/TheWarrior/medias/GameInventoryWindow.png)

### 🧙 Character Window
![Character Window](https://raw.githubusercontent.com/jeremydumais/TheWarrior/medias/GameCharacterWindow.png)

### ⚔️ Battle System
![Battle System](https://raw.githubusercontent.com/jeremydumais/TheWarrior/medias/BattleSystem.png)

---

## 🛠️ Tools & Editors

### 🗺️ Map Editor
![Map Editor](https://raw.githubusercontent.com/jeremydumais/TheWarrior/medias/MapEditor1.png)

### 🧾 Item Editor
![Item Editor](https://raw.githubusercontent.com/jeremydumais/TheWarrior/medias/ItemEditor1.png)

### 👹 Monster Editor
![Monster Editor](https://raw.githubusercontent.com/jeremydumais/TheWarrior/medias/MonsterEditor1.png)

---

## 🧰 Build Instructions

### 🐧 Linux

Full step-by-step instructions are available in the wiki:

👉 **[How to build TheWarrior from source on Linux](https://github.com/jeremydumais/TheWarrior/wiki/How-to-build-The-Warrior-from-source-in-Linux)**

---

## 📌 Project Philosophy

- **Tools before content**
- **Strong separation of concerns**
- **Readable, testable C++**
- **Game systems designed to scale**

This project is both a game and a long-term learning playground for engine architecture, tooling, and clean C++ design.

---

⭐ If you like the project, feel free to star it and follow development!


## Random encounter cooldown

Set `randomEncounters.minimumMovements` in `resources/gameplay.json` to the
minimum number of completed tile moves before another random encounter roll
(default: 5). The fifth move is eligible when the setting is 5; earlier moves
skip the roll entirely. Set 0 to disable the cooldown. Missing or invalid values
fall back to 5.

Random and scripted battles restart the cooldown when they start. Loading a map,
including a same-map teleport or a saved game, also restarts it. Completed moves
on all tiles count, including trigger tiles and tiles outside monster zones.
Blocked movement, menus, NPC interactions, dialogue, and combat transitions do
not advance or reset the count. The cooldown is session state and is not saved.

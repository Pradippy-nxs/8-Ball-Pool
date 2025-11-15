Struktur File:
project-root/
├── asset/
├── build/ *Setelah build pake CMake*
├── main_app/
│   ├── ENTITIES/
│   │   ├── Ball.h
│   │   ├── Entity.h
│   │   ├── Game.cpp
│   │   ├── Game.h
│   │   ├── Stick.h
│   │   ├── Table.cpp
│   │   └── Table.h
│   │
│   ├── GUI/
│   │   ├── .gitkeep
│   │   ├── Button.h
│   │   ├── HUD.h
│   │   └── Menu.h
│   │
│   ├── LOGIC/
│   │   ├── .gitkeep
│   │   ├── Collision.h
│   │   ├── GameRules.h
│   │   ├── Input.h
│   │   └── Physics.h
│   │
│   ├── UTILITIES/
│   │   ├── .gitkeep
│   │   └── Debug.h
│   │
│   └── main.cpp
│
├── .gitignore
├── CMakeLists.txt *Jangan lupa bikin ya*
├── CMakeListsInstructions.txt
└── INSTRUCTIONS.md

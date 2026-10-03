# 📚 Smart Library — ESP32-S3

<p align="center">
  <img src="photos_circuit/FindBook_demo.png" alt="Smart Library ESP32-S3 Prototype" width="780">
</p>

<h1 align="center">⚡ Smart Library</h1>

<p align="center">
  <b>A completed Smart Library prototype built around a single ESP32-S3 board.</b>
</p>

<p align="center">
  🔎 Find Books &nbsp;•&nbsp; 📡 Identify Books &nbsp;•&nbsp; 📖 Issue &nbsp;•&nbsp; ↩️ Return &nbsp;•&nbsp; 🚨 Security
</p>

<p align="center">
  <img src="https://img.shields.io/badge/ESP32--S3-Core-0F766E?style=for-the-badge&logo=espressif">
  <img src="https://img.shields.io/badge/PlatformIO-Development-F58220?style=for-the-badge&logo=platformio">
  <img src="https://img.shields.io/badge/C%2FC%2B%2B-Firmware-00599C?style=for-the-badge&logo=cplusplus">
  <img src="https://img.shields.io/badge/Prototype-Completed-16A34A?style=for-the-badge">
</p>

---

## 🌟 1. What is Smart Library?

Finding a book is not always as simple as knowing that it is **available**.

A student may know:

```text
📚 Book: Available
```

but still have to ask:

```text
❓ Which shelf?
❓ Which section?
❓ Which row?
❓ How do I issue it?
❓ How do I return it?
```

Our project explores a simple idea:

> **Can one ESP32-S3 turn the library into an interactive, guided experience?**

The result is a completed prototype that combines **book discovery, physical identification, issue/return interaction, visual indication and security-oriented feedback** around a single ESP32-S3 board.

---

# 💡 2. From Inspiration to Our Own Idea

The project was inspired by the **smart-library problem described in a reference article/document**. The reference concept discusses book-location difficulties, RFID-based transactions, shelf guidance and security. fileciteturn1file0L15-L26

The reference architecture is broader and uses multiple controllers and subsystems. fileciteturn1file0L5-L12

Instead of reproducing that architecture, we asked:

### 🧠 “What if we rethink the experience around ONE ESP32-S3?”

That became our engineering direction.

```mermaid
flowchart LR
    A["📄 Reference Idea"] --> B["💭 Our Interpretation"]
    B --> C["⚡ Single ESP32-S3"]
    C --> D["🧩 Connect Required Peripherals"]
    D --> E["🧪 Build & Test"]
    E --> F["🎓 Completed Prototype"]

    classDef start fill:#334155,stroke:#64748b,color:#fff
    classDef idea fill:#164e63,stroke:#22d3ee,color:#fff
    classDef core fill:#065f46,stroke:#34d399,color:#fff
    classDef final fill:#166534,stroke:#4ade80,color:#fff

    class A start
    class B idea
    class C core
    class D idea
    class E idea
    class F final
```

### Our design principle

> **Take the problem → rethink the architecture → build a compact working solution.**

---

# 🧭 3. How the Whole System Works

Think of the ESP32-S3 as the **brain of the library kiosk**.

```mermaid
flowchart TD
    U["👤 USER"] --> UI["🖥️ ESP32-S3 INTERACTIVE INTERFACE"]

    UI --> M{"What does the user want?"}

    M -->|Find a book| F["🔎 FIND BOOK"]
    M -->|Issue a book| I["📖 ISSUE BOOK"]
    M -->|Return a book| R["↩️ RETURN BOOK"]

    F --> L["📍 Locate / Indicate Book"]
    I --> ID["📡 Identify & Validate"]
    R --> ID

    L --> FB["💡 Visual Feedback"]
    ID --> S{"Result?"}

    S -->|Successful| OK["✅ Confirm Action"]
    S -->|Problem / Security Event| AL["🚨 Alert"]

    FB --> END["👤 USER UNDERSTANDS RESULT"]
    OK --> END
    AL --> END

    classDef user fill:#1e3a5f,stroke:#60a5fa,color:#fff
    classDef core fill:#064e3b,stroke:#34d399,color:#fff
    classDef action fill:#164e63,stroke:#22d3ee,color:#fff
    classDef decision fill:#713f12,stroke:#facc15,color:#fff
    classDef success fill:#166534,stroke:#4ade80,color:#fff
    classDef alert fill:#7f1d1d,stroke:#f87171,color:#fff

    class U,END user
    class UI core
    class F,I,R,L,ID,FB action
    class M,S decision
    class OK success
    class AL alert
```

### In simple words

**1. User interacts with the system**  
The user starts from the ESP32-S3 interface.

**2. ESP32-S3 decides what operation is required**  
Find, Issue or Return.

**3. The required hardware interaction happens**  
For example, RFID can be used when physical identification is required.

**4. The ESP32-S3 processes the result**  
The system determines what should happen next.

**5. The user gets immediate feedback**  
The display, LEDs or alert mechanism communicates the result.

---

# 🧠 4. The ESP32-S3 is the Central Brain

Instead of thinking about the project as many independent circuits, think of it like this:

```mermaid
flowchart TB
    U["👤 USER"] --> ESP["⚡ ESP32-S3<br/>CENTRAL CONTROLLER"]

    ESP --> UI["🖥️ User Interface"]
    ESP --> RFID["📡 RFID"]
    ESP --> LED["💡 Visual Indicators"]
    ESP --> ALERT["🔊 / 🚨 Alerts"]
    ESP --> LOGIC["🧠 Library Logic"]

    UI --> ESP
    RFID --> ESP
    LED --> ESP
    ALERT --> ESP
    LOGIC --> ESP

    classDef esp fill:#065f46,stroke:#34d399,color:#fff,stroke-width:3px
    classDef part fill:#0f172a,stroke:#38bdf8,color:#fff
    classDef user fill:#1e3a5f,stroke:#60a5fa,color:#fff

    class ESP esp
    class UI,RFID,LED,ALERT,LOGIC part
    class U user
```

### Why this approach?

| Traditional approach | Our approach |
|---|---|
| Several controller boards | **One ESP32-S3 core** |
| Distributed logic | **Centralized control** |
| More inter-board communication | **Simpler central architecture** |
| Harder to explain | **Clear user → ESP32-S3 → result flow** |
| More hardware coordination | **Compact prototype** |

> The purpose is not to claim that one board can replace every possible production architecture. Our goal was to explore how much of the **prototype experience** could be achieved with a single capable board.

---

# 🔎 5. Feature 01 — Find a Book

This is the feature that directly addresses the main library problem.

### User journey

```mermaid
flowchart LR
    A["👤 User"] --> B["🔎 Select / Search Book"]
    B --> C["🧠 ESP32-S3 Processes Request"]
    C --> D["📚 Book Identified"]
    D --> E["📍 Location / Shelf Indication"]
    E --> F["💡 User Finds Book"]

    classDef a fill:#1e3a5f,stroke:#60a5fa,color:#fff
    classDef b fill:#164e63,stroke:#22d3ee,color:#fff
    classDef c fill:#065f46,stroke:#34d399,color:#fff
    classDef d fill:#166534,stroke:#4ade80,color:#fff

    class A a
    class B,C,D,E b
    class F d
```

### What the user sees

The user does not need to understand the wiring, GPIO pins or RFID protocol.

They simply follow:

```text
SEARCH
  ↓
SELECT
  ↓
LOCATE
  ↓
FIND 📚
```

### 📸 Find Book — Demo

<p align="center">
  <img src="photos_circuit/FindBook_demo.png" alt="Find Book Demo" width="720">
</p>

The screen represents the starting point of the book-finding interaction.

### 📸 After Selecting a Book

<p align="center">
  <img src="photos_circuit/after_selection_of_book.png" alt="Book Selection" width="720">
</p>

After selection, the system moves toward identifying and indicating the requested book.

---

# 📡 6. Feature 02 — RFID Identification

RFID provides the bridge between the **digital system** and the **physical book**.

```mermaid
flowchart TD
    B["📚 Physical Book"] --> T["🏷️ RFID Tag"]
    T --> R["📡 RFID Reader"]
    R --> E["⚡ ESP32-S3"]
    E --> V{"Identification Valid?"}
    V -->|Yes| OK["✅ Continue Operation"]
    V -->|No| X["⚠️ Request / Show Alert"]

    classDef physical fill:#334155,stroke:#94a3b8,color:#fff
    classDef core fill:#065f46,stroke:#34d399,color:#fff
    classDef decision fill:#713f12,stroke:#facc15,color:#fff
    classDef good fill:#166534,stroke:#4ade80,color:#fff
    classDef bad fill:#7f1d1d,stroke:#f87171,color:#fff

    class B,T,R physical
    class E core
    class V decision
    class OK good
    class X bad
```

### Why RFID?

A book has a physical identity.

RFID allows that identity to be read electronically, giving the ESP32-S3 information that can be used by the library workflow.

### 📸 RFID Reader

<p align="center">
  <img src="photos_circuit/RFID_scanner.png" alt="RFID Scanner" width="720">
</p>

### 📸 Book Indication

<p align="center">
  <img src="photos_circuit/indication_of_book.png" alt="Book Indication" width="720">
</p>

---

# 📖 7. Feature 03 — Issue a Book

The issue process can be understood as a simple decision flow.

```mermaid
flowchart TD
    A["📖 User chooses ISSUE BOOK"] --> B["📡 Scan / Identify Book"]
    B --> C["⚡ ESP32-S3 Processes Identification"]
    C --> D{"Can the transaction continue?"}

    D -->|Yes| E["📝 Issue Operation"]
    D -->|No| F["⚠️ Show Problem"]

    E --> G["✅ BOOK ISSUED"]
    F --> H["👤 User Can Try Again"]

    classDef user fill:#1e3a5f,stroke:#60a5fa,color:#fff
    classDef process fill:#164e63,stroke:#22d3ee,color:#fff
    classDef decision fill:#713f12,stroke:#facc15,color:#fff
    classDef success fill:#166534,stroke:#4ade80,color:#fff
    classDef alert fill:#7f1d1d,stroke:#f87171,color:#fff

    class A user
    class B,C,E process
    class D decision
    class G success
    class F,H alert
```

### 📸 Issue Screen

<p align="center">
  <img src="photos_circuit/issue_book_demo.png" alt="Issue Book Screen" width="720">
</p>

The interface clearly tells the user what to do next.

### 📸 Successful Issue

<p align="center">
  <img src="photos_circuit/issue_success.png" alt="Book Issued Successfully" width="720">
</p>

The success screen gives immediate confirmation.

---

# ↩️ 8. Feature 04 — Return a Book

The return workflow follows the same user-friendly principle.

```mermaid
flowchart TD
    A["↩️ User chooses RETURN BOOK"] --> B["📡 Scan / Identify"]
    B --> C["⚡ ESP32-S3 Processes Request"]
    C --> D["📝 Update Return State"]
    D --> E["✅ RETURN SUCCESS"]

    classDef user fill:#1e3a5f,stroke:#60a5fa,color:#fff
    classDef process fill:#164e63,stroke:#22d3ee,color:#fff
    classDef success fill:#166534,stroke:#4ade80,color:#fff

    class A user
    class B,C,D process
    class E success
```

### 📸 Return Screen

<p align="center">
  <img src="photos_circuit/return_book_demo.png" alt="Return Book Screen" width="720">
</p>

### 📸 Successful Return

<p align="center">
  <img src="photos_circuit/return_success.png" alt="Return Success" width="720">
</p>

The interface gives the user a clear completion state.

---

# 💡 9. Feature 05 — Visual Feedback

A good embedded system should not make the user guess what happened.

Every important operation should end with a visible response.

```mermaid
flowchart LR
    E["⚡ ESP32-S3 Event"] --> D{"What happened?"}
    D -->|Success| G["🟢 SUCCESS"]
    D -->|Attention| Y["🟡 ATTENTION"]
    D -->|Security / Error| R["🔴 ALERT"]

    G --> U["👤 User understands"]
    Y --> U
    R --> U

    classDef core fill:#065f46,stroke:#34d399,color:#fff
    classDef decision fill:#713f12,stroke:#facc15,color:#fff
    classDef green fill:#166534,stroke:#4ade80,color:#fff
    classDef yellow fill:#854d0e,stroke:#facc15,color:#fff
    classDef red fill:#7f1d1d,stroke:#f87171,color:#fff
    classDef user fill:#1e3a5f,stroke:#60a5fa,color:#fff

    class E core
    class D decision
    class G green
    class Y yellow
    class R red
    class U user
```

This creates a simple **machine → human communication loop**:

```text
EVENT
 ↓
PROCESS
 ↓
DECISION
 ↓
FEEDBACK
 ↓
USER KNOWS WHAT HAPPENED
```

### 📸 Hardware Demonstration

<p align="center">
  <img src="photos_circuit/circuit.png" alt="Smart Library Circuit" width="760">
</p>

---

# 🚨 10. Feature 06 — Security-Oriented Detection

A smart library should also consider what happens when a book moves through a controlled area.

The prototype demonstrates an identification-based security concept:

```mermaid
flowchart TD
    A["📚 Book / Identification Event"] --> B["📡 Read Identity"]
    B --> C["⚡ ESP32-S3"]
    C --> D{"Expected State?"}

    D -->|Yes| E["🟢 Normal Operation"]
    D -->|No| F["🚨 Security Alert"]

    classDef input fill:#334155,stroke:#94a3b8,color:#fff
    classDef core fill:#065f46,stroke:#34d399,color:#fff
    classDef decision fill:#713f12,stroke:#facc15,color:#fff
    classDef normal fill:#166534,stroke:#4ade80,color:#fff
    classDef alert fill:#7f1d1d,stroke:#f87171,color:#fff

    class A,B input
    class C core
    class D decision
    class E normal
    class F alert
```

### 📸 Security Demonstration

<p align="center">
  <img src="photos_circuit/anti_theft_alarm.png" alt="Anti Theft Alarm" width="720">
</p>

The prototype uses feedback to communicate when a security-related condition is detected.

---

# 🔄 11. One Complete User Journey

The easiest way to understand the project is to follow one user from start to finish.

```mermaid
flowchart TD
    A["👤 Student enters library"] --> B["🖥️ Opens Smart Library Interface"]
    B --> C["🔎 Searches for a book"]
    C --> D["📚 Selects required book"]
    D --> E["📍 System indicates location"]
    E --> F["👋 Student finds the book"]
    F --> G["📡 Book is identified"]
    G --> H["📖 Student chooses Issue"]
    H --> I["⚡ ESP32-S3 processes transaction"]
    I --> J["✅ Issue confirmed"]

    J -. Later .-> K["↩️ Student returns book"]
    K --> L["📡 Book identified again"]
    L --> M["⚡ ESP32-S3 processes return"]
    M --> N["✅ Return confirmed"]

    classDef user fill:#1e3a5f,stroke:#60a5fa,color:#fff
    classDef system fill:#065f46,stroke:#34d399,color:#fff
    classDef action fill:#164e63,stroke:#22d3ee,color:#fff
    classDef success fill:#166534,stroke:#4ade80,color:#fff

    class A,B,F,K user
    class C,D,E,G,H,L action
    class I,M system
    class J,N success
```

### In one sentence:

> **The ESP32-S3 receives the user's intention, performs the required library interaction, processes the physical identification and communicates the result back to the user.**

---

# 🧩 12. Hardware-to-Software Flow

The project is easier to understand if we separate the physical layer from the logic layer.

```mermaid
flowchart LR
    subgraph PH["🔌 PHYSICAL WORLD"]
        R["📡 RFID"]
        D["🖥️ Display / Touch"]
        L["💡 LEDs"]
        A["🔊 Alert"]
        B["📚 Book"]
    end

    subgraph CORE["⚡ ESP32-S3"]
        I["Input Handling"]
        P["Library Logic"]
        S["System State"]
        O["Output Control"]
    end

    B --> R
    R --> I
    D --> I
    I --> P
    P --> S
    S --> O
    O --> D
    O --> L
    O --> A

    classDef physical fill:#334155,stroke:#94a3b8,color:#fff
    classDef core fill:#065f46,stroke:#34d399,color:#fff

    class R,D,L,A,B physical
    class I,P,S,O core
```

### This means:

**Input → Processing → Decision → Output**

That is the fundamental loop behind the prototype.

---

# ✨ 13. Interesting Design Features

### ⚡ Single-board architecture

The entire prototype is centered around **one ESP32-S3 board**.

### 🧠 Centralized control

Instead of spreading the main prototype logic across several controllers, the ESP32-S3 acts as the central decision point.

### 📡 Physical + digital interaction

The project connects:

```text
Digital interface
      ↕
ESP32-S3
      ↕
Physical book
      ↕
RFID
```

### 👤 User-first interaction

The system is designed around what the user needs to see and do, rather than around the electronics.

### 💡 Immediate feedback

Successful actions and alert conditions are communicated visually.

### 🧩 Modular thinking

Although the core is one board, the prototype can still be extended with additional peripherals.

---

# 🔧 14. Main Hardware & Technology

| Layer | Technology / Component |
|---|---|
| 🧠 Main Controller | **ESP32-S3** |
| 💻 Development | **VS Code + PlatformIO** |
| ⚙️ Firmware | **C/C++** |
| 📡 Identification | **MFRC522 RFID** |
| 🖥️ Interface | ESP32-S3 display/touch interface used in the prototype |
| 💡 Feedback | LEDs / display |
| 🚨 Alert | Alert output used by the prototype |
| 🧪 Simulation / Development | Wokwi where applicable |

---

# 📸 15. Prototype Gallery

## 🖥️ Smart Library Interaction

<p align="center">
  <img src="photos_circuit/default_OLED.png" alt="Default Interface" width="700">
</p>

The default interface provides the starting point for the user's interaction with the system.

---

## 📡 RFID Scanner

<p align="center">
  <img src="photos_circuit/RFID_scanner.png" alt="RFID Scanner" width="700">
</p>

The RFID reader provides physical identification input to the system.

---

## 📖 Issue Workflow

<p align="center">
  <img src="photos_circuit/issue_book_demo.png" alt="Issue Book" width="700">
</p>

The interface guides the user through the issue operation.

<p align="center">
  <img src="photos_circuit/issue_success.png" alt="Issue Success" width="700">
</p>

---

## ↩️ Return Workflow

<p align="center">
  <img src="photos_circuit/return_book_demo.png" alt="Return Book" width="700">
</p>

<p align="center">
  <img src="photos_circuit/return_success.png" alt="Return Success" width="700">
</p>

---

# 📂 16. Repository Structure

```text
Smart-Library-Demo/
│
├── 📂 src/
│   └── main.cpp
│
├── 📂 photos_circuit/
│   ├── FindBook_demo.png
│   ├── after_selection_of_book.png
│   ├── RFID_scanner.png
│   ├── indication_of_book.png
│   ├── issue_book_demo.png
│   ├── issue_success.png
│   ├── return_book_demo.png
│   ├── return_success.png
│   ├── anti_theft_alarm.png
│   ├── circuit.png
│   └── default_OLED.png
│
├── 📄 platformio.ini
├── 📄 diagram.json
├── 📄 wokwi.toml
├── 📄 libraries.txt
├── 📄 smart-library.code-workspace
├── 📄 .gitignore
└── 📄 README.md
```

---

# 🚀 17. Getting Started

### Clone

```bash
git clone https://github.com/hemachandramenni07/Smart-Library-Demo.git
```

### Open

Open the project folder in **Visual Studio Code**.

### Install

Install the **PlatformIO IDE** extension.

### Build

```bash
pio run
```

### Upload

```bash
pio run --target upload
```

> Check `platformio.ini` for the board and environment configuration used by the current prototype.

---

# 👥 Team & Contributions

This project was developed collaboratively by our team.  
Rather than dividing the project into fixed individual roles, **all team members contributed across the different stages of the project**, including ideation, hardware, software, testing, debugging, documentation and final integration.

## 🤝 Equal Contribution

```text
                    📚 SMART LIBRARY
                          │
             ┌────────────┼────────────┐
             │            │            │
             ▼            ▼            ▼
          💡 IDEATION   🔧 HARDWARE   💻 SOFTWARE
             │            │            │
             └────────────┼────────────┘
                          │
                          ▼
                    🧪 TESTING
                          │
                          ▼
                    🐛 DEBUGGING
                          │
                          ▼
                    📖 DOCUMENTATION
                          │
                          ▼
                    🚀 FINAL DEMO
```

Every member participated in multiple stages of the development process, and the final prototype represents the **combined work and ideas of the entire team**.

## 👨‍👩‍👧‍👦 Our Team

| Team Member |
|:---:|
| **HemaChandra** |
| **Yasaswini** |
| **Nisha** |
| **Srujitha** |

## 🛠️ Areas of Collective Contribution

| Area | Team Contribution |
|---|---|
| 💡 **Ideation** | Developing and refining the Smart Library concept |
| 🧩 **System Design** | Planning the ESP32-S3-centric architecture |
| 🔧 **Hardware** | Circuit design, component integration and testing |
| 💻 **Software** | Firmware development and system logic |
| 📡 **RFID** | Identification and transaction workflow |
| 🖥️ **User Interface** | Designing and testing the interaction flow |
| 🧪 **Testing** | Testing the complete prototype and individual functions |
| 🐛 **Debugging** | Identifying and resolving hardware/software issues |
| 📖 **Documentation** | README, diagrams, project documentation and presentation |
| 🚀 **Integration** | Combining all modules into the final working prototype |

> **🤝 Built together, tested together, and demonstrated together.**

## 🌟 Team Philosophy

> **No single member owns a single module — the project is a collective effort.**

Each team member contributed ideas, implementation, testing and problem-solving throughout the development of the prototype.


# 🏆 20. Project Status

## ✅ Completed Prototype Demo

This repository represents a **completed and demonstrated Smart Library prototype** using **one ESP32-S3 board as the central controller**.

### Demonstrated

```text
🟢 ESP32-S3 Core
       ↓
🟢 Interactive Interface
       ↓
🟢 Find Book
       ↓
🟢 RFID Identification
       ↓
🟢 Issue Book
       ↓
🟢 Return Book
       ↓
🟢 Visual Feedback
       ↓
🟢 Security-Oriented Demo
```

> ### ⚡ One Board. One Core. One Smart Library.

---

# 🔮 21. Future Possibilities

The prototype is complete, but the concept can be extended.

Possible future directions include:

- 🎙️ Voice-assisted book search
- 🤖 AI-assisted natural-language search
- 🗺️ Interactive library map
- 📱 Mobile companion application
- 📊 Inventory analytics
- 🔐 More advanced RFID security
- 🧠 Intelligent book recommendations
- 🌐 Optional network-based library integration
- ⚡ Further hardware simplification

These are **future possibilities**, not features claimed as part of the current completed demo.

---

# 📚 22. Inspiration & Original Implementation

The project was inspired by a reference smart-library concept that addresses book discovery, RFID-based transactions, shelf guidance and security. fileciteturn1file0L15-L26

The reference material describes a broader architecture involving multiple embedded platforms. fileciteturn1file0L5-L12

Our project takes a different implementation direction:

```text
REFERENCE PROBLEM
       ↓
OUR OWN THINKING
       ↓
SINGLE ESP32-S3
       ↓
PROTOTYPE DESIGN
       ↓
HARDWARE + SOFTWARE
       ↓
TESTING
       ↓
COMPLETED DEMO
```

The goal is to show how the **same problem space can be approached from a different engineering perspective**.

---

# 🎬 23. Demonstration

Add your final demonstration video here:

```text
▶️ Demo Video: [Add YouTube / Drive / Demo Link]
```

A short demonstration should ideally show:

```text
1. Start the system
        ↓
2. Find a book
        ↓
3. Show location / indication
        ↓
4. Scan / identify book
        ↓
5. Issue book
        ↓
6. Return book
        ↓
7. Demonstrate security feedback
```

---

<p align="center">

## 📚 Find it. Identify it. Issue it. Return it.

### ⚡ Powered by one ESP32-S3.

**Inspired by an idea. Reimagined by us. Built as a working prototype.**

</p>

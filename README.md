# 📚 Smart Library — ESP32-S3 Based Smart Library Prototype

<p align="center">
  <img src="photos_circuit/FindBook_demo.png" alt="Smart Library Prototype" width="760">
</p>

<h3 align="center">🔎 Find • 📖 Issue • ↩️ Return • 🔐 Identify • 💡 Guide</h3>

<p align="center">
  A completed Smart Library prototype built around <b>one ESP32-S3 board</b>.
</p>

<p align="center">
  <img src="https://img.shields.io/badge/ESP32--S3-Core-0F766E?style=for-the-badge&logo=espressif">
  <img src="https://img.shields.io/badge/PlatformIO-Development-F58220?style=for-the-badge&logo=platformio">
  <img src="https://img.shields.io/badge/C%2FC%2B%2B-Firmware-00599C?style=for-the-badge&logo=cplusplus">
  <img src="https://img.shields.io/badge/Prototype-Completed-16A34A?style=for-the-badge">
</p>

---

## 🌟 About the Project

Finding a book in a large library can be surprisingly difficult.

A digital catalogue may tell us that a book is **available**, but it does not always answer the question a student actually has:

> **“Where exactly is the book?”**

This project explores that problem through a compact embedded-system prototype.

Our team was **inspired by the smart-library concept presented in a reference article/document**, but we did not reproduce its architecture. We took the underlying problem and re-thought the implementation from our own perspective.

### 💡 Our central idea

Instead of building the prototype around multiple controller boards, we explored a **single ESP32-S3 based approach** where the board acts as the central platform for the user interface, interaction logic and connected library functions.

```text
                    📚 SMART LIBRARY
                          │
                          ▼
                  ┌───────────────┐
                  │   ESP32-S3    │
                  │ Central Board │
                  └───────┬───────┘
                          │
          ┌───────────────┼───────────────┐
          │               │               │
          ▼               ▼               ▼
      🔎 FIND          📖 ISSUE         ↩️ RETURN
      BOOK             BOOK             BOOK
          │               │               │
          └───────────────┼───────────────┘
                          ▼
                  📡 IDENTIFICATION
                          │
                          ▼
                  💡 VISUAL FEEDBACK
```

The goal is simple:

> **Make the physical library experience more interactive without making the hardware architecture unnecessarily complicated.**

---

# 💭 From Inspiration to Our Implementation

The reference smart-library concept describes a broader system for book search, RFID-based transactions, shelf guidance and security. fileciteturn1file0L5-L12

We used that **problem space as inspiration**, then asked a different engineering question:

### “How can we build our own working library prototype around a single ESP32-S3?”

That led us to focus on:

- A central ESP32-S3 controller
- An interactive user interface
- Book identification
- Find-book interaction
- Issue and return workflows
- Visual status indication
- A compact physical demonstration

The reference concept itself discusses the difficulty of locating books in large racks and the need for better inventory and transaction workflows. fileciteturn1file0L15-L26

---

# 🎯 The Problem

Imagine a student enters a library.

They know:

```text
📚 Book: Mathematics
✅ Availability: Available
```

But they still have to search through:

```text
Rack A
 ├── Row 1
 ├── Row 2
 ├── Row 3
 └── Row 4

Rack B
 ├── Row 1
 ├── Row 2
 ├── Row 3
 └── Row 4
```

That creates unnecessary searching.

Our prototype explores a more interactive flow:

```text
             USER
               │
               ▼
        🔎 Search / Select
               │
               ▼
        📚 Identify Book
               │
               ▼
       📍 Locate / Indicate
               │
               ▼
        📖 Issue / Return
               │
               ▼
         ✅ Confirmation
```

---

# 🧠 System Concept

The ESP32-S3 is the **central board** of the prototype.

It brings the interaction into one embedded platform rather than distributing the core experience across several independent controllers.

### Core responsibilities

| Function | What the prototype demonstrates |
|---|---|
| 🖥️ User Interface | Interactive library kiosk experience |
| 🔎 Find Book | Search and book-location interaction |
| 📡 Identification | RFID-based book identification |
| 📖 Issue Book | Book issuing workflow |
| ↩️ Return Book | Book return workflow |
| 💡 Feedback | Visual status and user feedback |
| 🔐 Security Concept | Identification-based security workflow |

---

# 🖥️ User Experience

The interface is designed around a simple principle:

> **The user should not need to understand the electronics to use the library.**

The interaction follows a straightforward flow:

```text
┌──────────────┐
│  HOME SCREEN │
└──────┬───────┘
       │
   ┌───┼───────────────┐
   │   │               │
   ▼   ▼               ▼
 FIND ISSUE          RETURN
 BOOK BOOK            BOOK
   │   │               │
   ▼   ▼               ▼
 BOOK IDENTIFICATION / RFID
           │
           ▼
      SYSTEM RESPONSE
           │
      ┌────┴────┐
      ▼         ▼
     ✅         ⚠️
 SUCCESS       ALERT
```

---

# 🔎 Find Book

The **Find Book** functionality is designed around the most common library problem: knowing that a book exists but not immediately knowing where to find it.

The interaction can be represented as:

```text
User selects / searches for a book
                ↓
       System identifies book
                ↓
       Book information shown
                ↓
       Location / indication
                ↓
          📚 Find the book
```

The prototype's visual interface and hardware demonstration are included below.

### 📸 Find Book — User Interaction

<p align="center">
  <img src="photos_circuit/FindBook_demo.png" alt="Find Book Demo" width="680">
</p>

The screen provides the user with an interface for beginning the book-search process.

### 📸 Book Selection / Location Indication

<p align="center">
  <img src="photos_circuit/after_selection_of_book.png" alt="After Book Selection" width="680">
</p>

After selecting a book, the system moves from the search stage toward identifying and indicating the required book.

---

# 📡 RFID / Book Identification

RFID provides a way to identify a physical book electronically.

The basic idea is:

```text
📚 Physical Book
      │
      ▼
🏷️ RFID Tag
      │
      ▼
📡 RFID Reader
      │
      ▼
🧠 ESP32-S3
      │
      ▼
System processes identification
```

### 📸 RFID Reader

<p align="center">
  <img src="photos_circuit/RFID_scanner.png" alt="RFID Scanner" width="680">
</p>

The RFID interface is used as the physical identification point for the prototype.

### 📸 Identification / Visual Indication

<p align="center">
  <img src="photos_circuit/indication_of_book.png" alt="Book Indication" width="680">
</p>

The prototype combines electronic identification with visible feedback so that the user can understand what the system is doing.

---

# 📖 Issue Book

The issue flow is designed to make a library transaction easy to understand.

```text
          📖 ISSUE BOOK
                │
                ▼
          Scan / Identify
                │
                ▼
        Validate the book
                │
                ▼
          Confirm action
                │
                ▼
        ┌───────┴───────┐
        ▼               ▼
       ✅               ❌
    ISSUED            FAILED
```

### 📸 Issue Book Screen

<p align="center">
  <img src="photos_circuit/issue_book_demo.png" alt="Issue Book Demo" width="680">
</p>

The interface guides the user to scan the book and continue through the issue process.

### 📸 Successful Issue

<p align="center">
  <img src="photos_circuit/issue_success.png" alt="Book Issued Successfully" width="680">
</p>

A dedicated success state gives immediate feedback after a successful issue operation.

---

# ↩️ Return Book

The return process follows a similar interaction pattern.

```text
        📚 RETURN BOOK
               │
               ▼
        Scan / Identify
               │
               ▼
       Validate the book
               │
               ▼
        Update the state
               │
               ▼
        ✅ Return Success
```

### 📸 Return Book Screen

<p align="center">
  <img src="photos_circuit/return_book_demo.png" alt="Return Book Demo" width="680">
</p>

The return interface provides a dedicated workflow instead of mixing return operations with the search interface.

### 📸 Successful Return

<p align="center">
  <img src="photos_circuit/return_success.png" alt="Return Success" width="680">
</p>

The confirmation screen clearly communicates that the operation has completed.

---

# 💡 Visual Feedback

A smart embedded system should not only process an event internally — it should also communicate the result to the user.

The prototype therefore uses visual feedback as part of the interaction.

```text
             SYSTEM EVENT
                  │
        ┌─────────┴─────────┐
        ▼                   ▼
      SUCCESS              ALERT
        │                   │
        ▼                   ▼
      🟢 LED              🔴 LED
        │                   │
        └─────────┬─────────┘
                  ▼
             USER FEEDBACK
```

### 📸 Hardware / Circuit Demonstration

<p align="center">
  <img src="photos_circuit/circuit.png" alt="Smart Library Circuit" width="760">
</p>

The circuit demonstration shows the physical electronics used to create the prototype interaction.

---

# 🔐 Security-Oriented Concept

A library system must also consider what happens when a book moves through a controlled area.

The identification-based security concept follows:

```text
Book detected
      │
      ▼
Read identification
      │
      ▼
Check expected state
      │
 ┌────┴────┐
 ▼         ▼
Valid     Invalid
 │           │
 ▼           ▼
🟢 Normal   🔴 Alert
```

### 📸 Security Demonstration

<p align="center">
  <img src="photos_circuit/anti_theft_alarm.png" alt="Anti Theft Alarm" width="680">
</p>

The prototype demonstrates the use of visible alert feedback when a security-related condition is detected.

---

# 🧩 Hardware Architecture

The central design decision of this project is:

## ⚡ One ESP32-S3 board as the core

Instead of making separate microcontrollers responsible for different sections of the prototype, our implementation is organized around one ESP32-S3.

```text
                    ┌──────────────────────┐
                    │      ESP32-S3        │
                    │                      │
                    │  🖥️ UI              │
                    │  🧠 Control Logic    │
                    │  📡 Connectivity     │
                    │  📚 Library Flow     │
                    └──────────┬───────────┘
                               │
          ┌────────────────────┼────────────────────┐
          │                    │                    │
          ▼                    ▼                    ▼
     📡 RFID               💡 LEDs              🔊 Alert
   Identification       Visual Feedback       Feedback
```

This gives the prototype a compact and easier-to-understand central architecture.

---

# 🔧 Main Technologies

| Category | Technology |
|---|---|
| 🧠 Main Controller | **ESP32-S3** |
| 💻 IDE | **Visual Studio Code** |
| ⚙️ Build System | **PlatformIO** |
| 💻 Programming | **C/C++** |
| 📡 Identification | **MFRC522 RFID** |
| 💡 Visual Feedback | Addressable LED / display feedback |
| 🖥️ User Interaction | ESP32-S3 display/touch interface |
| 🧪 Simulation / Development | Wokwi where applicable |
| 🌐 Communication | ESP32-S3 wireless capabilities where required |

---

# 🔌 Prototype Hardware

The exact hardware configuration can evolve with the prototype, but the central board is:

### 🧠 ESP32-S3

The ESP32-S3 is responsible for coordinating the main interaction and connected peripherals.

Other prototype components include interfaces such as:

- MFRC522 RFID reader
- Addressable LEDs
- Display/touch interface
- Buzzer / alert output
- Power and supporting electronic components

The reference material describes RFID readers, addressable LEDs and other supporting components as part of the broader smart-library problem space. fileciteturn1file0L27-L40

---

# 📂 Repository Structure

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
│   └── ...
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

# 🚀 Getting Started

## 1. Clone the repository

```bash
git clone https://github.com/hemachandramenni07/Smart-Library-Demo.git
```

## 2. Open the project

Open the cloned folder in:

**Visual Studio Code**

Install the **PlatformIO IDE** extension if it is not already installed.

## 3. Connect the ESP32-S3

Connect the ESP32-S3 to your computer through USB.

## 4. Build

From PlatformIO:

```text
Build
```

or:

```bash
pio run
```

## 5. Upload

Use:

```text
Upload
```

or:

```bash
pio run --target upload
```

> Check `platformio.ini` for the exact board/environment configuration used by the current source.

---

# 👥 Team Collaboration

This project is maintained as a team repository.

The development model is:

```text
                         ┌───────────┐
                         │   main    │
                         └─────┬─────┘
                               │
              ┌────────────────┼────────────────┐
              ▼                ▼                ▼
       feature/esp32     feature/rfid     feature/hardware
              │                │                │
              └────────────────┼────────────────┘
                               ▼
                        Pull Request
                               │
                               ▼
                             main
```

### Recommended workflow

```bash
git checkout main
git pull origin main
git checkout -b feature/your-feature
```

After making changes:

```bash
git add .
git commit -m "Describe your changes"
git push -u origin feature/your-feature
```

Then create a Pull Request on GitHub.

---

# 👨‍👩‍👧‍👦 Team

| Member | Area |
|---|---|
| **Hema** | ESP32-S3 integration & project coordination |
| **Nisha** | RFID / identification |
| **Srujitha** | Hardware & visual feedback |
| **Yasaswini** | Integration / documentation |

---

# 🏆 Project Status

## ✅ Completed Prototype Demo

This repository represents a **completed and demonstrated Smart Library prototype** built around **one ESP32-S3 board**.

The prototype has been physically assembled and demonstrated as a proof of concept.

### Demonstrated areas

| Area | Status |
|---|:---:|
| 🧠 ESP32-S3 Core | ✅ |
| 🖥️ Interactive Interface | ✅ |
| 🔎 Find Book Flow | ✅ |
| 📡 RFID / Identification | ✅ |
| 📖 Issue Book Flow | ✅ |
| ↩️ Return Book Flow | ✅ |
| 💡 Visual Feedback | ✅ |
| 🔐 Security-Oriented Demo | ✅ |
| 🧪 Prototype Demonstration | ✅ |

> **⚡ One Board. One Core. One Smart Library.**

---

# 🔮 Future Possibilities

Although the prototype is complete, the idea can continue to evolve.

Possible future improvements include:

- 🎙️ Voice-assisted book search
- 🤖 Natural-language / AI-assisted search
- 🗺️ Interactive library map
- 📱 Mobile companion interface
- 📊 Usage and inventory analytics
- 🔐 More advanced RFID security
- 🧠 Intelligent book recommendations
- 🌐 Optional network-based library integration
- ⚡ Further hardware simplification

These are **future possibilities**, not claims about the current prototype.

---

# 📚 Inspiration & Original Work

This project was inspired by the smart-library problem described in a reference article/document. The reference discusses book-location difficulties and a broader smart-library workflow involving search, issuing, returning and security. fileciteturn1file0L15-L26

Our team used that idea as a starting point and developed a different implementation direction:

```text
REFERENCE PROBLEM
       ↓
   OUR IDEATION
       ↓
 ESP32-S3-CENTRIC DESIGN
       ↓
 HARDWARE + SOFTWARE
       ↓
 COMPLETED PROTOTYPE
```

The purpose of this repository is to document **our implementation, experiments and engineering decisions**, rather than reproduce the reference system.

---

# 🎬 Project Demonstration

Add your final demonstration video here when you publish it:

```text
▶️ Demo Video: [Add YouTube Link]
```

---

# ⭐ Project Highlights

<p align="center">

| 🔎 Smart Search | 📡 RFID | 📖 Transactions | ⚡ ESP32-S3 |
|:---:|:---:|:---:|:---:|
| Find books | Identify books | Issue / Return | Single-board core |

</p>

---

<p align="center">

## 📚 Making the library easier to navigate.

### ⚡ Built around one ESP32-S3.

**Designed • Built • Tested • Demonstrated**

</p>

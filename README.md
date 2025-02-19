# **NIST STS React & C++ Integration (REST API)**
🚀 **A project integrating a React frontend with a C++ backend server for statistical randomness testing using NIST STS (Statistical Test Suite).**  

---

## 📌 **Project Overview**
This project provides a **web-based interface (React + Vite)** to interact with a **C++ backend server** that runs the **NIST Statistical Test Suite (STS)**. The system allows users to analyze randomness in data using various statistical tests.

### **Key Features**
- **Frontend (React + Vite)**
  - Modern UI for user interaction  
  - Live test results display  
  - Optimized for speed and performance  

- **Backend (C++ Server)**
  - Implements **NIST STS** randomness tests  
  - Handles incoming REST API requests from the frontend  
  - High-performance computation using C++  

- **Supported NIST Tests**
  - Frequency Test  
  - Block Frequency Test  
  - Runs Test  
  - Serial Test  
  - Universal Test  
  - Longest Run of Ones Test  
  - Random Excursions Test  
  - Approximate Entropy Test  
  - Cumulative Sums Test  
  - Rank Test  
  - Overlapping Templates Test  
  - Non-Overlapping Templates Test  
  - Linear Complexity Test  
  - Spectral DFT Test  
  - Differential FFT (DFFT) Test  

---
![1 (3)](https://github.com/user-attachments/assets/b0fe088d-0a74-4376-9563-853b8fc9b6d2)
![1 (2)](https://github.com/user-attachments/assets/d8d75696-9392-491d-be0f-37b3a4009593)
![1 (1)](https://github.com/user-attachments/assets/33212eaf-ce36-4d44-a17b-f83050efac32)

## 🛠 **Technology Stack**
| Component      | Technology Used |
|---------------|----------------|
| **Frontend**  | React, Vite, Material UI |
| **Backend**   | C++ (g++ with C++17 support) |
| **API**       | REST API built using C++ |
| **Randomness Testing** | NIST STS Suite |

---

## 📌 **Use Case**
This project is useful for:  
✅ **Cryptographic Analysis** - Validating randomness in cryptographic applications  
✅ **Statistical Analysis** - Checking the randomness of generated data  
✅ **Security Testing** - Assessing the unpredictability of encryption keys  
✅ **Research & Development** - Experimenting with randomness evaluation techniques  

---

## 🚀 **How to Run the Project**  



### **How to Run**
Frontend (React + Vite Setup)
📌 Requirements
Node.js (Latest LTS version recommended)
npm or yarn
VS Code (Recommended)

Run the development server:
npx vite --host 192.168.18.30

Open your browser and go to:
http://192.168.18.30:5173

Backend (C++ Server Setup)
📌 Requirements
GCC Compiler (g++ with C++17 support)
Windows: MinGW or MSVC Compiler
Linux: Build Essentials (sudo apt install build-essential)
Required Libraries: ws2_32 (Windows Sockets API)
📌 Steps to Run
Open Command Prompt (CMD)
Navigate to the backend server directory:
cd "NIST_STS_React&C++_integration_RestAPI/Server"

Compile the C++ server:
g++ -std=c++17 -Iinclude server.cpp NIST/frequency_test.cpp NIST/block_frequency_test.cpp NIST/runs_test.cpp NIST/serial_test.cpp NIST/universal_test.cpp NIST/longestRunOfOnes_test.cpp NIST/randomexcursions_test.cpp NIST/randomexcursions_variant_test.cpp NIST/overlappingtemplates_test.cpp NIST/approximateentropy_test.cpp NIST/cumulativesums_test.cpp NIST/rank_test.cpp NIST/nonoverlappingtemplates_test.cpp NIST/linearcomplexity_test.cpp NIST/spectralDFT_test.cpp NIST/dfft.cpp NIST/IGAMC.cpp NIST/erf.cpp NIST/matrix.cpp NIST/nist_handler.cpp -o server -lws2_32

Run the C++ server:
./server



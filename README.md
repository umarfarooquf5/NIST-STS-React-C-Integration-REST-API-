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

### **1️⃣ Clone the Repository**  
```bash
git clone https://github.com/yourusername/NIST_STS_React_Cpp.git
cd NIST_STS_React_Cpp


Frontend (React + Vite Setup)
📌 Requirements
Node.js (Latest LTS version recommended)
npm or yarn
VS Code (Recommended)
📌 Steps to Run
Navigate to the frontend directory:
bash
Copy
Edit
cd frontend
Install dependencies:
bash
Copy
Edit
npm install
Run the development server:
bash
Copy
Edit
npx vite --host 192.168.18.30
Open your browser and go to:
cpp
Copy
Edit
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
bash
Copy
Edit
cd "NIST_STS_React&C++_integration_RestAPI/Server"
Compile the C++ server:
bash
Copy
Edit
g++ -std=c++17 -Iinclude server.cpp NIST/frequency_test.cpp NIST/block_frequency_test.cpp NIST/runs_test.cpp NIST/serial_test.cpp NIST/universal_test.cpp NIST/longestRunOfOnes_test.cpp NIST/randomexcursions_test.cpp NIST/randomexcursions_variant_test.cpp NIST/overlappingtemplates_test.cpp NIST/approximateentropy_test.cpp NIST/cumulativesums_test.cpp NIST/rank_test.cpp NIST/nonoverlappingtemplates_test.cpp NIST/linearcomplexity_test.cpp NIST/spectralDFT_test.cpp NIST/dfft.cpp NIST/IGAMC.cpp NIST/erf.cpp NIST/matrix.cpp NIST/nist_handler.cpp -o server -lws2_32
Run the C++ server:
bash
Copy
Edit
./server
📌 Project Structure
php
Copy
Edit
NIST_STS_React_Cpp/
│── frontend/             # React Frontend with Vite
│   ├── src/              # React Source Code
│   ├── public/           # Static Assets
│   ├── package.json      # Dependencies
│   ├── vite.config.js    # Vite Configuration
│── backend/              # C++ Backend Server
│   ├── Server/           # Main Backend Code
│   ├── NIST/             # NIST Statistical Tests
│   ├── include/          # Header Files
│   ├── server.cpp        # Main Server Code
│── README.md             # Project Documentation
│── .gitignore            # Git Ignore File
🎯 Troubleshooting
Frontend not running?

Ensure Node.js is installed (node -v to check).
Run npm install again if there are missing dependencies.
Backend compilation errors?

Ensure g++ supports C++17 (g++ --version).
On Windows, install MinGW and add it to the system path.
If -lws2_32 fails, ensure you are on Windows.
📌 Contributing

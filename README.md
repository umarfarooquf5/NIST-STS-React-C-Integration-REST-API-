🚀 Setting Up & Running the Project
1️⃣ Clone the Repository
bash
Copy
Edit
git clone https://github.com/yourusername/NIST_STS_React_Cpp.git
cd NIST_STS_React_Cpp
Frontend (React + Vite Setup)
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
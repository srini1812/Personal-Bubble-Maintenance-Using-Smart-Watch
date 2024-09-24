# Personal-Bubble-Maintenance-Using-Smart-Watch
This code represents an embedded system project that involves Bluetooth Low Energy (BLE) communication, Firebase integration, and Wi-Fi connectivity, alongside components like temperature sensing, an LCD display, and a vibration motor for feedback. Here's a detailed breakdown of how it works:

### Key Components:
1. **BLE (Bluetooth Low Energy):** 
   - Two main modes are implemented:
     - **Broadcaster:** The device acts as a BLE server, broadcasting data via a defined service and characteristic.
     - **Observer:** Another device scans for BLE signals and reads the RSSI (Received Signal Strength Indicator), determining proximity.
   - UUIDs for service and characteristic are specified for communication.

2. **Wi-Fi:**
   - The system connects to a Wi-Fi network using the provided SSID and password (stored in macros `WIFI_SSID` and `WIFI_PASSWORD`).
   - This Wi-Fi connection allows communication with the Firebase real-time database.

3. **Firebase Integration:**
   - The project interacts with a Firebase database hosted at a defined URL. Authentication is handled using an authentication token (`FIREBASE_AUTH`).
   - The system reads and writes temperature and RSSI data to/from Firebase:
     - The device broadcasts temperature (`/TEMP2`) and reads temperature from another device (`/TEMP1`).
     - RSSI values are also sent to Firebase.

4. **LCD Display:**
   - The project utilizes an I2C Liquid Crystal Display (`LiquidCrystal_I2C`) to show messages and sensor values. This provides a real-time visual feedback mechanism.
   - Messages are displayed, such as connection status, BLE data, temperature readings, and safe distance alerts.

5. **Temperature Sensing:**
   - A temperature sensor connected to `tempPin` (analog input) is read and converted into voltage (millivolts). This is then converted into a temperature reading (in Fahrenheit), which is displayed on the LCD and sent to Firebase.

6. **Vibration Motor and Buttons:**
   - A vibration motor is activated based on proximity (determined by RSSI values), alerting when a "safe distance" condition is met.
   - Two buttons (accept and reject) are used to send a response (`Accepted` or `Rejected`) back to Firebase.

### Code Breakdown:

#### 1. **Setup Functions**
   - **`setup()`**:
     - Initializes the BLE broadcaster, Wi-Fi connection, Firebase integration, LCD, and sets up GPIO pins (for buttons and motor).
     - Interrupts are set up for buttons (`acpt`, `rjct`), and BLE broadcasting begins.

#### 2. **Loop Logic**
   - The `loop()` function continuously:
     1. Reads temperature data from a sensor and sends it to Firebase.
     2. Checks RSSI values via BLE and sends them to Firebase.
     3. Reads temperature and status from Firebase for another device.
     4. Executes functions based on proximity (safe distance alert) and button inputs (accept or reject).
     5. Updates Firebase with status (`Accepted` or `Rejected`) depending on user input.

#### 3. **BLE Modes**
   - **`set_as_broadcaster()`**: This function sets the device in broadcaster mode. It creates a BLE server with a specified UUID and characteristic, broadcasting values that can be read by other devices.
   - **`set_as_observer()`**: This function scans for BLE signals and retrieves RSSI values, determining proximity to other devices broadcasting over BLE.

#### 4. **Helper Functions**
   - **`safedistance()`**: Displays "Safe distance alert" on the LCD and activates the vibration motor if the RSSI value is within a threshold (below -58 dBm).
   - **`donothing()`**: Clears the LCD and stops the motor if no close device is detected.
   - **`accepts()` and `rejects()`**: Interrupt service routines triggered by button presses to mark an action as "Accepted" or "Rejected" in Firebase.
   - **`readtemp()`**: Reads the analog temperature sensor and calculates the temperature in Fahrenheit.

### Firebase Structure:
- **`/TEMP1` & `/TEMP2`:** Store temperature values from two devices.
- **`/RSSI`:** Stores the RSSI value, indicating the proximity of two devices.
- **`/Stat1` & `/Stat2`:** Stores the state of the devices (1: active, 0: inactive).
- **`/ans1` & `/ans2`:** Stores the status of acceptance or rejection.

### Potential Use Case:
This project could be used in an IoT-based health monitoring system, where temperature data is broadcasted, proximity is monitored, and decisions are made (accepted/rejected) based on alerts. For example:
- Device 1 monitors temperature and proximity to Device 2.
- If Device 1 detects close proximity (via RSSI), it triggers an alert and sends data to Firebase.
- A user can accept/reject alerts via buttons, which updates Firebase for further processing.

#### Next Steps for GitHub Repository:
- **Structure**: 
   - Organize files into clear folders for easier navigation (e.g., `src` for main code, `lib` for libraries).
   - Add a `README.md` file explaining the setup and purpose of the project, hardware requirements, and step-by-step instructions for usage.
   - Include code for Firebase security rules (for protecting the database) and the BLE UUID setup.

- **Documentation**:
   - Include comments for each function and major code sections to improve readability and make it easier for contributors to understand the flow.

# Project 5 — Greenhouse Climate Controller

**Student:** Souad Mostafa Kamel  
**Module:** Microcontroller-Based Systems (Plain C)  
**Environment:** C99 Compliant (`gcc -std=c99 -Wall -Wextra`)

---

## 1. Project Overview
An automated environmental monitoring and climate-regulation unit for a precision agricultural greenhouse implemented in standard C99. The controller interfaces with 4 core environmental sensors (ambient temperature, relative humidity, light intensity, and soil moisture) and drives 5 digital actuators (heater, cooler, humidifier, grow lights, and irrigation pump). It maintains a circular rolling buffer of historical telemetry to calculate rolling averages and identify long-term climate stability.

---

## 2. Control Rules & Logic Thresholds
* **Temperature Regulation:**
  * If $\text{Temperature} < 18^\circ\text{C} \implies$ **Heater ON**, **Cooler OFF**.
  * If $\text{Temperature} > 28^\circ\text{C} \implies$ **Cooler ON**, **Heater OFF**.
  * Otherwise $\implies$ Both **IDLE**.
* **Moisture & Lighting Regulation:**
  * If $\text{Humidity} < 50\% \implies$ **Humidifier ON**.
  * If $\text{Light} < 400\text{ Lux} \implies$ **Grow Lights ON**.
  * If $\text{Soil Moisture} < 40\% \implies$ **Irrigation Pump ON**.
* **Circular Rolling Log:** A 10-sample sliding buffer records reading history on every evaluation cycle to calculate genuine historical averages without memory fragmentation.
* **Architecture Compliance:** All routines are marked `static`, stay strictly under the 40-line limit, use `readInt()` for boundary-checked input parsing, and avoid non-standard libraries.

---

## 3. How to Build and Run

Compile with strict compiler flags:
```bash
gcc -std=c99 -Wall -Wextra -o app main.c

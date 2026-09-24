## Deliverable 1

## Planning Phase
I asked Claude to help me brainstorm what type of sensors would be useful for creating an Indoor Plant Growth Monitor with a Rasberry Pi, to help run the computation of the device. We were thinking we wanted to monitor water levels, soil moisture, light, CO2,ph and overall. The idea for this project is to have a plantcare monitor device that can take help care of your plants while you are gone for long periods of time during the day, so the plants don't die. Ideally this brought me to think that this product would work bets for people who have very busy schedules and don't have anyone else to monitor or take care of their plants while they are gone in the business of the schedule.
Claude Gave me a possible sensor setup that could work with this idea.
"
Recommended sensors

┌───────────────┬───────────────────────────────────────┬─────────────┬──────────────────────────────────────────────────────────────┐
│   What to     │          Recommended sensor           │  Interface  │                            Notes                             │
│    monitor    │                                       │             │                                                              │
├───────────────┼───────────────────────────────────────┼─────────────┼──────────────────────────────────────────────────────────────┤
│               │ BH1750 (budget) or TSL2591 / VEML7700 │             │ Measures lux. The TSL2591 handles both dim rooms and direct  │
│ Light         │  (better range)                       │ I2C         │ sun. For a real grow-light setup, an AS7341 spectral sensor  │
│               │                                       │             │ shows the color spectrum too.                                │
├───────────────┼───────────────────────────────────────┼─────────────┼──────────────────────────────────────────────────────────────┤
│               │                                       │             │ Both are true NDIR sensors. The SCD41 also gives temperature │
│ CO₂           │ Sensirion SCD40/SCD41 or MH-Z19C      │ I2C / UART  │  and humidity. Avoid the CCS811 and SGP30: they estimate     │
│               │                                       │             │ "eCO₂" from VOCs and don't measure CO₂.                      │
├───────────────┼───────────────────────────────────────┼─────────────┼──────────────────────────────────────────────────────────────┤
│               │                                       │             │ pH probes are made for water or nutrient solution, not soil. │
│ pH            │ Atlas Scientific EZO-pH (best) or     │ I2C /       │  Left in soil they drift and wear out. Measure the watering  │
│               │ DFRobot Gravity pH (budget)           │ analog→ADC  │ reservoir, or do occasional manual soil checks. Needs        │
│               │                                       │             │ calibration with buffer solutions.                           │
├───────────────┼───────────────────────────────────────┼─────────────┼──────────────────────────────────────────────────────────────┤
│               │ Float switch (simplest), XKC-Y25      │             │ Float switches give low/OK. Ultrasonic or ToF gives a        │
│ Water level   │ non-contact capacitive, or JSN-SR04T  │ GPIO / I2C  │ continuous level reading. The XKC-Y25 sticks to the outside  │
│ (reservoir)   │ waterproof ultrasonic / VL53L0X       │             │ of the tank, so nothing touches the water.                   │
│               │ time-of-flight                        │             │                                                              │
├───────────────┼───────────────────────────────────────┼─────────────┼──────────────────────────────────────────────────────────────┤
│               │ Capacitive soil moisture sensor v2.0  │ Analog→ADC  │ Avoid resistive (two-fork) sensors. They corrode within      │
│ Soil moisture │ or Adafruit STEMMA Soil Sensor        │ / I2C       │ weeks. The STEMMA version is I2C, so it needs no ADC, and it │
│               │                                       │             │  also reads temperature.                                     │
├───────────────┼───────────────────────────────────────┼─────────────┼──────────────────────────────────────────────────────────────┤
│ Air temp &    │ SHT40, BME280, or covered by the      │ I2C         │ Humidity matters a lot for tropical houseplants.             │
│ humidity      │ SCD41                                 │             │                                                              │
├───────────────┼───────────────────────────────────────┼─────────────┼──────────────────────────────────────────────────────────────┤
│ Soil/root     │ DS18B20 waterproof probe              │ 1-Wire      │ Cheap and reliable.                                          │
│ temp          │                                       │             │                                                              │
├───────────────┼───────────────────────────────────────┼─────────────┼──────────────────────────────────────────────────────────────┤
│ Nutrients     │ DFRobot Gravity TDS/EC sensor         │ Analog→ADC  │ Useful if you add fertilizer to the reservoir or grow        │
│ (optional)    │                                       │             │ hydroponically.                                              │
├───────────────┼───────────────────────────────────────┼─────────────┼──────────────────────────────────────────────────────────────┤
│ Plant health  │ Pi Camera Module 3 (or the NoIR       │ CSI         │ Take time-lapse photos, detect wilting or yellowing, or      │
│ (optional)    │ version)                              │             │ compute NDVI with the NoIR camera.                           │
└───────────────┴───────────────────────────────────────┴─────────────┴──────────────────────────────────────────────────────────────┘

Actuators for plant care while you're away

Monitoring alone won't keep plants alive, so the device also needs to act:
- Watering: a peristaltic pump (precise dosing) or a small 5V/12V submersible pump, driven by a relay module or MOSFET. Power the pump from its own supply, not from the Pi.
- Lighting: an LED grow light switched by a relay or smart plug, based on the light sensor plus a schedule.
- Airflow: a small fan, if humidity or CO₂ gets out of range.
- Safety: use the reservoir water-level sensor to stop the pump before it runs dry.

Practical tips

- Keep most sensors on I2C so they share two wires. If two sensors have the same address (for example, several soil sensors for several pots), add a TCA9548A I2C multiplexer.
- Don't put pH and EC probes in the same water while reading both. They interfere with each other. Read them at different times or use an isolated board (Atlas sells one).
- Software stack: Python with the adafruit-circuitpython-* libraries, logging to SQLite, a Flask or Grafana dashboard, and alerts by email, Discord, or a push notification when the water runs low or the soil stays dry.
- Control logic: water when soil moisture drops below a threshold. Add a minimum time between waterings so a bad sensor reading can't flood the pot.

Minimal starter kit (about $60–90)

Pi, ADS1115, 1–2 capacitive soil sensors, BH1750, SCD41 (CO₂ + temp + humidity), a float switch, a relay, a small pump, and tubing. You can add pH, EC and the camera later."
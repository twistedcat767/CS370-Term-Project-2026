# Problem memo -- Team Green (Tyler Orwa, Catalina Alcaraz)

## The user
Indoor plant owners/carers who are gone for several hours or long periods of time during the day.

## The problem
People aren't aware of how they are harming their indoor plants with the multiple factors that come together in taking good care of them. Plants fail when noboy is looking and sometimes people don't have all the time in the world to be taking care of their plants with the proper care. The question is, "What is wrong with my plant, and how can I handle it?"

## Why a device
The nutrients and varying conditions can't be measured by a phone and change over the course of the day which need to eventually be addressed. A phone app could just be a reminder, it can not since light, soil or heat conditons that affect the plant.

## The sensors
Light, CO2, pH, Soil Moisture

## Sensor Decisons
We plan to implement 4 snesnors in hases because some of the sensors are more expensive than orginally expected.
1. Light sensor-- BH1750 or VEML7700, I2C-- ($6-10)
2. Soil Moisture sensor- Capactive probe through an ADS1115 ADC
We want the soil and light sensors to come first as they are cheaper togther they cost abt $15-30 and give us working pipeline
3. CO2 sensor- SCD40 or I2C- we want to use a real NDIR-class sensor and not a VOC0based estimate because the estimate would sway the diagnosis engine to misleading data

4. PH is the stretch sensor, probes are usally expensive, they drift and need constant recalibration, and are often designed for liquids not soil. 

## The mechanisms
**Multi-process architecture**: Each sensor has its own process that a hub process aggregates together and a supervisor can restart/kill each process that faults.

**Storage**: The hub process accumalates readings into a memory buffer that writes in batches to a CSV in append-only mode.

## The risk
Hardware compatability between components and its environment

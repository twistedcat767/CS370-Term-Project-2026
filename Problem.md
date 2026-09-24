# Problem memo -- Team Green (Tyler Orwa, Catalina Alcaraz)

## The user
Indoor plant owners/carers who are gone for several hours or long periods of time during the day.

## The problem
People aren't aware of how they are harming their indoor plants with the multiple factors that come together in taking good care of them. Plants fail when noboy is looking and sometimes people don't have all the time in the world to be taking care of their plants with the proper care. The question is, "What is wrong with my plant, and how can I handle it?"

## Why a device
The nutrients and varying conditions can't be measured by a phone and change over the course of the day which need to eventually be addressed. A phone app could just be a reminder, it can not since light, soil or heat conditons that affect the plant.

## The sensors
Light, CO2, pH, Soil Moisture

## The mechanisms
**Multi-process architecture**: Each sensor has its own process that a hub process aggregates together and a supervisor can restart/kill each process that faults.

**Storage**: The hub process accumalates readings into a memory buffer that writes in batches to a CSV in append-only mode.

## The risk
Hardware compatability between components and its environment

# Problem memo -- Team Green (Tyler Orwa, Catalina Alcaraz)

## The user
Indoor plant owners/carers

## The problem
People aren't aware of how they are harming their indoor plants with the multiple factors that come together in taking good care of them.

## Why a device
The nutrients and varying conditions can't be measured by a phone and change over the course of the day which need to eventually be addressed.

## The sensors
Light, CO2, pH, Soil Moisture

## The mechanisms
**Multi-process architecture**: Each sensor has its own process that a hub process aggregates together and a supervisor can restart/kill each process that faults.

**Storage**: The hub process accumalates readings into a memory buffer that writes in batches to a CSV in append-only mode.

## The risk
Hardware compatability between components and its environment

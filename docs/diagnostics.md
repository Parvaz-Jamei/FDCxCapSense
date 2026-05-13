# Diagnostics

Diagnostics report flags and technical reasons only. They do not return application decisions.

Flags include I2C error, timeout, raw near positive or negative limit, CAPDAC at min or max, baseline not learned, signal unstable, and invalid input.

Application profiles may use these flags to lower confidence, return `Unknown`, or return `Unstable`.

# Automotive driving robot: pedal actuation

A robot that sits where the driver's feet go and works the pedals directly, so a conventional car can be automated without touching its wiring. University of Southampton MEng group design project (FEEG6013); my role was actuator design and closed-loop control. This was a team project, shared with the group's permission.

**Full write-up, with photos, video and what broke: [tgreen-fe.github.io/pedal-actuation.html](https://tgreen-fe.github.io/pedal-actuation.html)** · **[Design journal summary (PDF)](https://tgreen-fe.github.io/assets/reports/pedal-design-journal.pdf)**

[![The robot installed in the footwell of the test car, cycling the pedal during commissioning](https://tgreen-fe.github.io/assets/img/pedal-car-install.gif)](https://tgreen-fe.github.io/pedal-actuation.html)

## Results

The actuator covers full pedal travel in 114 ms on a full-scale step, with no meaningful overshoot.

![Open-loop step response: commanded step against measured pedal position](https://tgreen-fe.github.io/assets/fig/pedal-step-response.svg)

Position feedback comes from two independent sensors, one on the pedal and one on the actuator crank. Across a full stroke they agree with a correlation of 0.9990 and a mean absolute error of 1.8 per cent of travel, which makes the crank signal a usable redundant channel and a disagreement between the two a fault indicator rather than noise.

![Pedal sensor against crank potentiometer through a press, hold and release](https://tgreen-fe.github.io/assets/fig/pedal-sensor-agreement.svg)

## System identification before control

The plant is awkward: a motor driving a linkage against a pedal with its own return spring and a non-linear feel. So the first job was measuring it, not tuning it. The `Sys_Ident` and `Combo_*` sketches run a family of excitation signals through the actuator and log the response: steps at several amplitudes, ramps, sine sweeps, random square waves and pseudo-random binary sequences at a range of dwell times. The PRBS runs are the useful ones for fitting a model, because they excite a broad band of frequencies without driving the actuator beyond what the mechanics can follow.

![Pseudo-random binary excitation and the resulting motion](https://tgreen-fe.github.io/assets/fig/pedal-prbs.svg)

## Repository map

| Directory | Contents |
|---|---|
| `openLoop_v1/` | First open-loop control pass |
| `Closed_Loop_Motor/`, `closedLoop_v2/`, `CLEAN_Closed_Loop_Motor/` | Closed-loop motor control, in iteration order |
| `Closed_Loop_Pedal/` | Closed-loop control against the pedal itself |
| `pedalPosRead/`, `Pedal_and_Pot_Pos_Read/` | Position sensing; includes the crank-correlation dataset (`Crank Correlation.csv`) |
| `Sys_Ident/`, `Combo_Sine/`, `Combo_Rand/` | System-identification test signals and response plots |
| `Callibration_and_Limits/` | Calibration and travel-limit routines |

Firmware is Arduino C++ (`.ino`). The response plots in `Combo_Sine/` are the logged sine-sweep runs at each frequency.

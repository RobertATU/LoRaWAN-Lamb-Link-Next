---
pagetitle: Release Notes for STM32 sequencer
lang: en
---

::: {.row}
::: {.col-sm-12 .col-lg-4}

::: {.card .fluid}
::: {.sectione .dark}
<center>
# <small>Release Notes for</small> STM32 sequencer
Copyright &copy; 2019 STMicroelectronics\
    
[![ST logo](_htmresc/st_logo.png)](https://www.st.com){.logo}
</center>
:::
:::

# Purpose

The source code delivered is an utility to help the task scheduling inside a system without OS.

:::

::: {.col-sm-12 .col-lg-8}
# Update History

::: {.collapse}
<input type="checkbox" id="collapse-section8" checked aria-hidden="true">
<label for="collapse-section8" aria-hidden="true">v1.6.0 / 26-October-2021</label>
<div>

## Main Changes

### Maintenance release

  Headline
  ----------------------------------------------------------
  Ticket 104404 - Some events may be lost due to race conditions (rework)
  Ticket 115234 - [Licensing] Update the way to declare licenses in Cube and X-CUBE components
  Documentation update

## Known Limitations

  Outstanding bugs list : None

  Requirements not met or planned in a forthcoming release : None

## Development Toolchains and Compilers


- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Backward compatibility

  No compatibility break with previous version

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section8" aria-hidden="true">
<label for="collapse-section8" aria-hidden="true">v1.5.2 / 10-June-2021</label>
<div>

## Main Changes

### Maintenance release

  Headline
  ----------------------------------------------------------
  Ticket 104404 - Some events may be lost due to race conditions

## Known Limitations

  Outstanding bugs list : None

  Requirements not met or planned in a forthcoming release : None

## Development Toolchains and Compilers


- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Backward compatibility

  No compatibility break with previous version

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section8" aria-hidden="true">
<label for="collapse-section8" aria-hidden="true">v1.5.1 / 10-Sept-2020</label>
<div>

## Main Changes

### Maintenance release

  Headline
  ----------------------------------------------------------
  Ticket 86948 - CodeSpell : Correct some english words typo

## Known Limitations

  Outstanding bugs list : None

  Requirements not met or planned in a forthcoming release : None

## Development Toolchains and Compilers


- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Backward compatibility

  No compatibility break with previous version

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section7" aria-hidden="true">
<label for="collapse-section7" aria-hidden="true">v1.5.0 / 5-Sept-2020</label>
<div>

## Main Changes

### Maintenance release

  Headline
  ----------------------------------------------------------
  Ticket 91315 - Four lines of code should be under a __weak function to allow overcharge it for unprivileged mode
  Ticket 89624 - Utilities Releases Notes improvements - Link on openorg BSD-3-Clause
  Ticket 86948 - CodeSpell : Correct some english words typo  

## Known Limitations

  Outstanding bugs list : None

  Requirements not met or planned in a forthcoming release : None

## Development Toolchains and Compilers


- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Backward compatibility

  No compatibility break with previous version

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section6" aria-hidden="true">
<label for="collapse-section6" aria-hidden="true">v1.4.0 / 19-Dec-2019</label>
<div>

## Main Changes

### Maintenance release

  Headline
  ----------------------------------------------------------
  Doxygen update
  [78010][UTILITIES][SEQ] update the function UTIL_SEQ_EvtIdle to allow the run of all tasks except the task waiting for an event
  [78143][UTILITIES][SEQ] UTIL_SEQ_RFU definition has been wrongly removed from UTIL_Seq.h
  [77834][UTILITIES][SEQ] Allow call of WaitEvent in the context of PreIdle/PostIdle


## Known Limitations

  Outstanding bugs list : None

  Requirements not met or planned in a forthcoming release : None

## Development Toolchains and Compilers


- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Backward compatibility

  No compatibility break with previous version

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section5"  aria-hidden="false">
<label for="collapse-section5" aria-hidden="true">V1.3.0 / 25-Sept-2019</label>
<div>

## Main Changes

### release V1.3.0

## Contents

  Headline
  --------
  doxygen tag insertion 
  doxygen documentation html\\index.html
  MISRA 2012 test
  Ticket 72443 - Scheduler: CurrentTaskIdx it is not correctly handled.

  : Fixed bugs list
\

## known limitations

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

## backward compatibility

## Dependencies

</div>
:::


::: {.collapse}
<input type="checkbox" id="collapse-section4" aria-hidden="true">
<label for="collapse-section4" aria-hidden="true">V1.2.1 / 19-Sept-2019</label>
<div>

## Main Changes

### release V1.2.1

## Contents

  Headline
  --------
  Ticket 72443 - Scheduler: CurrentTaskIdx it is not correctly handled.

  : Fixed bugs list
\

## known limitations

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

## backward compatibility

## Dependencies

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section3" aria-hidden="true">
<label for="collapse-section3" aria-hidden="true">V1.2.0 / 24-June-2019</label>
<div>

## Main Changes

### release V1.2.0

## Contents

  Headline
  --------
  Fixed to avoid infinite loop when a task is waiting for an event.

  : Fixed bugs list
\

## known limitations

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

## backward compatibility

## Dependencies

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section2" aria-hidden="true">
<label for="collapse-section2" aria-hidden="true">V1.1.0 / 06-June-2019</label>
<div>

## Main Changes

### release V1.1.0

## Contents

  Headline
  --------
  License correction.
  change __weak symbol by __WEAK to be compliant with AC6.

  : Fixed bugs list
\

## known limitations

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

## backward compatibility

## Dependencies

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section1" aria-hidden="true">
<label for="collapse-section1" aria-hidden="true">V1.0.0 / 15-May-2019</label>
<div>

## Main Changes

### first release

## Contents

  Headline
  --------
  first version. 

  : Fixed bugs list
\

## known limitations

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

## backward compatibility

## Dependencies


</div>
:::

:::
:::

<footer class="sticky">
For complete documentation on STM32,visit: [[www.st.com/stm32](http://www.st.com)]

This release note uses up to date web standards and, for this reason, should not be opened with Internet Explorer
but preferably with popular browsers such as Google Chrome, Mozilla Firefox, Opera or Microsoft Edge.
</footer>

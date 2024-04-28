/**
  ******************************************************************************
  * @file    SM_Engine.c
  * @author  SRA Team
  * @version V1.0.0
  * @brief   Engine for SM Module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/** @addtogroup SM_MODULE_Engine State Machine Engine
  * @{
  */
/* Private includes ----------------------------------------------------------*/
#include "SM_Engine.h"

/* Private constants ---------------------------------------------------------*/
/** @defgroup SM_MODULE_Engine_Private_const private constants
  * @brief <i>Private constants</i>
  * @{
  */
/** @defgroup SM_MODULE_Engine_Private_const_version module version
  * @brief <i>Module version</i>
  * @{
  */
#define MOD_FWHEADER_NAME  "Module:SM"                                  /*!< module name        */
#define MOD_FWHEADER_VER   "0.9.2"                                      /*!< module version     */
#define MOD_FWHEADER_DATE  "Jan. 27th, 2022"                            /*!< module date        */
#define MOD_FWHEADER_DESCR MOD_FWHEADER_NAME " v" MOD_FWHEADER_VER      /*!< module description */

/* IDE selection */
#if defined(__GNUC__) /* STM32CubeIDE */
#define MOD_FWHEADER_IDE "STM32CubeIDE (STMicroelectronics)"            /*!< module ide         */
#elif defined (__ICCARM__) /* IAR EWARM */
#define MOD_FWHEADER_IDE "EWARM (IAR Systems)"                          /*!< module ide         */
#undef MOD_FWHEADER_DATE
#define MOD_FWHEADER_DATE  __DATE__                                     /*!< module date        */

/* the __USED definition is not correct */
#undef __USED
#define __USED __root
#elif defined (__CC_ARM) /* Keil uVision */
#define MOD_FWHEADER_IDE "uVision (ARM Keil)"                           /*!< module ide         */
#undef __USED
#define __USED
#else   /* Unknown */
#define MOD_FWHEADER_IDE "Unknown"                                      /*!< module ide         */
#endif  /* IDE selection */

/** @brief firmware header definition
  */
#define MOD_FWHEADER \
      "\r\n[Header]" \
      "\r\nName:"    MOD_FWHEADER_NAME \
      "\r\nVer:"     MOD_FWHEADER_VER \
      "\r\nDate:"    MOD_FWHEADER_DATE \
      "\r\nDescr:"   MOD_FWHEADER_DESCR \
      "\r\nIDE:"     MOD_FWHEADER_IDE \
      "\r\n[/Header]\r\n"
/**
  * @} SM_MODULE_Engine_Private_const_version
  */
/**
  * @} SM_MODULE_Engine_Private_const
  */

/** @defgroup SM_MODULE_Engine_Private_variables private variables
  * @brief <i>Private variables</i>
  * @{
  */

/** @brief module firmware header
  */
__USED const char Module_FWHeader[] = MOD_FWHEADER;

#if SM_USE_NAME
/** @brief definition string for None
  */
const char * SM_NONE_TEXT = "<None>";

/** @brief definition string for Unknown
  */
const char * SM_UNKNOWN_TEXT = "<Unknown>";

/** @brief definition string for Epsilon event
  */
const char * SM_EV_EPSILON_TEXT = "<Epsilon>";

/** @brief definition string for Return event
  */
const char * SM_EV_RETURN_TEXT = "<Return>";

/** @brief definition string for Invalid state
  */
const char * SM_ST_INVALID_TEXT = "<Invalid>";
#endif /* SM_USE_NAME */

/** @brief state machine handle
  */
SM_Handle_t SM_Handle = {
#if SM_USE_NAME
  .Name = "None",
#endif /* SM_USE_NAME */
  .CurrentState         = SM_NONE,
  .PreviousState        = SM_NONE,
  .ReturnState          = SM_NONE,
  .CriticalEvent        = SM_NONE,
  .LastEvent            = SM_NONE,
  .Live = {
    .Tick               = 0,
    .ModuleErrorCount   = 0,
    .Timeout            = SM_NO_TIMEOUT,
  },
  .Handlers = {
    .GetSysTickFunc     = NULL,
#if SM_USE_LOG
    .LogTransitionFunc  = NULL,
#endif /* SM_USE_LOG */
  }
};
/**
  * @} SM_MODULE_Engine_Private_variables
  */

/** @defgroup SM_MODULE_Engine_Private_functions private functions
  * @brief <i>Private functions</i>
  * @{
  */
static void __ErrorHandler(uint8_t param);
static void __HandleReset(void);
static uint32_t __GetSysTick(void);
#if SM_USE_LOG
static void __LogTransition(SM_State_t startState, SM_Event_t event, SM_State_t endState);
#endif /* SM_USE_LOG */

/** @defgroup SM_MODULE_Engine_Private_functions_cycle cycle support
  * * @brief <i>Cycle support</i>
  * @{
  */
static SM_Error_t __Cycle_Events(SM_State_t * pstate, SM_Event_t * pevent);
static SM_Error_t __Cycle_Timeout(SM_State_t * pstate, SM_Event_t * pevent);
static SM_Error_t __Cycle_Modules(void);
/**
  * @} SM_MODULE_Engine_Private_functions_cycle
  */

/**
  * @} SM_MODULE_Engine_Private_functions
  */

/** @addtogroup SM_MODULE_Engine_Private_functions
  * @{
  */

/** @brief handler of error
  * @param param parameter of the error handler
  * @retval None
  */
static void __ErrorHandler(uint8_t param)
{
  /* in case of error read the param and block the execution */
  (void)param;
  while(1);
}

/** @brief handle reset
  * @retval None
  */
static void __HandleReset(void)
{
  SM_StateItem_t const * pstate_item = SM_GetCurrentStateItem();

  SM_Handle.Live.Tick = 0;
  SM_Handle.Live.Timestamp = __GetSysTick();

  /* set the tick timeout according to the new state */
  SM_Handle.Live.Timeout = SM_NO_TIMEOUT;
  if (pstate_item &&
      pstate_item->TimeoutConf.Mode != SM_TO_MODE_NONE &&
      pstate_item->TimeoutConf.Value >= 0)
  {
      SM_Handle.Live.Timeout = SM_Handle.Live.Timestamp + pstate_item->TimeoutConf.Value;
  }
}

/** @brief get sys tick time (internally or externally)
  * @retval tick
  */
static uint32_t __GetSysTick(void)
{
  return SM_Handle.Handlers.GetSysTickFunc ?
          SM_Handle.Handlers.GetSysTickFunc() :
          SM_Handle.Live.Tick;
}

static SM_State_t __LoadFirstState(void)
{
  if (SM_Handle.Handlers.LoadFirstStateFunc)
  {
    return SM_Handle.Handlers.LoadFirstStateFunc();
  }
  else
  {
    return SM_ST_START_DEF;
  }
}

static void __SaveFirstState(SM_State_t s)
{
  if (SM_Handle.Handlers.SaveFirstStateFunc)
  {
    SM_Handle.Handlers.SaveFirstStateFunc(s);
  }
}

static void __Reboot(void)
{
  if (SM_Handle.Handlers.RebootFunc)
  {
    SM_Handle.Handlers.RebootFunc();
  }
}

#if SM_USE_LOG
/** @brief call the log function if defined
  * @param startState start state
  * @param event event
  * @param endState end state
  */
static void __LogTransition(SM_State_t startState, SM_Event_t event, SM_State_t endState)
{
  if (SM_Handle.Handlers.LogTransitionFunc)
  {
    SM_Handle.Handlers.LogTransitionFunc(startState, event, endState);
  }
}
#endif /* SM_USE_LOG */

/**
  * @} SM_MODULE_Engine_Private_functions
  */

/** @addtogroup SM_MODULE_Engine_Private_functions_cycle
  * @{
  */

/** @brief manage events during the cycle
  * @param pstate pointer to state variable (output)
  * @param pevent pointer to event variable (output)
  * @retval error condition
  */
static SM_Error_t __Cycle_Events(SM_State_t * pstate, SM_Event_t * pevent)
{
  /* return vars */
  SM_State_t ret_state = SM_NONE;
  SM_Event_t ret_event = SM_NONE;

  /* condition events vars */
  uint8_t cond_ev_critical = 0;
  uint8_t cond_ev_normal   = 0;

  /* check epsilon event in case of first tick */
  if (SM_Handle.Live.Tick == 0)
  {
    ret_event = SM_EV_EPSILON;
    ret_state = SM_GetNextState(SM_Handle.CurrentState, ret_event);
  }

  /* only in case of not valid state re-init of the vars */
  if (!SM_IsValidState(ret_state))
  {
    ret_state = SM_NONE;
    ret_event = SM_NONE;

    cond_ev_critical = SM_IsValidEvent(SM_Handle.CriticalEvent) || SM_IsSpecialEvent(SM_Handle.CriticalEvent);
    cond_ev_normal   = SM_IsValidEvent(SM_Handle.LastEvent) || SM_IsSpecialEvent(SM_Handle.LastEvent);
  }

  /* if there is an event check the next state */
  if (cond_ev_critical || cond_ev_normal)
  {
    /* first-of-all check the critical event */
    /* in case of no transition the event is leave in the queue */
    ret_event = SM_Handle.CriticalEvent;
    if (cond_ev_critical)
    {
      /* search a valid transition */
      ret_state = SM_GetNextState(SM_Handle.CurrentState, ret_event);

      if (SM_IsValidState(ret_state))
      {
        /* serving the critical event and reset the var */
        SM_Handle.CriticalEvent = SM_NONE;
      }
    }

    /* check the normal event in case of no next state */
    if (cond_ev_normal && !SM_IsValidState(ret_state))
    {
      ret_event = SM_Handle.LastEvent;
      ret_state = SM_GetNextState(SM_Handle.CurrentState, ret_event);
      if (!SM_IsValidState(ret_state)) ret_state = SM_ST_INVALID;
    }
  }

  /* in any case reset the last event */
  SM_Handle.LastEvent = SM_NONE;

  /* set output variables */
  if (pstate) *pstate = ret_state;
  if (pevent) *pevent = ret_event;

  return SM_OK;
}

/** @brief manage time-out during the cycle
  * @param pstate pointer to state variable (output)
  * @param pevent pointer to event variable (output)
  * @retval error condition
  */
static SM_Error_t __Cycle_Timeout(SM_State_t * pstate, SM_Event_t * pevent)
{
  SM_State_t ret_state = SM_NONE;
  SM_Event_t ret_event = SM_NONE;

  /* no transition, evaluate the tick timeout */
  if (SM_Handle.Live.Timeout != SM_NO_TIMEOUT)
  {
    switch (SM_Handle.pCurrentStateItem->TimeoutConf.Mode)
    {
    case SM_TO_MODE_FIXED:
      /* nothing */
      break;
    case SM_TO_MODE_OK_ENTRY:
      /* check only first time */
      if (SM_Handle.Live.ModuleErrorCount == 0)
      {
        SM_Handle.Live.Timeout = SM_NO_TIMEOUT;
      }
      break;
    case SM_TO_MODE_OK_WATCH_DOG:
      /* each time the result is ok reset the counter*/
      if (SM_Handle.Live.ModuleErrorCount == 0)
      {
        /* calculate the next timeout (current timestamp + the state TO */
        SM_Handle.Live.Timeout = SM_Handle.Live.Timestamp + SM_Handle.pCurrentStateItem->TimeoutConf.Value;
      }
      break;
    default:
      SM_Handle.Live.Timeout = SM_NO_TIMEOUT;
      break;
    }

    /* if the TO is expired */
    if (SM_Handle.Live.Timeout != SM_NO_TIMEOUT && SM_Handle.Live.Timestamp >= SM_Handle.Live.Timeout)
    {
      /* get the event from state configuration */
      ret_event = SM_Handle.pCurrentStateItem->TimeoutConf.Event;

      /* try to move to the next state */
      ret_state = SM_GetNextState(SM_Handle.CurrentState, ret_event);

      /* disable the TO */
      SM_Handle.Live.Timeout = SM_NO_TIMEOUT;
    }
  }

  /* set output variables */
  if (pstate) *pstate = ret_state;
  if (pevent) *pevent = ret_event;

  return SM_OK;
}

/** @brief manage modules during the cycle
  * @retval error condition
  */
static SM_Error_t __Cycle_Modules(void)
{
  /* module handler callbacks and check if there are errors */
  SM_Module_t module;
  SM_Error_t module_error;

  SM_Handle.Live.ModuleErrorCount = 0;
  for(module = 0; module < SM_Handle.Config->NumModules; module++)
  {
    SM_ModuleHandler_t func = SM_Handle.Config->Modules[module].Handlers[SM_Handle.CurrentState];
    if (func)
    {
      module_error = func(module, SM_Handle.CurrentState);
      if (module_error < 0) SM_Handle.Live.ModuleErrorCount++;
    }
  }

  return SM_OK;
}
/**
  * @} SM_MODULE_Engine_Private_functions_cycle
  */

/** @addtogroup SM_MODULE_Engine_Exported_functions_main
  * @{
  */

/** @brief state machine initialization
  * @param name name of the state machine
  * @param config pointer to configuration structure
  * @param handlers handlers structure
  */
void SM_Init(const char * name, const SM_Config_t * config, SM_Handlers_t * handlers)
{
  SM_Handle.Config = config;

  if (config->NumStates > SM_ST_MAX_NUM)
  {
    /* Check the max number of states */
    __ErrorHandler(0);
  }

  SM_Stop();
  SM_SetHandlers(handlers);
  SM_SetName(name);

  /* reset the handle */
  __HandleReset();
}

/** @brief set the handlers
  * @param handlers handlers structure pointer
  * @retval error condition
  */
SM_Error_t SM_SetHandlers(SM_Handlers_t * handlers)
{
  if (!handlers) return SM_ERR_INVALID_PARAM;
  memcpy((void *)&SM_Handle.Handlers, (void *)handlers, sizeof(SM_Handlers_t));
  return SM_OK;
}

/** @brief get the handlers
  * @param handlers handlers structure pointer
  * @retval error condition
  */
SM_Error_t SM_GetHandlers(SM_Handlers_t * handlers)
{
  if (!handlers) return SM_ERR_INVALID_PARAM;
  memcpy((void *)handlers, (void *)&SM_Handle.Handlers, sizeof(SM_Handlers_t));
  return SM_OK;
}

#if SM_USE_NAME
/** @brief set the name of the current state machine
  * @param name name of the state machine
  */
void SM_SetName(const char * name)
{
  SM_Handle.Name = name;
}
#endif

/** @brief get cycle status
  * @retval status
  */
SM_CycleStatus_t SM_GetCycleStatus(void)
{
  return SM_Handle.CycleStatus;
}

/** @brief set the cycle status
  * @param cycleStatus set the cycle status
  * @retval error condition
  */
SM_Error_t SM_SetCycleStatus(SM_CycleStatus_t cycleStatus)
{
  SM_Error_t err = SM_OK;
  switch(cycleStatus)
  {
  case SM_CS_STOP:
    SM_Handle.PreviousState = SM_Handle.CurrentState;
    SM_Handle.CurrentState = SM_Handle.Config->StartState;
    SM_Handle.CycleStatus = cycleStatus;
    break;
  case SM_CS_RUN:
    SM_Handle.CycleStatus = cycleStatus;
    break;
  case SM_CS_PAUSE:
    SM_Handle.CycleStatus = cycleStatus;
    break;
  default:
    /* nothing */
    err = SM_ERR_GENERIC;
    break;
  }

  return err;
}

/** @brief check if the state machine is running
  * @retval 1 if is run, 0 otherwise
  */
uint8_t SM_IsRunning(void)
{
  return SM_GetCycleStatus() == SM_CS_RUN;
}

/** @brief start the state machine (cycle is on)
  * @retval error condition
  */
SM_Error_t SM_Start(void)
{
  return SM_SetCycleStatus(SM_CS_RUN);
}

/** @brief stop the state machine (cycle is stopped), the state machine is reset
  * @retval error condition
  */
SM_Error_t SM_Stop(void)
{
  return SM_SetCycleStatus(SM_CS_STOP);
}

/** @brief pause the state machine (cycle is paused), the state machine is not reset
  * @retval error condition
  */
SM_Error_t SM_Pause(void)
{
  return SM_SetCycleStatus(SM_CS_PAUSE);
}

/** @brief get the pointer to live variables
  * @retval live pointer
  */
SM_Live_t * SM_GetLive(void)
{
  return &SM_Handle.Live;
}

/** @brief state machine cycle
  * @retval error condition
  */
SM_Error_t SM_Cycle(void)
{
  if (SM_Handle.CycleStatus == SM_CS_PAUSE)
  {
    return SM_WRN_PAUSED;
  }

  if (SM_Handle.CycleStatus == SM_CS_STOP)
  {
    return SM_WRN_STOPPED;
  }

  if (!SM_IsValidState(SM_Handle.CurrentState))
  {
    return SM_ERR_NO_RUNNABLE;
  }

  SM_Handle.Live.Timestamp = __GetSysTick();
  SM_Handle.pCurrentStateItem = SM_GetCurrentStateItem();

  SM_State_t next_state = SM_NONE;
  SM_State_t event = SM_NONE;

  /* check the current event (if present) */
  if (next_state == SM_NONE) __Cycle_Events(&next_state, &event);

  static uint8_t bSmFirstTime = 1;
  if(bSmFirstTime == 1)
  {
    bSmFirstTime = 0;
    if (SM_Handle.pCurrentStateItem && SM_Handle.pCurrentStateItem->EntryHandler) 
    {
      if(SM_Handle.pCurrentStateItem->bNeedReboot)
      {
        if(__LoadFirstState() != SM_Handle.CurrentState)
        {
          __SaveFirstState(SM_Handle.CurrentState);
          __Reboot();
        }
        else
        {
          __SaveFirstState(SM_ST_START_DEF);
        }
      }
      SM_Handle.pCurrentStateItem->EntryHandler(SM_TA_ENTRY, SM_Handle.PreviousState, event, SM_Handle.CurrentState);
    }
  }
  
  /* in case of no next state check the timeout */
  if (next_state == SM_NONE) __Cycle_Timeout(&next_state, &event);

  /* move to next state */
  if (SM_IsValidState(next_state))
  {
    /* call the exit handler if the current status is valid and the handler is available */
    if (SM_Handle.pCurrentStateItem && SM_Handle.pCurrentStateItem->ExitHandler) SM_Handle.pCurrentStateItem->ExitHandler(SM_TA_EXIT, SM_Handle.CurrentState, event, next_state);

    /* perform the transition */
    SM_Handle.PreviousState = SM_Handle.CurrentState;
    SM_Handle.CurrentState = next_state;
    __HandleReset();

#if SM_USE_LOG
    __LogTransition(SM_Handle.PreviousState, event, SM_Handle.CurrentState);
#endif /* SM_USE_LOG */

    /* call the entry handler if available */
    SM_Handle.pCurrentStateItem = SM_GetStateItem(SM_Handle.CurrentState);
    if (SM_Handle.pCurrentStateItem && SM_Handle.pCurrentStateItem->EntryHandler) 
    {
      if(SM_Handle.pCurrentStateItem->bNeedReboot)
      {
        if(__LoadFirstState() != SM_Handle.CurrentState)
        {
          __SaveFirstState(SM_Handle.CurrentState);
          __Reboot();
        }
        else
        {
          __SaveFirstState(SM_ST_START_DEF);
        }
      }
      SM_Handle.pCurrentStateItem->EntryHandler(SM_TA_ENTRY, SM_Handle.PreviousState, event, SM_Handle.CurrentState);
    }
  }
  else
  {
    SM_Handle.Live.Tick++;
  }
  
  /* Call the module handlers and calculate the number of error modules */
  __Cycle_Modules();
  
  if(SM_Handle.pCurrentStateItem->bNeedReboot)
  {
    if(__LoadFirstState() != SM_Handle.CurrentState)
    {
      __Reboot();
    }
  }
  
#if SM_USE_LOG
  if (next_state == SM_ST_INVALID)
  {
    __LogTransition(SM_Handle.CurrentState, event, SM_ST_INVALID);
  }
#endif /* SM_USE_LOG */

  return SM_OK;
}
/**
  * @} SM_MODULE_Engine_Exported_functions_main
  */

/** @addtogroup SM_MODULE_Engine_Exported_functions_state
  * @{
  */

/** @brief check if it is a valid state
  * @param state state
  * @retval 1 if is valid, 0 otherwise
  */
uint8_t SM_IsValidState(SM_State_t state)
{
  return state >= 0 && state < SM_Handle.Config->NumStates;
}

/** @brief get the item of the current state
  * @retval state item pointer
  */
SM_StateItem_t const * SM_GetCurrentStateItem(void)
{
  return SM_GetStateItem(SM_Handle.CurrentState);
}

/** @brief get the item of the specified state
  * @param state state
  * @retval state item pointer
  */
SM_StateItem_t const * SM_GetStateItem(SM_State_t state)
{
  return SM_IsValidState(state) ? &SM_Handle.Config->States[state] : NULL;
}

/** @brief get the next state according state and event
  * @param state state
  * @param event event
  * @retval next state or SM_NONE
  */
SM_State_t SM_GetNextState(SM_State_t state, SM_Event_t event)
{
  SM_State_t s = SM_NONE;
  if (event == SM_EV_RETURN)
  {
    s = SM_Handle.ReturnState != SM_NONE ? SM_Handle.ReturnState : SM_Handle.PreviousState;
  }
  else
  {
    SM_Transition_t t;
    SM_TransitionItem_t const * ptrans_item = NULL;

    for(t = 0; t < SM_Handle.Config->NumTransitions; t++)
    {
      ptrans_item = &SM_Handle.Config->Transitions[t];
      if (((ptrans_item->StartState == state) || (ptrans_item->StartState == SM_ST_ANY))
          && (ptrans_item->Event == event))
      {
        s = ptrans_item->EndState;
        break;
      }
    }
  }
  if (s == SM_ST_RET) s = SM_Handle.ReturnState;

  return s;
}

#if SM_USE_NAME
/** @brief get state name
  * @param state state
  * @retval pointer to text
  */
const char * SM_GetStateName(SM_State_t state)
{
  if (state == SM_ST_INVALID)
  {
    return SM_ST_INVALID_TEXT;
  }
  SM_StateItem_t  const * pstate = SM_GetStateItem(state);
  return pstate ? pstate->Name  : SM_UNKNOWN_TEXT;
}

/** @brief get a state with name
  * @param name name of the state
  * @retval state
  */
SM_State_t SM_GetStateWithName(const char * name)
{
  SM_State_t state;
  for(state = 0; state < SM_Handle.Config->NumStates; state++)
  {
    if (strcmp(SM_Handle.Config->States[state].Name, name) == 0)
      break;
  }
  return SM_IsValidState(state) ? state : SM_NONE;
}
#endif

/** @brief set the specified state as return state
  * @param state
  * @retval error condition
  */
SM_Error_t SM_SetReturnState(SM_State_t state)
{
  SM_Handle.ReturnState = state;
  return SM_OK;
}

/** @brief set the current state as return state
  * @retval error condition
  */
SM_Error_t SM_SetCurrentStateAsReturn(void)
{
  return SM_SetReturnState(SM_Handle.CurrentState);
}

/** @brief clear the return state
  * @retval error condition
  */
SM_Error_t SM_ClearReturnState(void)
{
  return SM_SetReturnState(SM_NONE);
}
/**
  * @} SM_MODULE_Engine_Exported_functions_state
  */

/** @addtogroup SM_MODULE_Engine_Exported_functions_event
  * @{
  */

/** @brief check if it is a special event
  * @param event event
  * @retval 1 if is special, 0 otherwise
  */
uint8_t SM_IsSpecialEvent(SM_Event_t event)
{
  return (event >= SM_EV_SP_BASE && event <= SM_EV_SP_LAST);
}

/** @brief check if it is a valid event
  * @param event event
  * @retval 1 if is valid, 0 otherwise
  */
uint8_t SM_IsValidEvent(SM_Event_t event)
{
  return ( event >= 0 && event < SM_Handle.Config->NumEvents ) || event == SM_EV_RETURN;
}

/** @brief get the item of the specified event
  * @param event event
  * @retval event item pointer
  */
SM_EventItem_t const * SM_GetEventItem(SM_Event_t event)
{
  return SM_IsValidEvent(event) ? &SM_Handle.Config->Events[event] : NULL;
}

#if SM_USE_NAME
/** @brief get event name
  * @param event event
  * @retval pointer to text
  */
const char * SM_GetEventName(SM_Event_t event)
{
  char const * name = NULL;
  SM_EventItem_t const * pevent = NULL;
  switch(event)
  {
  case SM_EV_EPSILON:
    name = SM_EV_EPSILON_TEXT;
    break;
  case SM_EV_RETURN:
    name = SM_EV_RETURN_TEXT;
    break;
  default:
    pevent = SM_GetEventItem(event);
    name = pevent ? pevent->Name  : SM_UNKNOWN_TEXT;
    break;
  }
  return name;
}

/** @brief get an event with name
  * @param name name of the event
  * @retval event
  */
SM_Event_t SM_GetEventWithName(const char * name)
{
  SM_Event_t event;
  for(event = 0; event < SM_Handle.Config->NumEvents; event++)
  {
    if (strcmp(SM_Handle.Config->Events[event].Name, name) == 0)
      break;
  }
  return SM_IsValidEvent(event) ? event : SM_NONE;
}
#endif
/**
  * @} SM_MODULE_Engine_Exported_functions_event
  */

/** @addtogroup SM_MODULE_Engine_Exported_functions_transition
  * @{
  */

/** @brief check if it is a valid transition
  * @param transition transition
  * @retval 1 if is valid, 0 otherwise
  */
uint8_t SM_IsValidTransition(SM_Event_t transition)
{
  return transition >= 0 && transition < SM_Handle.Config->NumTransitions;
}

/** @brief get the item of the specified transition
  * @param transition transition
  * @retval transition item pointer
  */
SM_TransitionItem_t const * SM_GeTransitionItem(SM_Transition_t transition)
{
  return SM_IsValidTransition(transition) ? &SM_Handle.Config->Transitions[transition] : NULL;
}

/** @brief get a transition with the specified state and event
  * @param state state
  * @param event event
  * @retval transition
  */
SM_Transition_t SM_GetTransitionWithStateEvent(SM_State_t state, SM_Event_t event)
{
  SM_Transition_t t;
  SM_State_t s;
  SM_Event_t e;

  for(t = 0; t < SM_Handle.Config->NumTransitions; t++)
  {
    s = SM_Handle.Config->Transitions[t].StartState;
    e = SM_Handle.Config->Transitions[t].Event;
    if (((s == state) || (s == SM_ST_ANY)) && (e == event))
    {
      break;
    }
  }

  return t >= SM_Handle.Config->NumTransitions ? SM_NONE : t;
}
/**
  * @} SM_MODULE_Engine_Exported_functions_transition
  */

/** @addtogroup SM_MODULE_Engine_Exported_functions_trigger
  * @{
  */

/** @brief fire an event with the specified priority
  * @param event event
  * @param priority priority
  * @retval error condition
  */
SM_Error_t SM_Trigger(SM_Event_t event, SM_EventPriority_t priority)
{
  /* implement here a queue */
  if (priority == SM_PRIO_CRITICAL)
  {
    SM_Handle.CriticalEvent = event;
    return SM_OK;
  }

  if (SM_Handle.LastEvent != SM_NONE && priority < SM_Handle.EventPriority)
  {
    return SM_WRN_NO_PRIORITY;
  }

  SM_Handle.EventPriority = priority;
  SM_Handle.LastEvent = event;

  return SM_OK;
}

/** @brief fire an event with the normal priority
  * @param event event
  * @retval error condition
  */
SM_Error_t SM_NormalTrigger(SM_Event_t event)
{
  return SM_Trigger(event, SM_PRIO_NORMAL);
}

/** @brief fire an event with the critical priority
  * @param event event
  * @retval error condition
  */
SM_Error_t SM_CriticalTrigger(SM_Event_t event)
{
  return SM_Trigger(event, SM_PRIO_CRITICAL);
}
/**
  * @} SM_MODULE_Engine_Exported_functions_trigger
  */

/** @addtogroup SM_MODULE_Engine_Exported_functions_module
  * @{
  */

/** @brief check if it is a valid module
  * @param module module
  * @retval 1 if is valid, 0 otherwise
  */
uint8_t SM_IsValidModule(SM_Module_t module)
{
  return module >= 0 && module < SM_Handle.Config->NumModules;
}

/** @brief get the item of the specified module
  * @param module module
  * @retval module item pointer
  */
SM_ModuleItem_t const * SM_GetModuleItem(SM_Module_t module)
{
  return SM_IsValidModule(module) ? &SM_Handle.Config->Modules[module] : NULL;
}

#if SM_USE_NAME
/** @brief get module name
  * @param module module
  * @retval pointer to text
  */
const char * SM_GetModuleName(SM_Module_t module)
{
  SM_ModuleItem_t  const * pmodule = SM_GetModuleItem(module);
  return pmodule ? pmodule->Name  : SM_UNKNOWN_TEXT;
}

/** @brief get an module with name
  * @param name name of the module
  * @retval module
  */
SM_Module_t SM_GetModuleWithName(const char * name)
{
  SM_Module_t module;
  for(module = 0; module < SM_Handle.Config->NumModules; module++)
  {
    if (strcmp(SM_Handle.Config->Modules[module].Name, name) == 0)
      break;
  }
  return SM_IsValidModule(module) ? module : SM_NONE;
}
#endif
/**
  * @} SM_MODULE_Engine_Exported_functions_module
  */

/**
  * @} SM_MODULE_Engine
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

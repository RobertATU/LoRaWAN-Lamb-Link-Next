/**
  ******************************************************************************
  * @file    SM_Engine.h
  * @author  SRA Team
  * @version V1.0.0
  * @brief   Header file for SM_Engine.c file in SM Module
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

#ifndef _SM_ENGINE_H
#define _SM_ENGINE_H

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup SM_MODULE State Machine (SM)
  * @brief <p>State machine with module management and timeo-out
  *        customization with template files, the user have to be specified:
  *        <ul>
  *        <li>States</li>
  *        <li>Events</li>
  *        <li>Transitions</li>
  *        <li>Modules</li>
  *        </ul>
  *        </p>
  *        <p>For each state it is possible to specify an entry and exit handler
  *        </p>
  *        <p>For each state and for each module it is possible to specify a module handler
  *        </p>
  * @note  To reduce the footprint the user have to apply the follow recommendations:
  *        <ul>
  *        <li>the states, events and modules have to be 0-index (the number is the index in the array)</li>
  *        <li>USE_NAME remove all name fields and linked functions</li>
  *        </ul>
  * @{
  */

/** @defgroup SM_MODULE_Engine Engine
  * @brief the Engine implements the state machine cycle and manage transitions
  * @{
  */
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef LIBRARY
#include "SM_Conf.h"
#endif

/* Exported constants --------------------------------------------------------*/
/** @defgroup SM_MODULE_Engine_Exported_const exported constants
  * @brief <i>Exported constants</i>
  * @{
  */

/** @defgroup SM_MODULE_Engine_Exported_const_misc misc
  * @brief <i>Miscellany</i>
  * @{
  */
#define SM_NONE              (-1)               /*!< none                       */
#define SM_NODATA             (0)               /*!< no data                    */
#define SM_DONT_CARE       (0xFF)               /*!< don't care                 */
#define SM_NO_TIMEOUT        (-1)               /*!< no timeout                 */

#if SM_USE_NAME
#define SM_NAME_SIZE         (15)               /*!< size of name               */
#endif /* SM_USE_NAME */
/**
  * @} SM_MODULE_Engine_Exported_const_misc
  */

/** @defgroup SM_MODULE_Engine_Exported_const_state state
  * @brief <i>State</i>
  * @{
  */
#define SM_ST_MAX_NUM           10              /*!< Maximum number of states   */
#define SM_ST_START_DEF ((SM_State_t)0x00)      /*!< Default start state        */
#define SM_ST_ANY       ((SM_State_t)0xE0)      /*!< Any state definition       */
#define SM_ST_RET       ((SM_State_t)0xE1)      /*!< Any state definition       */
#define SM_ST_INVALID           (-2)            /*!< Invalid state definition   */
/**
  * @} SM_MODULE_Engine_Exported_const_state
  */

/** @defgroup SM_MODULE_Engine_Exported_const_event event
  * @brief <i>Event</i>
  * @{
  */
#define SM_EV_SP_BASE       (0x40)              /*!< base special events        */
#define SM_EV_EPSILON       (SM_EV_SP_BASE + 0) /*!< epsilon event              */
#define SM_EV_RETURN        (SM_EV_SP_BASE + 1) /*!< return event               */
#define SM_EV_SP_LAST       SM_EV_RETURN        /*!< last special event         */
/**
  * @} SM_MODULE_Engine_Exported_const_event
  */

/** @defgroup SM_MODULE_Engine_Exported_const_handler handler mode
  * @brief <i>Mode</i>
  * @{
  */
#define SM_TA_EXIT              0               /*!< identify an exit action    */
#define SM_TA_ENTRY             1               /*!< identify an entry action   */
/**
  * @} SM_MODULE_Engine_Exported_const_handler
  */

/** @defgroup SM_MODULE_Engine_Exported_const_ret return
  * @brief <i>Return</i>
  * @{
  */
#define SM_OK                   0               /*!< ok, no error               */
#define SM_ERR_GENERIC         -1               /*!< error: generic             */
#define SM_ERR_NO_RUNNABLE     -2               /*!< error: no runnable         */
#define SM_ERR_INVALID_PARAM   -3               /*!< error: invalid parameter   */
#define SM_WRN_PAUSED        -100               /*!< warning: paused            */
#define SM_WRN_STOPPED       -101               /*!< warning: stopped           */
#define SM_WRN_WAITING       -102               /*!< warning: waiting           */
#define SM_WRN_NO_PRIORITY   -103               /*!< warning: no priority       */
#define SM_WRN_BUSY          -104               /*!< warning: busy              */
/**
  * @} SM_MODULE_Engine_Exported_const_ret
  */

/**
  * @} SM_MODULE_Engine_Exported_const
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup SM_MODULE_Engine_Exported_macro exported macro
  * @brief <i>Exported macro</i>
  * @{
  */
#if SM_USE_NAME
#define SET_NAME(NAME) .Name = NAME,            /*!< set name definition        */
#define DEF_NAME(SIZE) char Name[SIZE];         /*!< name field definition      */
#else  /* SM_USE_NAME */
#define SET_NAME(NAME)                          /*!< empty set name             */
#define DEF_NAME(SIZE)                          /*!< empty name field           */
#endif /* SM_USE_NAME */

/** @brief Add a state with timeout
  * @param name         name of the state
  * @param state        state id
  * @param entryHandler handler called on entry action
  * @param exitHandler  handler called on exit action
  * @param toMode       timeout mode
  *          @arg @ref SM_TO_MODE_NONE          no timeout
  *          @arg @ref SM_TO_MODE_FIXED         fire the event after the timeout
  *          @arg @ref SM_TO_MODE_OK_ENTRY      fire the event in case of error for timeout in entry
  *          @arg @ref SM_TO_MODE_OK_WATCH_DOG  fire the event in case of error for timeout
  * @param toValue      timeout value
  * @param toEvent      event to fire
  */
#define SM_ADD_STATE_TO(name, state, entrymode, entryHandler, exitHandler, toMode, toValue, toEvent) { \
  SET_NAME(name) \
  .State        = state, \
  .TimeoutConf.Mode  = toMode, \
  .TimeoutConf.Value = toValue, \
  .TimeoutConf.Event = toEvent, \
  .EntryHandler = entryHandler, \
  .ExitHandler  = exitHandler, \
  .bNeedReboot  = entrymode, \
}
/** @brief Add a state without timeout
  * @param name         name of the state
  * @param state        state id
  * @param entryHandler handler called on entry action
  * @param exitHandler  handler called on exit action
  */
#define SM_ADD_STATE(name, state, entrymode, entryHandler, exitHandler) SM_ADD_STATE_TO(name, state, entrymode, entryHandler, exitHandler, SM_TO_MODE_NONE, SM_NO_TIMEOUT, SM_NONE)

/** @brief Add an event
  * @param name         name of the state
  * @param event        event id
  */
#define SM_ADD_EVENT(name, event) { \
  SET_NAME(name) \
  .Event = event, \
}
/** @brief Add a module, with the handlers
  * @param name         name of the state
  * @param module       module id
  * @param ...          list of handlers
  */
#define SM_ADD_MODULE(name, module, ...) { \
  SET_NAME(name) \
  .Module = module, \
  .Handlers = {__VA_ARGS__}, \
}
/** @brief Add a transition
  * @param startState   start state
  * @param event        event
  * @param endState     end state
  */
#define SM_ADD_TRANSITION(startState, event, endState) { \
  .StartState = startState, \
  .Event = event, \
  .EndState = endState, \
}
/**
  * @} SM_MODULE_Engine_Exported_macro
  */

/* Exported typedef ----------------------------------------------------------*/
/** @defgroup SM_MODULE_Engine_Exported_typedef exported typedef
  * @brief <i>Exported typedef</i>
  * @{
  */
/** @defgroup SM_MODULE_Engine_Exported_typedef_base base
  * @brief <i>Base</i>
  * @{
  */
typedef int8_t SM_State_t;                      /*!< state type definition      */
typedef int8_t SM_Event_t;                      /*!< event type definition      */
typedef int8_t SM_Transition_t;                 /*!< transition type definition */
typedef int8_t SM_Module_t;                     /*!< module type definition     */
typedef int8_t SM_Error_t;                      /*!< error type definition      */

/**
  * @} SM_MODULE_Engine_Exported_typedef_base
  */

/** @defgroup SM_MODULE_Engine_Exported_typedef_handler handler
  * @brief <i>Handler</i>
  * @{
  */
/** @brief module handler typedef
  * @param module       module
  * @param state        current state
  * @retval error condition
  */
typedef SM_Error_t (*SM_ModuleHandler_t)(SM_Module_t module, SM_State_t state);

/** @brief module handler typedef
  * @param typeAction   type action of the caller
  *          @arg @ref SM_TA_EXIT          exit action
  *          @arg @ref SM_TA_ENTRY         entry action
  * @param startState   start state
  * @param event        event
  * @param endState     end state
  * @retval error condition
  */
typedef SM_Error_t (*SM_StateHandler_t)(uint8_t typeAction, SM_State_t startState, SM_Event_t event, SM_State_t endState);

/** @brief get systick handler typedef
  * @retval error condition
  */
typedef uint32_t   (*SM_GetSysTick_t)(void);

typedef SM_State_t   (*SM_LoadFirstState_t)(void);

typedef void   (*SM_SaveFirstState_t)(SM_State_t);

typedef void   (*SM_Reboot_t)(void);


#if SM_USE_LOG
/** @brief log transition handler typedef
  * @param startState   start state
  * @param event        event
  * @param endState     end state
  */
typedef void       (*SM_LogTransition_t)(SM_State_t startState, SM_Event_t event, SM_State_t endState);
#endif

/**
  * @} SM_MODULE_Engine_Exported_typedef_handler
  */

/** @defgroup SM_MODULE_Engine_Exported_typedef_enum enum
  * @brief <i>Enumeration</i>
  * @{
  */
/** @brief time-out mode
  */
typedef enum
{
  SM_TO_MODE_NONE          = 0u,                /*!< no timeout                                            */
  SM_TO_MODE_FIXED         = 1u,                /*!< fire the event after the timeout                      */
  SM_TO_MODE_OK_ENTRY      = 2u,                /*!< fire the event in case of error for timeout in entry  */
  SM_TO_MODE_OK_WATCH_DOG  = 3u,                /*!< fire the event in case of error for timeout           */
} SM_TO_Mode_t;

/** @brief entry mode
  */
typedef enum
{
  SM_ENTRY_MODE_NM         = 0u,                /*!< entry mode normal                                     */
  SM_ENTRY_MODE_RB         = 1u,                /*!< reboot needed to enter the mode                       */
} SM_ENTRY_Mode_t;

/** @brief state machine status
  */
typedef enum
{
  SM_CS_STOP               = 0u,                /*!< stopped status (started from default state)            */
  SM_CS_RUN                = 1u,                /*!< running status                                         */
  SM_CS_PAUSE              = 2u,                /*!< paused                                                 */
} SM_CycleStatus_t;

/** @brief priority event
  */
typedef enum
{
  SM_PRIO_NONE             = 0u,                /*!< no priority                                            */
  SM_PRIO_LOW              = 1u,                /*!< low priority                                           */
  SM_PRIO_NORMAL           = 2u,                /*!< normal priority                                        */
  SM_PRIO_HIGH             = 3u,                /*!< high priority                                          */
  SM_PRIO_CRITICAL         = 4u,                /*!< critical priority (these events are not discarded)     */
} SM_EventPriority_t;
/**
  * @} SM_MODULE_Engine_Exported_typedef_enum
  */

/** @defgroup SM_MODULE_Engine_Exported_typedef_struct struct
  * @brief <i>Structure</i>
  * @{
  */
/** @brief time-out configuration
  */
typedef struct
{
  SM_TO_Mode_t Mode;                            /*!< mode of time-out @ref SM_TO_Mode_t                     */
  int32_t Value;                                /*!< time-out value, set to SM_NO_TIMEOUT                   */
  SM_Event_t Event;                             /*!< event to fire in case of time-out                      */
} SM_TO_Conf_t;

/** @brief state item
  */
typedef struct
{
  DEF_NAME(SM_NAME_SIZE)                        /*!< name field definition      */
  SM_State_t State;                             /*!< state                      */
  SM_TO_Conf_t TimeoutConf;                     /*!< timeout configuration field*/
  SM_StateHandler_t ExitHandler;                /*!< exit action handler        */
  SM_StateHandler_t EntryHandler;               /*!< entry action handler       */
  SM_ENTRY_Mode_t bNeedReboot;
} SM_StateItem_t;

/** @brief event item
  */
typedef struct
{
  DEF_NAME(SM_NAME_SIZE)                        /*!< name field definition      */
  SM_Event_t Event;                             /*!< event                      */
} SM_EventItem_t;

/** @brief module item
  */
typedef struct
{
  DEF_NAME(SM_NAME_SIZE)                        /*!< name field definition      */
  SM_Module_t Module;                           /*!< module                     */

  /* Add here the functions */
  SM_ModuleHandler_t Handlers[SM_ST_MAX_NUM];   /*!< handler of modules         */

} SM_ModuleItem_t;

/** @brief transition item
  */
typedef struct
{
  SM_State_t StartState;                        /*!< start state                */
  SM_Event_t Event;                             /*!< event                      */
  SM_State_t EndState;                          /*!< end state                  */
} SM_TransitionItem_t;

/** @brief configuration structure
  */
typedef struct
{
  SM_State_t StartState;                        /*!< start state                */
  uint8_t NumStates;                            /*!< number of states           */
  uint8_t NumEvents;                            /*!< number of events           */
  uint8_t NumTransitions;                       /*!< number of transitions      */
  uint8_t NumModules;                           /*!< number of modules          */

  const SM_StateItem_t * States;                /*!< pointer to states configuration      */
  const SM_EventItem_t * Events;                /*!< pointer to events configuration      */
  const SM_TransitionItem_t * Transitions;      /*!< pointer to transitions configuration */
  const SM_ModuleItem_t * Modules;              /*!< pointer to modules configuration     */
} SM_Config_t;

/** @brief live variable
  */
typedef struct
{
  uint32_t Tick;                                /*!< number of ticks for current  state */
  uint32_t Timestamp;                           /*!< timestamp                          */
  int32_t  Timeout;                             /*!< timeout                            */
  int32_t ModuleErrorCount;                     /*!< counting of module error           */
} SM_Live_t;

/** @brief handle
  */
typedef struct
{
  /* Functions */
  SM_GetSysTick_t       GetSysTickFunc;         /*!< get sys tick handler               */
  
  SM_LoadFirstState_t   LoadFirstStateFunc;     /*!< load first state handler               */
  SM_SaveFirstState_t   SaveFirstStateFunc;     /*!< Save first state handler               */
  SM_Reboot_t           RebootFunc;             /*!< reboot handler               */
  
#if SM_USE_LOG
  SM_LogTransition_t LogTransitionFunc;         /*!< log transition handler             */
#endif /* SM_USE_LOG */
} SM_Handlers_t;

/** @brief handle
  */
typedef struct
{
#if SM_USE_NAME
  const char * Name;                            /*!< name field definition      */
#endif
  SM_CycleStatus_t CycleStatus;                 /*!< current cycle status       */
  SM_State_t PreviousState;                     /*!< previous state             */
  SM_State_t CurrentState;                      /*!< current state              */
  SM_State_t ReturnState;                       /*!< return state               */

  SM_StateItem_t const * pCurrentStateItem;     /*!< item of the current state  */

  SM_Event_t CriticalEvent;                     /*!< critical event             */
  SM_Event_t LastEvent;                         /*!< last event                 */
  SM_EventPriority_t EventPriority;             /*!< priority of the last event */
  SM_Live_t Live;                               /*!< live variables             */

  const SM_Config_t * Config;                   /*!< pointer to configuration   */

  SM_Handlers_t Handlers;                       /*!< handlers                   */

} SM_Handle_t;
/**
  * @} SM_MODULE_Engine_Exported_typedef_struct
  */
/**
  * @} SM_MODULE_Engine_Exported_typedef
  */

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/** @defgroup SM_MODULE_Engine_Exported_functions exported functions
  * @brief <i>Exported functions</i>
  * @{
  */

/** @defgroup SM_MODULE_Engine_Exported_functions_main main
  * @brief <i>Main</i>
  * @{
  */
void SM_Init(const char * name, const SM_Config_t * config, SM_Handlers_t * handlers);
SM_Error_t SM_SetHandlers(SM_Handlers_t * handlers);
SM_Error_t SM_GetHandlers(SM_Handlers_t * handlers);

#if SM_USE_NAME
void SM_SetName(const char * name);
#else
/** @brief empty definition of SM_SetName
  */
#define SM_SetName(name)
#endif

SM_CycleStatus_t SM_GetCycleStatus(void);
SM_Error_t SM_SetCycleStatus(SM_CycleStatus_t cycleStatus);

uint8_t SM_IsRunning(void);
SM_Error_t SM_Start(void);
SM_Error_t SM_Stop(void);
SM_Error_t SM_Pause(void);

SM_Error_t SM_Cycle(void);
SM_Live_t * SM_GetLive(void);
/**
  * @} SM_MODULE_Engine_Exported_functions_main
  */

/** @defgroup SM_MODULE_Engine_Exported_functions_state state
  * @brief <i>State</i>
  * @{
  */
uint8_t SM_IsValidState(SM_State_t state);
SM_StateItem_t const * SM_GetCurrentStateItem(void);
SM_StateItem_t const * SM_GetStateItem(SM_State_t state);
SM_State_t SM_GetNextState(SM_State_t state, SM_Event_t event);

#if SM_USE_NAME
const char * SM_GetStateName(SM_State_t state);
SM_State_t SM_GetStateWithName(const char * name);
#else
/** @brief empty definition of SM_GetStateWithName
  */
#define SM_GetStateName(state)    (NULL)
#define SM_GetStateWithName(name) (SM_NONE)
#endif

SM_Error_t SM_SetReturnState(SM_State_t state);
SM_Error_t SM_SetCurrentStateAsReturn(void);
SM_Error_t SM_ClearReturnState(void);
/**
  * @} SM_MODULE_Engine_Exported_functions_state
  */

/** @defgroup SM_MODULE_Engine_Exported_functions_event event
  * @brief <i>Event</i>
  * @{
  */
uint8_t SM_IsSpecialEvent(SM_Event_t event);
uint8_t SM_IsValidEvent(SM_Event_t event);
SM_EventItem_t const * SM_GetEventItem(SM_Event_t event);
#if SM_USE_NAME
const char * SM_GetEventName(SM_Event_t event);
SM_Event_t SM_GetEventWithName(const char * name);
#else
/** @brief empty definition of SM_GetEventWithName
  */
#define SM_GetEventName(event)    (NULL)
#define SM_GetEventWithName(name) (SM_NONE)
#endif
/**
  * @} SM_MODULE_Engine_Exported_functions_event
  */

/** @defgroup SM_MODULE_Engine_Exported_functions_transition transition
  * @brief <i>None</i>
  * @{
  */
uint8_t SM_IsValidTransition(SM_Event_t transition);
SM_TransitionItem_t const * SM_GeTransitionItem(SM_Transition_t transition);
SM_Transition_t SM_GetTransitionWithStateEvent(SM_State_t state, SM_Event_t event);
/**
  * @} SM_MODULE_Engine_Exported_functions_transition
  */

/** @defgroup SM_MODULE_Engine_Exported_functions_trigger trigger
  * @brief <i>Trigger</i>
  * @{
  */
SM_Error_t SM_Trigger(SM_Event_t event, SM_EventPriority_t priority);
SM_Error_t SM_NormalTrigger(SM_Event_t event);
SM_Error_t SM_CriticalTrigger(SM_Event_t event);
/**
  * @} SM_MODULE_Engine_Exported_functions_trigger
  */

/** @defgroup SM_MODULE_Engine_Exported_functions_module module
  * @brief <i>Module</i>
  * @{
  */
/* Module API */
uint8_t SM_IsValidModule(SM_Module_t module);
SM_ModuleItem_t const * SM_GetModuleItem(SM_Module_t module);
#if SM_USE_NAME
const char * SM_GetModuleName(SM_Module_t module);
SM_Module_t SM_GetModuleWithName(const char * name);
#else
/** @brief empty definition of SM_GetModuleWithName
  */
#define SM_GetModuleName(module)   (NULL)
#define SM_GetModuleWithName(name) (SM_NONE)
#endif
/**
  * @} SM_MODULE_Engine_Exported_functions_module
  */

/**
  * @} SM_MODULE_Engine_Exported_functions
  */

/**
  * @} SM_MODULE_Engine
  */

/**
  * @} SM_MODULE
  */

#ifdef __cplusplus
}
#endif

#endif /* _SM_ENGINE_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

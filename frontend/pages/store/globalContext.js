// Lets do all database stuff here and just share this global context with the rest of the App
// - so no database code anywhere else in our App
// - every CRUD function the App needs to do is in here, in one place
// - makes debugging etc so much easier
// - all external connections still have to go through /api routes 

import { createContext, useState, useEffect } from 'react'

const GlobalContext = createContext()

export function GlobalContextProvider(props) {
    const [globals, setGlobals] = useState({ aString: 'init val', count: 0, hideHamMenu: true,pins: [],mapCenter:[-9.0105,53.2787] , dataLoaded: false })

 
    useEffect(() => {
        getAllPins()
    }, []);


    async function getAllPins() {
        const response = await fetch('/api/get-pins', {
            method: 'Get',
            headers: {
                'Content-Type': 'application/json'
            }
        });
        let data = await response.json();
        for(let i = 0;i<data.length;i++){
            console.log(data[i]);
        }
        
        setGlobals((previousGlobals) => { const newGlobals = JSON.parse(JSON.stringify(previousGlobals)); newGlobals.pins = data;newGlobals.dataLoaded = true; return newGlobals })
    }
    async function deletePinById(sheepId){
        console.log(sheepId)
        await fetch('/api/delete-pins',{
        method:'Post',
        headers:{
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(sheepId),
       });
       await getAllPins();
    }

    async function editGlobalData(command) { // {cmd: someCommand, newVal: 'new text'}
        if (command.cmd == 'hideHamMenu') { // {cmd: 'hideHamMenu', newVal: false} 
            //  WRONG (globals object reference doesn't change) and react only looks at its 'value' aka the reference, so nothing re-renders:
            //    setGlobals((previousGlobals) => { let newGlobals = previousGlobals; newGlobals.hideHamMenu = command.newVal; return newGlobals })
            // Correct, we create a whole new object and this forces a re-render:
            setGlobals((previousGlobals) => {
                const newGlobals = JSON.parse(JSON.stringify(previousGlobals));
                newGlobals.hideHamMenu = command.newVal; return newGlobals
            })
        }
      
        if (command.cmd == 'updateMapCenter') {
            
            setGlobals((previousGlobals) => {
                const newGlobals = JSON.parse(JSON.stringify(previousGlobals))
                newGlobals.mapCenter = command.newCenter
                return newGlobals
            });
        }

    }

    const context = {
        updateGlobals: editGlobalData,
        deletePins: deletePinById,
        getAll:getAllPins,
        theGlobalObject: globals
    }

    return <GlobalContext.Provider value={context}>
        {props.children}
    </GlobalContext.Provider>
}


export default GlobalContext

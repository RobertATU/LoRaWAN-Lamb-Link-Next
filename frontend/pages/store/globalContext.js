// Lets do all database stuff here and just share this global context with the rest of the App
// - so no database code anywhere else in our App
// - every CRUD function the App needs to do is in here, in one place
// - makes debugging etc so much easier
// - all external connections still have to go through /api routes

import { createContext, useState, useEffect } from "react";

const GlobalContext = createContext();

export function GlobalContextProvider(props) {
  const [globals, setGlobals] = useState({
    aString: "init val",
    count: 0,
    hideHamMenu: true,
    pins: [],
    mapCenter: [-7.503, 53.4495],
    zoom: 6.5,
    dataLoaded: false,
  });

  useEffect(() => {
    getAllPins();
  }, []);

  async function getAllPins() {
    const response = await fetch("/api/get-pins", {
      method: "Get",
      headers: {
        "Content-Type": "application/json",
      },
    });
    let data = await response.json();
    for (let i = 0; i < data.length; i++) {
      data[i].id.date = new Date(data[i].id.date).toLocaleString("en-GB", {
        dateStyle: "short",
        timeStyle: "short",
      });
    }

    setGlobals((previousGlobals) => {
      const newGlobals = JSON.parse(JSON.stringify(previousGlobals));
      newGlobals.pins = data;
      newGlobals.dataLoaded = true;
      return newGlobals;
    });
  }

  async function updateAllPins(center,zoom) {
    console.log(globals)
    const response = await fetch("/api/get-pins", {
      method: "Get",
      headers: {
        "Content-Type": "application/json",
      },
    });
    let data = await response.json();
    for (let i = 0; i < data.length; i++) {
      data[i].id.date = new Date(data[i].id.date).toLocaleString("en-GB", {
        dateStyle: "short",
        timeStyle: "short",
      });
    }

    setGlobals((previousGlobals) => {
        if(globals.pins.length !== previousGlobals.pins.length ){
      const newGlobals = JSON.parse(JSON.stringify(previousGlobals));
      newGlobals.pins = data;
      newGlobals.mapCenter = center;
      newGlobals.zoom = zoom;
      newGlobals.dataLoaded = true;
      return newGlobals;}
      else{
      return previousGlobals;}
    });
  }

  async function deletePinById(sheepId) {
    console.log(sheepId);
    await fetch("/api/delete-pins", {
      method: "Post",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(sheepId),
    });
    await getAllPins();
  }

  async function editGlobalData(command) {
    if (command.cmd == "hideHamMenu") {
      setGlobals((previousGlobals) => {
        const newGlobals = JSON.parse(JSON.stringify(previousGlobals));
        newGlobals.hideHamMenu = command.newVal;
        return newGlobals;
      });
    }

    if (command.cmd == "updateMapCenter") {
      setGlobals((previousGlobals) => {
        const newGlobals = JSON.parse(JSON.stringify(previousGlobals));
        newGlobals.mapCenter = command.newCenter;
        newGlobals.zoom = 15;
        return newGlobals;
      });
    }

    if (command.cmd == "updateZoom") {
      setGlobals((previousGlobals) => {
        const newGlobals = JSON.parse(JSON.stringify(previousGlobals));
        newGlobals.mapCenter = [-7.503, 53.4495];
        newGlobals.zoom = 6.5;
        return newGlobals;
      });
    }
  }

  const context = {
    updateGlobals: editGlobalData,
    deletePins: deletePinById,
    getAll: getAllPins,
    updateAll: updateAllPins,
    theGlobalObject: globals,
  };

  return (
    <GlobalContext.Provider value={context}>
      {props.children}
    </GlobalContext.Provider>
  );
}

export default GlobalContext;

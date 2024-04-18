import { createContext, useState, useEffect } from "react";

const GlobalContext = createContext();

export function GlobalContextProvider(props) {
  const [globals, setGlobals] = useState({
    aString: "init val",
    count: 0,
    hideHamMenu: true,
    pins: [],
    mapCenter: [-8.503, 53.4495],
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

    setGlobals((previousGlobals) => {
      const newGlobals = JSON.parse(JSON.stringify(previousGlobals));
      newGlobals.pins = data;
      newGlobals.dataLoaded = true;
      return newGlobals;
    });
  }

  async function updateAllPins(center, zoom) {
    console.log(globals);
    const response = await fetch("/api/get-pins", {
      method: "Get",
      headers: {
        "Content-Type": "application/json",
      },
    });
    let data = await response.json();

    setGlobals((previousGlobals) => {
      const newGlobals = JSON.parse(JSON.stringify(previousGlobals));
      newGlobals.pins = data;
      newGlobals.mapCenter = center;
      newGlobals.zoom = zoom;
      newGlobals.dataLoaded = true;
      return newGlobals;
    });
  }

  async function deletePinById(genId) {
    console.log(genId);
    await fetch("/api/delete-pins", {
      method: "Post",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(genId),
    });
  }

  async function editGlobalData(command) {
  
    if (command.cmd == "updateMapCenter") {
      setGlobals((previousGlobals) => {
        const newGlobals = JSON.parse(JSON.stringify(previousGlobals));
        newGlobals.mapCenter = command.newCenter;
        newGlobals.zoom = 20;
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

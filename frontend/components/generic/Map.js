import React, { useRef, useEffect, useState } from "react";
import mapboxgl from "mapbox-gl";
import 'mapbox-gl/dist/mapbox-gl.css';
import { useContext } from "react";
import Confirm from '../generic/Confirm';
import GlobalContext from "../../pages/store/globalContext"
import { confirmAlert } from 'react-confirm-alert'; // Import
import 'react-confirm-alert/src/react-confirm-alert.css' // Import css


mapboxgl.accessToken =
  "pk.eyJ1Ijoicm9iZXJ0bXVsZG9vbmZ5cCIsImEiOiJjbHBlajVqejIxZDc5MmhybGZha3N3dGF3In0.EmdczNp0fNiA_es8l1-y9Q";
function Map(props) {
  const [map, setMap] = useState(null);
  const globalCtx = useContext(GlobalContext)
  const mapCenter = globalCtx.theGlobalObject.mapCenter;
  const zoom = globalCtx.theGlobalObject.zoom;
  let pins =globalCtx.theGlobalObject.pins
 
 
  useEffect(() => {

    console.log(zoom)
    console.log(pins)
    const initializeMap = () => {
      
      const mapInst = new mapboxgl.Map({
        container: "map",
        style: "mapbox://styles/mapbox/streets-v12",
        center: mapCenter,
        zoom: zoom,
      });
      setMap(mapInst);
    };
    if (!map) {
      initializeMap();
     
    }
    else{
      map.setCenter(mapCenter);
      map.setZoom(zoom)
      
 
    }
  
 


    
      for (let i = 0;i < globalCtx.theGlobalObject.pins.length;i++){
        console.log(pins[i].id.date)
        var pin

        if(i ==  globalCtx.theGlobalObject.pins.length -1)
        { pin = new mapboxgl.Marker({ "color": "#b40219" }).setLngLat([pins[i].longitude, pins[i].latitude]).addTo(map);}

        else{
         pin = new mapboxgl.Marker().setLngLat([pins[i].longitude, pins[i].latitude]).addTo(map);}

          const pinPopContent = document.createElement('div');
          pinPopContent.innerHTML = `<h3>${pins[i].sheepId}</h3><p>Seen at: ${pins[i].id.date}</p><button id="deleteButton_${i}">Delete</button>`;

          const pinPopup =  new mapboxgl.Popup().setDOMContent(pinPopContent);
          pin.setPopup(pinPopup);
          

          pinPopContent.querySelector(`#deleteButton_${i}`).addEventListener("click",() =>{
            confirmAlert({
              title: 'Confirm to Delete',
              message: 'Are you want to delete this pin.',
              buttons: [
                {
                  label: 'Yes',
                  onClick: () => deletePin(i)
                  
                },
                {
                  label: 'No'
                }
              ]
            })

           })
        pin.getElement().addEventListener("click", () => {
          pin.togglePopup();
        });
      };

      const deletePin = async(i) => {
        await globalCtx.deletePins(pins[i].sheepId)
        await globalCtx.getAll();
         map.remove();
         initializeMap();
      
      }

    }, [map,mapCenter,zoom]);

      
      
    
  return (
    <div>
      <div id="map" style={{ width: "100%", height: "800px" }}></div>
    </div>
  );
}

export default Map;

import React, { useRef, useEffect, useState } from "react";
import mapboxgl from "mapbox-gl";
import 'mapbox-gl/dist/mapbox-gl.css';
import { useContext } from "react";
import GlobalContext from "../../pages/store/globalContext"

mapboxgl.accessToken =
  "pk.eyJ1Ijoicm9iZXJ0bXVsZG9vbmZ5cCIsImEiOiJjbHBlajVqejIxZDc5MmhybGZha3N3dGF3In0.EmdczNp0fNiA_es8l1-y9Q";
function Map(props) {
  const [map, setMap] = useState(null);

  const globalCtx = useContext(GlobalContext)
  const mapCenter = globalCtx.theGlobalObject.mapCenter;
 
 

  useEffect(() => {

    console.log(pins)
    const initializeMap = () => {
      const mapInst = new mapboxgl.Map({
        container: "map",
        style: "mapbox://styles/mapbox/streets-v12",
        center: mapCenter,
        zoom: 9,
      });
      setMap(mapInst);
    };
    if (!map) {
      initializeMap();
    }
    else{
      map.setCenter(mapCenter);

    }
  }, [map,mapCenter]);
 
  let pins =globalCtx.theGlobalObject.pins

    
      for (let i = 0;i < globalCtx.theGlobalObject.pins.length;i++){
        console.log(pins[i].id.date)

        const pin = new mapboxgl.Marker()
          .setLngLat([pins[i].longitude, pins[i].latitude])
          .addTo(map);
          const createdAtDate = new Date(pins[i].id.date)
          const formattedDate = createdAtDate.toLocaleDateString('en-GB');
        pin.setPopup(
          new mapboxgl.Popup().setHTML(`<h3>${pins[i].sheepId}</h3><p>Seen at: ${formattedDate}</p>`)
        );
        pin.getElement().addEventListener("click", () => {
          pin.togglePopup();
        });
      };
    
  return (
    <div>
      <div id="map" style={{ width: "100%", height: "800px" }}></div>
    </div>
  );
}

export default Map;
